#region Details
// -------------------------------------------------------- //
// Name: PatrolNode Class                                   //
// Author: Chris McBain                                     //
// Created:                                                 //
// Purpose:                                                 //
// - Object to represent one of a collection of PatrolNodes //
// - To allow A.I. to patrol the whole environment          //
// - User editor defined radius with public access          //
// - Can be flagged as visited or not                       //
// -------------------------------------------------------- //
#endregion

#region Includes
// Unity Includes //
using UnityEngine;
using System.Collections;
#endregion

// PatrolNode Class //
public class PatrolNode : MonoBehaviour {
	// Unity Editor Variables //
	[ SerializeField ] private float mSearchRadius = 0.0f;

	// Const Variables //
	private const float PATROL_NODE_RADIUS  = 0.5f;
	private const float PATROL_GIZMO_HEIGHT = 5.0f;

	// Private Variables //
	private bool  mVisited = false;

	// Public Functions //

	// GetSearchRadius          //
	// Getter for mSearchRadius //
	public float GetSearchRadius() {
		return mSearchRadius;
	}

	// HasBeenVisited           //
	// Getter for mVisited flag //
	public bool HasBeenVisited() {
		return mVisited;
	}

	// SetVisited         //
	// Sets mVistied flag //
	public void SetVisited( bool hasBeenVisited ) {
		mVisited = hasBeenVisited;
	}

	// Debug Functions //

	// OnDrawGizmos                                    //
	// Draws cyan spheres to represent the patrol node //
	void OnDrawGizmos() {
		Gizmos.color = Color.cyan;
		Gizmos.DrawSphere( new Vector3( transform.position.x, PATROL_GIZMO_HEIGHT, transform.position.z ), PATROL_NODE_RADIUS ); 
	}
}
