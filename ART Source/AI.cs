#region Details
// -------------------------------------------------------------------------------------------------------- //
// Name:    AI Class                                                                                        //
// Author:  Chris McBain                                                                                    //
// Created: 130115                                                                                          //
// Purpose:                                                                                                 //
// - Representation of the A.I.                                                                             //
// - Has three modes of behaviour:                                                                          //
//   - Patrolling - using the GameControllers list of PatrolNodes and their radii to patrol the environment //
//   - Investigating - moving towards a detected audio contact and searching the area                       // 
//   - Chasing - chasing a visual contact, search the area last seen if contact it broken                   //
// - A.I.'s acceleration, speed and colour are dependant on it's state                                      //
// - Has two spot lights to represent the A.I.'s primary and perpiheral vision cones                        // 
// - Implements sortedlists to move towards closest patrol or search nodes first                            //
// - Implements DBSCAN algorithm to determine 'clusters' of unseen nodes during an area search              //
// -------------------------------------------------------------------------------------------------------- //
#endregion

#region Includes
// Unity Includes //
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
#endregion

// AL Class //
public class AI : MonoBehaviour {
	// Unity Editor Variables //
	[ SerializeField ] private Light mPrimaryLight    = null; // primary light reference
	[ SerializeField ] private Light mPerpiheralLight = null; // perpiheral light reference
	
	// A.I. Vision Variables //
	[ SerializeField ] private float mPrimaryVisionRange      = 7.5f;  // range in unity units
	[ SerializeField ] private float mPrimaryVisionHalfArc    = 24.0f; // total arc 48 degrees
	[ SerializeField ] private float mPeripheralVisionRange   = 3.25f; // range in unity units
	[ SerializeField ] private float mPeripheralVisionHalfArc = 70.0f; // total arc 140 degrees
	[ SerializeField ] private float mDBSCANRange             = 20.0f; // radius of the DBSCAN
	[ SerializeField ] private float mAcceleration            =  8.0f; // A.I.'s acceleration
	[ SerializeField ] private float mSpeed                   =  4.0f; // A.I.'s speed

	// A.I. Spotlight Colours //
	// Their true values have been set within the Unity Editor
	[ SerializeField ] private Color mPrimaryBlue      = Color.blue;
	[ SerializeField ] private Color mPrimaryYellow    = Color.yellow;
	[ SerializeField ] private Color mPrimaryRed       = Color.red;
	[ SerializeField ] private Color mPerhiperalBlue   = Color.blue;
	[ SerializeField ] private Color mPerhiperalYellow = Color.yellow;
	[ SerializeField ] private Color mPerhiperalRed    = Color.red;

	// A.I. Speed Scalars //
	[ SerializeField ] private float mInvestigatingScalar = 0.75f; // move at 75% of speed when investiagting
	[ SerializeField ] private float mPatrollingScalar    = 0.5f;  // move at 50% of speed when patrolling

	// Const Variables //
	private const float SEARCH_TARGET_HEIGHT = 7.0f;
	private const float SEARCH_TARGET_RADIUS = 0.75f;

	// Private Variables //
	private NavMeshAgent mNavMeshAgent       = null;         // reference to attached NavMeshAgent component
	private Vector3      mSearchTargetVector = Vector3.zero; // target Vector3 position
	private Vector3      mAreaTargetVector   = Vector3.zero; // area Vector3 position
	private Vector3      mForwardVector      = Vector3.zero; // body forward vector

	// Sorted List Variables //
	private SortedList< float, Vector4 > mPatrolPoints    = new SortedList< float, Vector4 >(); // sorted list of nearest patrol points
	private IList< Vector4 >             mClosestPoints   = new List< Vector4 >();              // list of sorted patrol positions (x/y/z) and their radiaa (w)
	private SortedList< float, Cluster > mSearchClusters  = new SortedList< float, Cluster >(); // sorted list of nearest clusters
	private IList< Cluster >             mSearchPoints    = new List< Cluster >();              // sorted list of cluster positions
	private int                          mSearchNodeIndex = 0;                                  // index for mSearchpoints

	private ClusteringAlgorithm mSearcher = null; // reference for DBSCAN algorithm

	private int   mPrefabMeshColliderMask = 1 << 8; // layermask for layer 8 only
	
	// AI State Variables //
	public  enum AIState { PATROLLING, INVESTIGATING, CHASING }; // main AI state
	private      AIState mCurrentState;                          // current state

