#region Details 
// ------------------------------------------------ //
// Name:    ClusteringAlgorithm Class               //
// Author:  Chris McBain                            //
// Created: 170215                                  //
// Purpose:                                         //            
// - Implements DBSCAN clustering algorithm         //
// - Calculates clusters of nodes and their centers //
// ------------------------------------------------ //
#endregion

#region Includes
// Unity Includes //
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
#endregion

// ClusteringAlgorithm Class //
public class ClusteringAlgorithm : MonoBehaviour {
	// Unity Editor Variables //
	[ SerializeField ] private float      mMinimumDistance = 1.5f; // minimum distance between points for clustering
	[ SerializeField ] private int        mMinimumPoints   = 3;    // minimum number of points required to be a cluster
	[ SerializeField ] private GameObject mTiles           = null; // tile object reference   

	private SearchArea[]       mSearchAreas;                            // list of search areas
	private List< Cluster >    mClusters    = new List< Cluster >();    // list of clusters
	private List< SearchNode > mNoise       = new List< SearchNode >(); // list of search nodes that are not part of a cluster (NOISE)

	// Unity Functions //

	// Start                                //
	// Creates the clusters and noise lists //
	void Start () {
		mSearchAreas = mTiles.GetComponentsInChildren< SearchArea >();
	}

	// Public Functions //

	// GetClusters                                                  //
	// Returns an array of clusters determined by the latest DBSCAN //
	public Cluster[] GetClusters() {
		return mClusters.ToArray();
	}

	// ResetSearchAreas                  //
	// Resets all search areas variables //
	public void ResetSearchAreas() {
		for( int i = 0; i < mSearchAreas.Length; i++ ) {
			mSearchAreas[ i ].ResetSearchArea();
		}
	}

	// ClearClusters                   //
	// Clears clusters and noise lists //
	public void ClearClusters() {
		mClusters.Clear();
		mNoise.Clear();
	}

	// DBSCAN                                                      //
	// Density-based spatial clustering of applications with noise //
	// http://en.wikipedia.org/wiki/DBSCAN                         //
	public void DBSCAN( SearchNode[] nodesToBeSearched ) {
		// Clear lists
		mClusters.Clear();
		mNoise.Clear();

		// Zero length array safeguard
		if( nodesToBeSearched.Length > 0 ) {
			// DBSCAN Main Loop //
			for( int i = 0; i < nodesToBeSearched.Length; i++ ) {
				// For each point NOT visited
				if( !nodesToBeSearched[ i ].HasBeenVisited() ) { 
					// Flag search node as visited
					nodesToBeSearched[ i ].SetVisited( true );

					// Get list of neighbours for this search node
					List< SearchNode > neighbours = GetNeighbours( nodesToBeSearched[ i ], nodesToBeSearched );

					// If the neighbours lists count is under mMinimumPoints
					// It is NOT part of a cluster and therefore NOISE
					if( neighbours.Count < mMinimumPoints ) {
						nodesToBeSearched[ i ].mIsNoise = true;
						mNoise.Add( nodesToBeSearched[ i ] );
					// Else is has sufficent neighbours to be part of a cluster
					// Send this search node and neighbours to begin ExpandCluster 
					// Returns the full cluster which is added to mClusters list
					} else {
						mClusters.Add( ExpandCluster( nodesToBeSearched[ i ], neighbours, nodesToBeSearched ) );
					}
				}
			}

			// Rogue Nodes
			for( int i = 0; i < nodesToBeSearched.Length; i++ ) {
				if( ( !nodesToBeSearched[ i ].PartOfCluster() ) && ( !nodesToBeSearched[ i ].mIsNoise ) ) {
					//Debug.Log( "Rogue NOISE at - " + nodesToBeSearched[ i ].transform.position );
					nodesToBeSearched[ i ].mIsNoise = true;
					mNoise.Add( nodesToBeSearched[ i ] );
				}
			}

			// Number clusters
			for( int i = 0; i < mClusters.Count; i++ ) {
				mClusters[ i ].mClusterNumber = i;
				mClusters[ i ].SetClusterNumber();
				mClusters[ i ].CalculateCenter();
			}
		}
	}

	// Private Functions //
	
