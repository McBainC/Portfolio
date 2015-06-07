#region Details 
// ------------------------------------------------------------------------------- //
// SearchArea Class                                                                //
// Author  - Chris McBain                                                          //
// Created - 220115                                                                //
// Purpose - Object which contains a list of 'SearchNodes' to be searched          //
//         - Has its own collider mesh to be checked before searching nodes within //
// ------------------------------------------------------------------------------- //
#endregion

#region Includes
// Unity Includes //
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
#endregion

// SearchArea //
public class SearchArea : MonoBehaviour {
	// Private Variables //
	[ SerializeField ] private SearchNode[] mSearchNodes;

	private const float SEARCH_NODE_RADIUS = 0.2f;
	private const float NAV_MESH_DISTANCE  = 0.1f;

	// Unity Functions //

	// Start                                                         //
	// Gets all the seach node components from the attached children //
	void Start () {
		// Initialise search node list
		mSearchNodes = GetComponentsInChildren< SearchNode >();
	}

	// Update                              //
	// Blocks nodes not within the navmesh //
	void Update () {
		NavMeshHit hit;
		for( int i = 0; i < mSearchNodes.Length; i++ ) {
			if( !NavMesh.SamplePosition( new Vector3( mSearchNodes[ i ].transform.position.x,
			                                          NAV_MESH_DISTANCE,
			                                          mSearchNodes[ i ].transform.position.z ), out hit, NAV_MESH_DISTANCE, -1 ) ) {
				mSearchNodes[ i ].SetSearchState( SearchNode.Search_State.BLOCKED );
			}
		}
	}

	// FixedUpdate                 //
	// Returns nodes to void state //
	void FixedUpdate() {
		for( int i = 0; i < mSearchNodes.Length; i++ ) {
			if( mSearchNodes[ i ].GetSearchState() == SearchNode.Search_State.VISIBLE ) {
				mSearchNodes[ i ].SetSearchState( SearchNode.Search_State.VOID );
			}
		}
	}

	// Public Functions //

	// ResetSearchArea                       //
	// Resets this search areas search nodes //
	public void ResetSearchArea() {
		for( int i = 0; i < mSearchNodes.Length; i++ ) {
			if( mSearchNodes[ i ].GetSearchState() != SearchNode.Search_State.BLOCKED ) {
				mSearchNodes[ i ].SetSearchState( SearchNode.Search_State.VOID );
				mSearchNodes[ i ].Reset();
			}
		}
	}

	// GetSearchArea                                                                          //
	// Returns an array of search nodes that are within the passed radius in the search state //
	public SearchNode[] GetSearchArea( Vector3 center, float radius ) {
		List< SearchNode > searchAreaList = new List< SearchNode > ();

		for( int i = 0; i < mSearchNodes.Length; i++ ) {
			Vector3 distanceVector = mSearchNodes[ i ].transform.position - center;
			if( ( distanceVector.magnitude < radius ) && ( mSearchNodes[ i ].GetSearchState() == SearchNode.Search_State.SEARCH ) ) {
				searchAreaList.Add( mSearchNodes[ i ] );
			}
		}

		SearchNode[] searchArea = searchAreaList.ToArray();
		return searchArea;
	}

	// CheckForHits //
	// Sets all nodes within the passed radius of the the position as to be searched //
	public void CheckForHits( Vector3 center, float radius ) {
		for( int i = 0; i < mSearchNodes.Length; i++ ) {
			Vector3 distanceVector = mSearchNodes[ i ].transform.position - center;
			if( distanceVector.magnitude < radius ) {
				mSearchNodes[ i ].SetSearchState( SearchNode.Search_State.SEARCH );
			}
		}
	}

	// CheckForArcHits                                                    //
	// Sets al nodes with the passed radius, position and arc as visiable //
	public void CheckForArcHits( Vector3 center, Vector3 forward, float radius, float angle ) {
		for( int i = 0; i < mSearchNodes.Length; i++ ) {
			Vector3 distanceVector = mSearchNodes[ i ].transform.position - center;
			if( distanceVector.magnitude < radius ) {
				// Dot product of normalised direction vectors
				// Returns 1 for same direction to 0 for 90 degrees
				Vector3 sight = forward.normalized;
				Vector3 direction = distanceVector.normalized;
				float cosine = Vector3.Dot( sight, direction );
				float degrees = Mathf.Acos( cosine ) * Mathf.Rad2Deg;

				// If within arc
				if( degrees <= angle ) {
					// If a linecast hits no colliders
					if( !Physics.Linecast( new Vector3( center.x, 0.25f, center.z ), new Vector3( mSearchNodes[ i ].transform.position.x, 0.25f, mSearchNodes[ i ].transform.position.z ) ) ) {
						mSearchNodes[ i ].SetSearchState( SearchNode.Search_State.VISIBLE );
					}
				}
			}
		}
	}

	// Debug Functions //

	// OnDrawGizmos                                                            //
	// Draws coloured spheres to represent search nodes state within this area //
	void OnDrawGizmos() {
		// Loop through search nodes in this area
		if( mSearchNodes.Length/*Count*/ != 0 ) {
			for( int i = 0; i < mSearchNodes.Length; i++ ) {
				// Set gizmo colour based on the current search nodes state
				switch( mSearchNodes[ i ].GetSearchState() ) {
					case SearchNode.Search_State.BLOCKED: {
						Gizmos.color = Color.black;
						break;
					}
					case SearchNode.Search_State.VOID: {
						Gizmos.color = Color.white;
						break;
					}
					case SearchNode.Search_State.VISIBLE: {
						Gizmos.color = Color.green;
						break;
					}
					case SearchNode.Search_State.SEARCH: {
						Gizmos.color = Color.gray;
						break;
					}
				}

				if( mSearchNodes[ i ].HasBeenVisited() ) {
					Gizmos.color = Color.blue;
				}

				if( mSearchNodes[ i ].IsNeighbour() ) {
					Gizmos.color = Color.magenta;
				}

				if( mSearchNodes[ i ].PartOfCluster() ) {
					Gizmos.color = new Vector4( ( 1.0f / ( mSearchNodes[ i ].mClusterNo + 1.0f ) ), 0.0f, 0.0f, 1.0f );
				}

				if( mSearchNodes[ i ].mIsNoise ) {
					Gizmos.color = Color.yellow;
				}

				// Draw it
				Gizmos.DrawSphere( mSearchNodes[ i ].transform.position, SEARCH_NODE_RADIUS ); 
			}
		}
	}
}