	// Unity Functions //

	// Start                               //
	// Get attached NavMeshAgent component //
	void Start() {
		// Getting references to attached components
		mNavMeshAgent = GetComponent< NavMeshAgent >();
		mSearcher     = GetComponent< ClusteringAlgorithm >();

		// Initialise AI state variables
		mCurrentState  = AIState.PATROLLING;
	}
	
	// Update                                //
	// Move object to position of move click //
	void Update() {
		// Perform AI behaviour
		AILogic();

		// Update State Variables
		CurrentStateSwitch();

		// Update forward vectors
		mForwardVector = transform.forward;

		// Mark nodes as seen
		DetectNodesInLOS();
	}

	// Public Functions //

	// GetAIState               //
	// Returns current AI state //
	public AIState GetAIState() {
		return mCurrentState;
	}

	// SetContact                                                                       //
	// Stops A.I.'s current activity, State is reset to either Investigating or Chasing //
	// Commences a single move to target action, followed by a search of that area      //
	public void SetContact( Vector3 position, float searchRadius, AIState newAIState ) {
		// Duplicate safeguard
		if( mSearchTargetVector != position ) {
			// Stop A.I.
			mNavMeshAgent.Stop();
			
			// Reset
			ResetAIVariables();
			
			// Set position as new search target
			mSearchTargetVector = position;
			
			// Designate the passed search area radius
			DesignateSearchArea( mSearchTargetVector, searchRadius );
			
			// Set state to investigating
			mCurrentState = newAIState;
			
			// Start A.I.
			mNavMeshAgent.SetDestination( mSearchTargetVector );
			mNavMeshAgent.Resume();
		}
	}

	// Reset                                                               //
	// Returns A.I. to its starting position and resets the A.I. variables //
	public void Reset() {
		mCurrentState = AIState.PATROLLING;
		mSearchTargetVector = mAreaTargetVector = Vector3.zero;
		mSearchNodeIndex = 0;
		mNavMeshAgent.Stop();
	}

	// Private Functions //

	// AILogic             //
	// Main A.I. Logic FSM //
	private void AILogic() {
		// PATROLLING //
		if( mCurrentState == AIState.PATROLLING ) {
			// Initial patrolling state
			if( ( mSearchTargetVector == Vector3.zero ) && ( mAreaTargetVector == Vector3.zero ) ) {
				PatrolToNextNode();
			}
			
			// Patrolling towards patrol node 
			if( ( mSearchTargetVector != Vector3.zero ) && ( mAreaTargetVector == Vector3.zero ) ) {
				// Reaching a patrol node - zero search target vector
				float distance = Vector3.Distance( transform.position, mSearchTargetVector );
				
				// Initialise area search
				if( distance < 1.0f ) {
					// Zero search target vector
					mSearchTargetVector = Vector3.zero;
					
					// Find clusters
					PerformDBSCAN();
					
					// Begin area search if clusters are found
					if( mSearchPoints.Count > 0 ) {
						// Set target vector as first point (closest cluster)
						mAreaTargetVector = mSearchPoints[ mSearchNodeIndex ].GetCenter();
						
						// Start A.I.
						mNavMeshAgent.SetDestination( mAreaTargetVector );
						mNavMeshAgent.Resume();
					// Otherwise prepare to move to next closest patrol node
					} else {
						// Clear clusters from last DBSCAN
						mSearcher.ClearClusters();
						
						// Patrol to next node
						PatrolToNextNode();
					}
				}
			}
			
			// Searching area 
			if( mAreaTargetVector != Vector3.zero ) {
				// Reaching a cluster center 
				float distance = Vector3.Distance( transform.position, mSearchPoints[ mSearchNodeIndex ].GetCenter() );
				if( distance < 1.0f ) {
					// Move on to next cluster
					mSearchNodeIndex++;
					
					// If there are still clusters to be searched
					if( mSearchNodeIndex < mSearchPoints.Count ) {
						// Set target vector as first point (closest cluster)
						mAreaTargetVector = mSearchPoints[ mSearchNodeIndex ].GetCenter();
						
						// StartA.I.
						mNavMeshAgent.SetDestination( mAreaTargetVector );
						mNavMeshAgent.Resume();
					// End of area search - tidyup variables and patrol towards next patrol node
					} else {
						// Zero index
						mSearchNodeIndex = 0;
						
						// Zero area target vector
						mAreaTargetVector = Vector3.zero;
						
						// Clear clusters from last DBSCAN
						mSearcher.ClearClusters();
						
						// Patrol to next patrol node
						PatrolToNextNode();
					}
				}
			}
		}
		
		// INVESTIGATING OR CHASING //
		if( ( mCurrentState == AIState.INVESTIGATING ) || ( mCurrentState == AIState.CHASING ) ) {
			// Reaching the target position
			if( mAreaTargetVector == Vector3.zero ) {
				// Reaching the source of the sound 
				float distance = Vector3.Distance( transform.position, mSearchTargetVector );
				
				// Initialise area search
				if( distance < 1.0f ) {
					// Zero search target vector
					mSearchTargetVector = Vector3.zero;
					
					// Find clusters
					PerformDBSCAN();
					
					// Begin area search if clusters are found
					if( mSearchPoints.Count > 0 ) {
						// Set target vector as first point (closest cluster)
						mAreaTargetVector = mSearchPoints[ mSearchNodeIndex ].GetCenter();
						
						// Start A.I.
						mNavMeshAgent.SetDestination( mAreaTargetVector );
						mNavMeshAgent.Resume();
				// Otherwise return to patrolling
					} else {
						ResetAIVariables();
						mCurrentState = AIState.PATROLLING;
					}
				}
			}
			
			// Searching area 
			if( mAreaTargetVector != Vector3.zero ) {
				// Reaching a cluster center 
				float distance = Vector3.Distance( transform.position, mSearchPoints[ mSearchNodeIndex ].GetCenter() );
				if( distance < 1.0f ) {
					// Move on to next cluster
					mSearchNodeIndex++;
					
					// If there are still clusters to be searched
					if( mSearchNodeIndex < mSearchPoints.Count ) {
						// Set target vector as first point (closest cluster)
						mAreaTargetVector = mSearchPoints[ mSearchNodeIndex ].GetCenter();
						
						// StartA.I.
						mNavMeshAgent.SetDestination( mAreaTargetVector );
						mNavMeshAgent.Resume();
					// End of area search - reset search variables, return to patrolling
					} else {
						ResetAIVariables();
						mCurrentState = AIState.PATROLLING;
					}
				}
			}
		}
	}

