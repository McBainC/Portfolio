#region Details
// ---------------------------------------------------------------------- //
// Name: SearchNode Class                                                 //
// Author: Chris McBain                                                   //
// Created: 220115                                                        //
// Purpose:                                                               //
// - Object to serve as a node to be discovered as part of an area search //
// ---------------------------------------------------------------------- //
#endregion

#region Includes
// Unity Includes //
using UnityEngine;
using System.Collections;
#endregion

// SearchNode //
public class SearchNode : MonoBehaviour {
	// Search State Variables
	public enum Search_State { BLOCKED, VOID, VISIBLE, SEARCH }; // node search state
	private     Search_State mSearchState;                       // current search state

	// Clustering Variables
	private bool mVisited       = false; // flag for being visited by DBSCAN
	private bool mPartOfCluster = false; // flag for belonging to a cluster
	private bool mIsNeighbour   = false; // dont honestly remember...
	public  bool mIsNoise       = false; // flag for being 'noise'
	public  int  mClusterNo     = 0;     // cluster index

	// Unity Functions //

	// Start                       //
	// Use this for initialization //
	void Start () {
		// Initialise search state as void
		mSearchState = Search_State.VOID;
	}

	// GetSearchState               //
	// Returns current search state //
	public Search_State GetSearchState() {
		return mSearchState;
	}

	// SetSearchState               //
	// Sets this nodes search state //
	public void SetSearchState( Search_State newState ) {
		mSearchState = newState;
	}      

	// HasBeenVisited        //
	// Returns mVisited bool //
	public bool HasBeenVisited() {
		return mVisited;
	}

	// SetVisited         //
	// Sets mVisited bool //
	public void SetVisited( bool hasBeenVisited ) {
		mVisited = hasBeenVisited;
	}

	// PartOfCluster          //
	// Returns mPartOfCluster //
	public bool PartOfCluster() {
		return mPartOfCluster;
	}

	// SetAsPartOfCluster       //
	// Sets mPartOfCluster bool //
	public void SetAsPartOfCluster( bool partOfCluster ) {
		mPartOfCluster = partOfCluster;
	}

	// IsNeighbour          //
	// Returns mIsNeighbour //
	public bool IsNeighbour() {
		return mIsNeighbour;
	}

	// SetAsNeighbour         //
	// Sets mIsNeighbour bool //
	public void SetAsNeighbour( bool isNeighbour ) {
		mIsNeighbour = isNeighbour;
	}

	// Reset                            //
	// Resets the search node variables //
	public void Reset() {
		mVisited       = false;
		mPartOfCluster = false;
		mIsNeighbour   = false;
		mIsNoise       = false;
		mClusterNo     = 0;
	}
}
