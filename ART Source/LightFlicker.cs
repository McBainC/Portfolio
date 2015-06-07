#region Details 
// --------------------------------------------------- //
// Name: LightFlicker Class                            //
// Author: Chris McBain                                //
// Purpose:                                            //
// - Implements a flickering light source              //
// - Must be attached to a Light Object                //
// - Has functionality to alter and stagger flickering //
// --------------------------------------------------- //
#endregion

#region Includes
// Unity Includes
using UnityEngine;
using System.Collections;
#endregion

// LightFlicker Class //
public class LightFlicker : MonoBehaviour {
	// Unity Editor Variables //
	[ SerializeField ] private bool  mFlickering = true;
	[ SerializeField ] private float mMinimum    = 0.1f;
	[ SerializeField ] private float mMaximum    = 1.0f;

	[ SerializeField ] [ Range( 0.0f, 0.1f ) ] private float mRandomStart = 0.0f;

	// Private Variables //
	private       Light mLight        = null;
	private const float MAX_TIME      = 10000;
	private       float mTime         = 0.0f;
	private       float mFlickerTimer = 0.0f;

	// Unity Functions //

	// Start                                    //
	// Get light component and add random start //
	void Start () {
		mLight = GetComponent< Light >();	
		mTime += mRandomStart;
		mFlickerTimer = Random.Range( mMinimum, mMaximum );
	}

	// Update                                                      //
	// Causes flickering effect by turning light source on and off //
	void Update () {
		// Update time
		mTime += Time.deltaTime;

		if( mFlickering ) {
			if( mTime > mFlickerTimer ) {
				mLight.enabled = false;
				mFlickerTimer = Random.Range( mMinimum, mMaximum );
				mTime = 0.0f;
			} else {
				mLight.enabled = true;
			}
		}

		/*if( mTime > MAX_TIME ) {
			mTime = 0.0f;
		}*/
	}
}