	// ResetAIVariables                                  //
	// Resets all search based variables for a new input //
	private void ResetAIVariables() {
		// Zero target vectors
		mSearchTargetVector = Vector3.zero;
		mAreaTargetVector   = Vector3.zero;

		// Tidy up searcher (all search nodes and cluster/noise lists) 
		mSearcher.ClearClusters();
		mSearcher.ResetSearchAreas();

		// Recreate A.I. lists and index
		mPatrolPoints    = new SortedList< float, Vector4 >();
		mClosestPoints   = new List< Vector4 >();             
		mSearchClusters  = new SortedList< float, Cluster >();
		mSearchPoints    = new List< Cluster >();             
    	mSearchNodeIndex = 0;
	}

	// PatrolToNextnode                                                           //
	// Abusing a vector4 to hold an extra float of data - the patrol nodes radius //
	private void PatrolToNextNode() {
		// Calculate closest patrol node list
		GetClosestPatrolNode_Path();
		
		// Get list of values from sorted list
		mClosestPoints = mPatrolPoints.Values;
		
		// Set target as closest patrol node (first in the sorted list)
		mSearchTargetVector = new Vector3( mClosestPoints[ 0 ].x, mClosestPoints[ 0 ].y, mClosestPoints[ 0 ].z );
		
		// Mark target areas search nodes within that patrol nodes radius
		DesignateSearchArea( mSearchTargetVector, mClosestPoints[ 0 ].w );
		
		// Start A.I.
		mNavMeshAgent.SetDestination( mSearchTargetVector );
		mNavMeshAgent.Resume();
	}
	
