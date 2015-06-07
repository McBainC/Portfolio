#region Details
// ------------------------------------------------//
// Name:    Flare Class                            //
// Author:  Chris McBain                           //
// Created:                                        //
// Purpose:                                        //
// - Represents a flare to be thrown by the player //
// - Destroys itself after the set time            //
// ----------------------------------------------- //
#endregion

#region Includes
// Unity Includes //
using UnityEngine;
using System.Collections;
#endregion

// Flare Class //
public class Flare : MonoBehaviour {
	// Unity Editor Variables //
	[ SerializeField ] private float mDuration = 15.0f;

	// Private Variables //
	private float mTime = 0.0f;

	// Unity Functions //

	// Update                          //	
	// Update is called once per frame //
	void Update () {
		// Increase time
		mTime += Time.deltaTime;

		// Destroy condition
		if( mTime > mDuration ) {
			Destroy( this.gameObject );
		}
	}
}
