#region Details 
// ------------------------------------------------------------------------- //
// Cluster Class                                                             //
// Author  - Chris McBain                                                    //
// Created - 170215                                                          //
// Purpose - Contains a list of SearchNodes - the 'cluster'                  //
//         - Ability to add nodes and calculate the clusters center          //
//         - ***CalculateCenter() must be called once cluster is complete*** //
//         - ***GetCenter() will return ( 0.0f, 0.0f, 0.0f ) by default***   //
// ------------------------------------------------------------------------- //
#endregion

#region Includes
// Unity Includes //
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
#endregion

// Cluster Class //
public class Cluster {
	// Private Variables //
	private List< SearchNode > mSearchNodes   = new List< SearchNode >();
	private Vector3            mCenter        = Vector3.zero;
	public int                 mClusterNumber = 0;

	// Public Functions //

	// Get Center                  //
	// Returns the mCenter Vector3 //
	public Vector3 GetCenter() {
		return mCenter;
	}

	// CalculateCenter                      //
	// Calculates the center of the cluster //
	public void CalculateCenter() {
		// Get number of search nodes
		int listCount = mSearchNodes.Count;

		// Default center
		Vector3 center = new Vector3( 0.0f, 0.0f, 0.0f );

		// If there are any search nodes
		if( listCount > 0 ) {
			// Loop through them
			for( int i = 0; i < listCount; i++ ) {
				// Add their (local) position vectors to center
				center += mSearchNodes[ i ].transform.position;
			}

			// Divide center by number of nodes to calculate center
			center /= listCount;
		}

		// Set mCenter
		mCenter = center;
	}

	// AddSearchNode //
	public void AddSearchNode( SearchNode toBeAdded ) {
		mSearchNodes.Add( toBeAdded );
	}

	// Debugging
	public void SetClusterNumber() {
		for( int i = 0; i < mSearchNodes.Count; i++ ) {
			mSearchNodes[ i ].mClusterNo = mClusterNumber;
		}
	}
}