	// PerformDBSCAN                                                                                            //
	// Firstly creates an array of all the SearchNode objects within the desired radius of the impending DBSCAN //
	// Gets all the possible SearchArea's within the radius, then checks each for any SearchNodes within        //
	// Then sends the array to the DBSCAN object                                                                //
	public void PerformDBSCAN() {
		// Clear search point list
		mSearchClusters.Clear();

		// Get an array of all the SearchArea colliders in range 
		Collider[] colliders = Physics.OverlapSphere( transform.position, mDBSCANRange, mPrefabMeshColliderMask );

		// List of SearchNodes to be processed by DBSCAN
		List< SearchNode > searchAreaList = new List< SearchNode >();
		
		// If there are any
		if( colliders.Length > 0 ) {
			for( int i = 0; i < colliders.Length; i++ ) {
				// Add all the SearchNodes within each SearchArea that are within the radius
				searchAreaList.AddRange( colliders[ i ].GetComponentInParent< SearchArea >().GetSearchArea( transform.position, mDBSCANRange ) ); 
			}
		}
		
		// Convert the list to an array
		SearchNode[] searchArea = searchAreaList.ToArray();

		// Perform DBSCAN
		mSearcher.DBSCAN( searchArea );

		// Populate array of points to be searched
		Cluster[] clusters = mSearcher.GetClusters();

		// Sort by distance into sorted list
		for( int i = 0; i < clusters.Length; i++ ) {
			float distance = Vector3.Distance( transform.position, clusters[ i ].GetCenter() );
			mSearchClusters.Add( distance, clusters[ i ] );
		}

		// Set points as values of sorted list
		mSearchPoints = mSearchClusters.Values;

		// Reset search areas before a new DBSCAN
		mSearcher.ResetSearchAreas();
	}

	// Private Functions //

	// GetClosestPatrolNode_Linear                                                                   //
	// Linear implementation to sort un-vistied patrol nodes by their current distance from the A.I. //
	private void GetClosestPatrolNode_Linear() {
		// Get list of patrol nodes
		PatrolNode[] patrolNodes = GameController.Instance.GetPatrolNodes();

		// Sort by distance into list 
		for( int i = 0; i < patrolNodes.Length; i++ ) {
			// Only calculate for unvisited patrol nodes
			if( !patrolNodes[ i ].HasBeenVisited() ) {
				// Get linear distance to patrol node
				float distance = Vector3.Distance( transform.position, patrolNodes[ i ].transform.position );

				// Add to sorted list - distance to node as the 'Key' and the nodes position vector as the 'value'
				// Abusing a vector4 to hold another piece of float data - the patrol nodes radius
				mPatrolPoints.Add( distance, new Vector4( patrolNodes[ i ].transform.position.x,
				                                          patrolNodes[ i ].transform.position.y,
				                                          patrolNodes[ i ].transform.position.z,
				                                          patrolNodes[ i ].GetSearchRadius() ) );
			}
		}

		// If list is empty - all patrol nodes have been visited - reset and start again
		if( mPatrolPoints.Count == 0 ) {
			GameController.Instance.ResetPatrolNodes();
			GetClosestPatrolNode_Linear();
		}
	}

	// GetClosestPatrolNode_Path                                                                                         //
	// Unity NavMeshAgent path based implementation to sort un-visted patrol nodes by their paths distance from the A.I. //
	void GetClosestPatrolNode_Path() {
		// Clear patrol list
		mPatrolPoints.Clear();

		// Get list of patrol nodes
		PatrolNode[] patrolNodes = GameController.Instance.GetPatrolNodes();

		// Sort by distance into list 
		for( int i = 0; i < patrolNodes.Length; i++ ) {
			// Only calculate for unvisited patrol nodes
			if( !patrolNodes[ i ].HasBeenVisited() ) {
				// Local path
				NavMeshPath path = new NavMeshPath();

				// Calculate path to current patrol node
				mNavMeshAgent.CalculatePath( patrolNodes[ i ].transform.position, path );

				// Get path distance to patrol node
				float distance = GetPathLength( path );
				
				// Add to sorted list - distance to node as the 'Key' and the nodes position vector as the 'value'
				// Abusing a vector4 to hold another piece of float data - the patrol nodes radius
				mPatrolPoints.Add( distance, new Vector4( patrolNodes[ i ].transform.position.x,
				                                          patrolNodes[ i ].transform.position.y,
				                                          patrolNodes[ i ].transform.position.z,
				                                          patrolNodes[ i ].GetSearchRadius() ) );
			}
		}

		// If list is empty - all patrol nodes have been visited - reset and start again
		if( mPatrolPoints.Count == 0 ) {
			GameController.Instance.ResetPatrolNodes();
			GetClosestPatrolNode_Path();
		}
	}

