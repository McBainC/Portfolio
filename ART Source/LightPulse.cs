#region Details 
// ----------------------------------------------- //
// Name: LightPulse Class                          //
// Author: Chris McBain                            //
// Purpose:                                        //
// - Implements a pulsing light source             //
// - Must be attached to a Light Object            //
// - Has functionality to alter and stagger pulses //
// ----------------------------------------------- //
#endregion

#region Includes
// Unity Includes
using UnityEngine;
using System.Collections;
#endregion

// LightPulse Class //
public class LightPulse : MonoBehaviour {
	// Unity Editor Variables //
	[ SerializeField ] private float mPulseMinimum = 1.0f;
	[ SerializeField ] private float mPulseRange   = 4.0f;
	[ SerializeField ] private float mPulseSpeed   = 3.0f;

	[ SerializeField ] [ Range( 0.0f, 1.0f ) ] private float mRandomStart = 0.0f;

	// Private Variables //
	private       Light mLight   = null;
	private const float MAX_TIME = 10000;
	private       float mTime    = 0.0f;

	// Unity Functions //

	// Start                                             //
	// Gets light component and adds random start if set //
	void Start () {
		mLight = GetComponent< Light >();	
		mTime += mRandomStart;
	}

	// Update                 //
	// Pulses light intensity //
	void Update() {
		// Increase mTime
		mTime += Time.deltaTime;

		// Pulse lights intensity
		mLight.intensity = mPulseMinimum + Mathf.PingPong( ( mTime * mPulseSpeed ), mPulseRange - mPulseMinimum );

		// Time reset
		if( mTime > MAX_TIME ) {
			mTime = 0.0f;
		}
	}
}
