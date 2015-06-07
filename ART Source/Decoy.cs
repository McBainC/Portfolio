#region Details
// ------------------------------------------------------------------------------- //
// Name:    Decoy Class                                                            //
// Author:  Chris McBain                                                           //
// Created:                                                                        //
// Purpose:                                                                        //
// - Representation of both a visual and audio contact within the test environment //
// - Emitts a audio contact and can be seen as a visual contact while active       //
// - Activated by designated 360 controller button, has text mesh indicator        //
// - Has public availability flag, only one such object to be active at any time   //
// ------------------------------------------------------------------------------- //
#endregion

#region Includes
// Unity Includes //
using UnityEngine;
using System.Collections;
#endregion

// Decoy Class //
public class Decoy : MonoBehaviour {
	// Object References //
	[ SerializeField ] private Light      mLight1     = null; // light1 reference 
	[ SerializeField ] private Light      mLight2     = null; // light2 reference
	[ SerializeField ] private Light      mLight3     = null; // light3 reference
	[ SerializeField ] private GameObject mTextObject = null; // 3D text mesh object reference

	// Unity Editor Variables //
	[ SerializeField ] private float  mSpeed        = 100.0f; // speed of lights rotation
	[ SerializeField ] private float  mStartOffset  = 0.0f;   // initial rotation angle
	[ SerializeField ] private float  mActiveTime   = 0.0f;   // time spend active
	[ SerializeField ] private float  mDuration     = 20.0f;  // time till deactivation
	[ SerializeField ] private string mInputString  = null;   // string for controller input
	
	// Private Varibles //
	private float    mXRotation    = 30.0f;  // set slightly downwards
	private float    mYRotation    = 0.0f;   // rotation axis
	private float    mZRotation    = 0.0f;   // not rotation required
	private float    mLight2Offset = 180.0f; // light2 offset by 180 degrees
	private bool     mActive       = false;  // active flag
	private bool     mAvailable    = true;   // available to be activated
	private Renderer mTextRenderer = null;   // text mesh renderer reference

	// Unity Functions //

	// Start                                                             //
	// Initialises rotation, check lights object and get text refenences //
	void Start() {
		// Add initial starting rotation
		mYRotation += mStartOffset;

		// Check light object have been assigned
		if( !mLight1 || !mLight2 || !mLight3 ) {
			Debug.Log( "Light objects not assigned!" );
		}

		// Check text object then get references
		if( mTextObject ) {
			//mTextMesh     = mTextObject.GetComponent< TextMesh >();
			mTextRenderer = mTextObject.GetComponent< Renderer >();
		} else {
			Debug.Log( "Text object not assigned!" );
		}

		// Initially disable text mesh
		mTextRenderer.enabled = false;
	}

	// Update                                                                                 //
	// Checks for availabilty and correct input, then activates rotation for the set duration //
	void Update() {
		// If available and correct input is detected
		if( mAvailable && Input.GetButtonDown( mInputString ) ) {
			GameController.Instance.SetDecoys( true );
			mActive = true;
		}

		// When active - rotate lights till duration expires
		// Light3 does not move - it has a LightPulse component
		if( mActive ) {
			// Enable lights 1 & 2
			mLight1.enabled = true;
			mLight2.enabled = true;
			mLight3.enabled = true;

			// Disable text renderer
			mTextRenderer.enabled = false;

			// Increase Y rotation 
			mYRotation += Time.deltaTime * mSpeed;

			// Increase time spent active
			mActiveTime += Time.deltaTime;

			// Rotate both lights clockwise - offset light 2 by 180 degrees
			mLight1.transform.rotation = Quaternion.Euler( new Vector3( mXRotation, mYRotation , mZRotation ) );
			mLight2.transform.rotation = Quaternion.Euler( new Vector3( mXRotation, ( mYRotation + mLight2Offset ), mZRotation ) );

			// Keep Y rotation within 0 - 360 range
			if( mYRotation > 360.0f ) {
				mYRotation = 0.0f;
			}

			// Shutdown condition
			if( mActiveTime > mDuration ) {
				mActiveTime = 0.0f;
				GameController.Instance.SetDecoys( false );
				mActive = false;
			}
		// When not active - turn off lights
		} else {
			mLight1.enabled = false;
			mLight2.enabled = false;
			mLight3.enabled = false;
		}

		// Show text mesh if not active but available
		if( !mActive && mAvailable ) {
			mTextRenderer.enabled = true;
		} else {
			mTextRenderer.enabled = false;
		}
	}

	// IsAvailable                //
	// Getter for mAvailable flag //
	public bool IsAvailable() {
		return mAvailable;
	}

	// SetAvailability            //
	// Setter for mAvailable flag //
	public void SetAvailability( bool availability ) {
		mAvailable = availability;
	}

	// IsActive                //
	// Getter for mActive flag //
	public bool IsActive() {
		return mActive;
	}
}