	// GetPathLength                                                                                              //          
	// Calculate the length of a NavMeshPath - Unity NavMeshAgent.remainingDistance does not function as expected //
	// A character will typically move a slightly longer distance than this as a result of its steering behaviour //
	// http://docs.unity3d.com/ScriptReference/NavMeshPath-corners.html                                           //
	float GetPathLength( NavMeshPath path ) {
		// Previous corner and length locals
		Vector3 previousCorner = path.corners[ 0 ];
		float lengthSoFar = 0.0f;

		// The length is implicitly zero if there aren't at least two corners in the path
		if( path.corners.Length < 2 ) {
			return 0.0f;
		}

		// Calculate the total distance by adding up the lengths of the straight lines between corners
		for( int i = 1; i < path.corners.Length; i++ ) {
			Vector3 currentCorner = path.corners[ i ];
			lengthSoFar += Vector3.Distance( previousCorner, currentCorner );
			previousCorner = currentCorner;
		}
		
		return lengthSoFar;
	}

	// DetectNodesInLOS                //
	// Higlighting 'seen' search nodes //
	private void DetectNodesInLOS() {
		// Get an array of all the colliders that are within the primary vision range - using a mask to only return correct objects
		Collider[] colliders = Physics.OverlapSphere( transform.position, mPrimaryVisionRange, mPrefabMeshColliderMask );
		// If there are any
		if( colliders.Length > 0 ) {
			// Due to the layer mask any return colliders are children of a SearchArea object
			for( int i = 0; i < colliders.Length; i++ ) {
				colliders[ i ].GetComponentInParent< SearchArea >().CheckForArcHits( transform.position, mForwardVector, mPeripheralVisionRange, mPeripheralVisionHalfArc );
				colliders[ i ].GetComponentInParent< SearchArea >().CheckForArcHits( transform.position, mForwardVector, mPrimaryVisionRange, mPrimaryVisionHalfArc );
			}
		}
	}
	
	// DesignateSearchArea                                                  //
	// Designates all the SearchNodes within the passed radius and position //
	private void DesignateSearchArea( Vector3 targetPosition, float targetRadius ) {
		// Get all colliders in target area 
		Collider[] colliders = Physics.OverlapSphere( targetPosition, targetRadius, mPrefabMeshColliderMask );
		// If there are any
		if( colliders.Length > 0 ) {
			for( int i = 0; i < colliders.Length; i++ ) {
				colliders[ i ].GetComponentInParent< SearchArea >().CheckForHits( targetPosition, targetRadius );
			}
		}
	}

	// CurrentStateSwitch                                                                            //
	// Updates A.I. spotlight colours and accelration and speed variables dependant on current state //
	private void CurrentStateSwitch() {
		switch( mCurrentState ) {
			case AIState.PATROLLING: {
				mPrimaryLight.color        = mPrimaryBlue;
				mPerpiheralLight.color     = mPerhiperalBlue;
				mNavMeshAgent.acceleration = mAcceleration * mPatrollingScalar;
				mNavMeshAgent.speed        = mSpeed * mPatrollingScalar;
				break;
			}
			case AIState.INVESTIGATING: {
				mPrimaryLight.color        = mPrimaryYellow;
				mPerpiheralLight.color     = mPerhiperalYellow;
				mNavMeshAgent.acceleration = mAcceleration * mInvestigatingScalar;
				mNavMeshAgent.speed        = mSpeed * mInvestigatingScalar;
				break;
			}
			case AIState.CHASING: {
				mPrimaryLight.color        = mPrimaryRed;
				mPerpiheralLight.color     = mPerhiperalRed;
				mNavMeshAgent.acceleration = mAcceleration;
				mNavMeshAgent.speed        = mSpeed;
				break;
			}
		}
	}

	// Debug Functions //

	// GetMousePositionInWorld                                             //
	// Converts mouse position into world space coordinate at 'zero' depth //	
	private Vector3 GetMousePositionInWorld() {
		// Get ray from screen point of mouse position into world space
		Ray ray = Camera.main.ScreenPointToRay( Input.mousePosition );

		// TargetPosition is the cameras distance from the origin along the ray
		Vector3 targetPosition = ray.origin + ( ray.direction * Camera.main.transform.position.y );

		// Return it
		return targetPosition;
	}
	
	// OnDrawGizmos //
	void OnDrawGizmos() {
		if( mSearchTargetVector != Vector3.zero ) {
			Gizmos.color = Color.magenta;
			Gizmos.DrawSphere( new Vector3( mSearchTargetVector.x, SEARCH_TARGET_HEIGHT, mSearchTargetVector.z ), SEARCH_TARGET_RADIUS ); 
		}
	}
}