	// ExpandCluster                                                                                                     //
	// Requires and initial search node and a list of its neighbours, along with full list of search nodes for expansion //
	// Returns a Cluster Class object - which deals with its own variables                                               //
	private Cluster ExpandCluster( SearchNode initialPoint, List< SearchNode > neighbours, SearchNode[] nodesToBeSearched ) {
		// Create new cluster
		Cluster cluster = new Cluster();

		// Flag and add initial point to cluster
		initialPoint.SetAsPartOfCluster( true );
		cluster.AddSearchNode( initialPoint );

		// Loop through passed neighbour points
		for( int i = 0; i < neighbours.Count; i++ ) {
			// If it has NOT been visited
			if( !neighbours[ i ].HasBeenVisited() ) {
				// Now flag as visited
				neighbours[ i ].SetVisited( true );

				// Get neighbours of each point
				List< SearchNode > localNeighbours = GetNeighbours( neighbours[ i ], nodesToBeSearched );

				// If this point has enough neighbours
				if( localNeighbours.Count >= mMinimumPoints ) {
					for( int j = 0; j < localNeighbours.Count; j++ ) {
						// If this point isnt already a neighbour or has NOT been previously visited
						// Add it as a brand new neighbour
						//if( ( !localNeighbours[ j ].IsNeighbour() ) && ( !localNeighbours[ j ].HasBeenVisited() ) ) {
						if( !localNeighbours[ j ].HasBeenVisited() ) {
							// Add ONLY add localNeighbours that are 'joined' with nodes in neighbours
							if( CheckIfJoined( localNeighbours[ j ], neighbours ) ) {
								neighbours.Add( localNeighbours[ j ] );
							}
						}
					}

					// If this point is not part of a cluster - flag and add it!
					if( !neighbours[ i ].PartOfCluster() ) {
						neighbours[ i ].SetAsPartOfCluster( true );
						cluster.AddSearchNode( neighbours[ i ] );
					}
				}
			}
		}

		// Return final cluster
		return cluster;
	}

	// CheckIfJoined                                                                                        //
	// Returns TRUE if the passed point is within mMinimumDistance of one of the passed nodes of the DBSCAN //
	// Therefore it is a neighbour of one of those points                                                   //                      
	private bool CheckIfJoined( SearchNode point, List< SearchNode > nodesToBeSearched ) {
		/*
		// 0 length list safeguard
		if( nodesToBeSearched.Count == 0 ) {
			return false;
		}
		*/

		// Get position of point of interest
		Vector3 pointPosition = point.transform.position;

		// Loop through and check if it is within mMinimumDistance of any of nodes of this DBSCAN
		for( int i = 0; i < nodesToBeSearched.Count; i++ ) {
			Vector3 distanceVector = nodesToBeSearched[ i ].transform.position - pointPosition;
			if( distanceVector.magnitude < mMinimumDistance ) {
				return true;
			}
		}

		return false;
	}

	// GetNeighbours                                                                                        //
	// Returns a list of all the search nodes that are within mMinimumDistance of the specified search node //
	// Nodes are flagged as neigbours to avoid duplication                                                  //
	private List< SearchNode > GetNeighbours( SearchNode centerPoint, SearchNode[] nodesToBeSearched ) {
		// Create list to be returned
		List< SearchNode > neighbours = new List< SearchNode >();

		// Get center position Vector3
		Vector3 centerPosition = centerPoint.transform.position;

		// Loop through nodes to be searched
		for( int i = 0; i < nodesToBeSearched.Length; i++ ) {
			// Exclude self
			if( nodesToBeSearched[ i ].transform.position != centerPosition ) {
				// Get distance (B-A) between Vector3's
				Vector3 distanceVector = nodesToBeSearched[ i ].transform.position - centerPosition;

				// If under mMinimumDistance flag and add as neighbour
				if( distanceVector.magnitude < mMinimumDistance ) {
					nodesToBeSearched[ i ].SetAsNeighbour( true );
					neighbours.Add( nodesToBeSearched[ i ] );
				}
			}
		}

		// Return neighbours list
		return neighbours;
	}

	// Debug Functions //

	// Moar Debugging
	void OnDrawGizmos() {
		for( int i = 0; i < mClusters.Count; i++ ) {
			if( mClusters[ i ].GetCenter() != Vector3.zero ) {
				Gizmos.color = Color.blue;
				Gizmos.DrawSphere( new Vector3( mClusters[ i ].GetCenter().x, 3.0f, mClusters[ i ].GetCenter().z ), 0.3f ); 
			}
		}
	}
}

#region DBSCAN Pseudo Code
// DBSCAN - Martin Esterm, Hans-Peter Kriegel, Jorg Sander and Xiaowei Xu
// http://en.wikipedia.org/wiki/DBSCAN
// Density-based spatial clustering of applications with noise

// D           = list of passed searchnodes
// MinPts      = int of minimum required points to be considered a cluster (3+)
// eps         = distance between points of a cluster
// P           = point (searchnode) will need a visted flag (mVisited)
// NOISE       = list of searchnodes not part of a cluster
// NeighborPts = list of searchnodes within parameters P & eps
// C           = list of clusters (lists themselves?)

/*
DBSCAN(D, eps, MinPts)

	C = 0
	for each unvisited point P in dataset D
		mark P as visited
		NeighborPts = regionQuery(P, eps)
		if sizeof(NeighborPts) < MinPts
			mark P as NOISE
		else
			C = next cluster
			expandCluster(P, NeighborPts, C, eps, MinPts)
								
	expandCluster(P, NeighborPts, C, eps, MinPts)
		add P to cluster C
		for each point P' in NeighborPts 
     		 if P' is not visited
				mark P' as visited
        		NeighborPts' = regionQuery(P', eps)
        		if sizeof(NeighborPts') >= MinPts
					NeighborPts = NeighborPts joined with NeighborPts'
     		 if P' is not yet member of any cluster
				add P' to cluster C
          
	regionQuery(P, eps)
   		return all points within P's eps-neighborhood (including P)
*/
#endregion