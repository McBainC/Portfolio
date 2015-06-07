#region Details
// ----------------------------------------------------------------- //
// Name:    Player Class                                             //
// Author:  Chris McBain                                             //
// Created:                                                          //
// Purpose:                                                          //
// - Represents the Player within the game environemnt               //
// - Responds to Xbox 360 controls:                                  //
//   - Left Trigger       -> Crouch                                  //
//   - Right Trigger      -> Throw Flare (hold to increase distance) //
//   - Left Analog Stick  -> Movement                                //
//   - Right Analog Stick -> Throw Direction                         //
// - Analog stick controls respond to actual input amounts           //
// - Public access to the PlayerState and Flare                      //
// ----------------------------------------------------------------- //
#endregion

#region Includes
// Unity Includes //
using UnityEngine;
using System.Collections;
#endregion

// Player Class //
public class Player : MonoBehaviour {
	// Unity Editor Variables //
	[ SerializeField ] private float      mSpeedScale    = 15.0f;  // speed scalar 
	[ SerializeField ] private float      mThrowForce    = 500.0f; // flare throw force
	[ SerializeField ] private float      mMaxThrowTime  = 3.0f;   // maximum duration for increased throw distance
	[ SerializeField ] private GameObject mBody          = null;   // body cylinder reference
	[ SerializeField ] private GameObject mFlarePrefab   = null;   // flare prefab reference
	[ SerializeField ] private GameObject mFlareMarker   = null;   // flare marker reference
	[ SerializeField ] private Vector3    mStartPosition = Vector3.zero; // players starting position

	// Private Const Variables //
	private const float PLAYER_SCALE = 0.3f;

	// Private Variables //
	private GameObject mFlare         = null;  // active flare object
	private Rigidbody  mRigidbody     = null;  // rigidbody component reference
	private Light      mLight         = null;  // players spotlight reference
	private float      mStanding      = 0.5f;  // standing 'height'
	private float      mCrouched      = 0.25f; // crouched 'height'
	private float      mCrouchedScale = 1.5f;  // crouched speed scalar
	private float      mThrowTime     = 0.0f;  // duration of throw trigger held
	private float      mLightCrouched = 2.0f;  // intensity of light when crouched
	private float      mLightStanding = 8.0f;  // intensity of lightwhen standing
	
	// PlayerState Variables //
	public enum PlayerState { CROUCHED, SNEAKING, STANDING, RUNNING };
	private     PlayerState mCurrentState;

	// Unity Functions //

	// Start                                                          //
	// Gets objects rigidbody component and sets initial player state //
	void Start () {
		mRigidbody    = GetComponent< Rigidbody >();
		mLight        = GetComponentInChildren< Light > ();
		mCurrentState = PlayerState.STANDING;
	}

	// Update                                                   //
	// Checks for player input:                                 //
	// - throwing a flare (right trigger & right analog stick)  //
	// - moving the player (left trigger and left analog stick) //
	// - moving the flare marker (right analog stick)           //
	void Update () {
		CheckForAction();
		DeterminePlayerState();
		UpdateFlareMarker();
		MovePlayer();
	}

	// Public Functions //

	// GetPlayerState      //
	// Player state getter //
	public PlayerState GetPlayerState() {
		return mCurrentState;
	}
	
	// FlarePosition                                                     //
	// Getter for the flares position - returns zero vector if it doesnt //
	public Vector3 FlarePosition() {
		if( mFlare ) {
			return mFlare.transform.position;
		} else {
			return Vector3.zero;
		}
	}

	// Reset                                                                 //
	// Return the player to its starting position and reset player vairables //
	public void Reset() {
		transform.position = mStartPosition;
		Destroy( mFlare );
		mFlare = null;
		mThrowTime = 0.0f;
	}

	// Destroy Flare                 //
	// Destroys the flare gameobject //
	public void DestroyFlare() {
		if( mFlare ) {
			Destroy( mFlare );
			mFlare = null;
		}
	}

	// Private Functions //

	// UpdateFlareMarker                                              //
	// Moves flare marker in response to right analog sticks position //
	private void UpdateFlareMarker() {
		mFlareMarker.transform.position = new Vector3( transform.position.x - Input.GetAxis( "Right_Stick_Horizontal" ),
		                                               mFlareMarker.transform.position.y,
		                                               transform.position.z - Input.GetAxis( "Right_Stick_Vertical" ) );
	}

	// DeterminePlayerState                                                                                //
	// Uses current "Crouch" input state and player objects velocity to determine the current player state //
	private void DeterminePlayerState() {
		// If 'Crouch' is held and speed is zero then player is crouched
		if( ( Input.GetAxis( "Crouch" ) > 0.0f ) && ( mRigidbody.velocity == Vector3.zero ) ) {
			mBody.transform.localScale = new Vector3( PLAYER_SCALE, mCrouched, PLAYER_SCALE );
			mBody.transform.position   = new Vector3( transform.position.x, mCrouched, transform.position.z );
			mLight.intensity           = mLightCrouched;
			mCurrentState              = PlayerState.CROUCHED;
			// Otherwise 'crouch' is held and speed is non-zero - player is sneaking
		} else if( Input.GetAxis( "Crouch" ) > 0.0f ) { 
			mBody.transform.localScale = new Vector3( PLAYER_SCALE, mCrouched, PLAYER_SCALE );
			mBody.transform.position   = new Vector3( transform.position.x, mCrouched, transform.position.z );
			mLight.intensity           = mLightCrouched;
			mCurrentState              = PlayerState.SNEAKING;
			// If not crouching and not moving - player is standing
		} else if( mRigidbody.velocity == Vector3.zero ) {
			mBody.transform.localScale = new Vector3( PLAYER_SCALE, mStanding, PLAYER_SCALE );
			mBody.transform.position   = new Vector3( transform.position.x, mStanding, transform.position.z );
			mLight.intensity           = mLightStanding;
			mCurrentState              = PlayerState.STANDING;
			// Final possibility - player is running
		} else {
			mBody.transform.localScale = new Vector3( PLAYER_SCALE, mStanding, PLAYER_SCALE );
			mBody.transform.position   = new Vector3( transform.position.x, mStanding, transform.position.z );
			mLight.intensity           = mLightStanding;
			mCurrentState              = PlayerState.RUNNING;
		}
	}
	
	// CheckForAction                                 //
	// Throws a flare if "Throw Flare" is pressed     //
	// Force increases the longer the control is held //
	private void CheckForAction() {
		// Time trigger is held down
		if( Input.GetAxis( "Throw Flare" ) > 0.0f ) {
			mThrowTime += Time.deltaTime;
			
			if( mThrowTime > mMaxThrowTime ) {
				ThrowFlare();
				mThrowTime = 0.0f;
			}
		} else if ( mThrowTime > 0.0f ) {
			ThrowFlare();
			mThrowTime = 0.0f;
		} else {
			mThrowTime = 0.0f;
		}
	}

	// MovePlayer                                                                                                            //
	// Moves player by adding a force to its rigidbody component based on the current position of the controllers left stick //
	private void MovePlayer() {
		// Input is modified by a user defined scalar divided by the state scalar - to slow down movement while crouched
		switch( mCurrentState ) {
			case PlayerState.CROUCHED: {
				mRigidbody.AddForce( ( -Input.GetAxis( "Left_Stick_Horizontal" ) * ( mSpeedScale / mCrouchedScale ) ), 0.0f, ( -Input.GetAxis( "Left_Stick_Vertical" ) * ( mSpeedScale / mCrouchedScale ) ) );
				break;
			}
			case PlayerState.SNEAKING: {
				mRigidbody.AddForce( ( -Input.GetAxis( "Left_Stick_Horizontal" ) * ( mSpeedScale / mCrouchedScale ) ), 0.0f, ( -Input.GetAxis( "Left_Stick_Vertical" ) * ( mSpeedScale / mCrouchedScale ) ) );
				break;
			}
			case PlayerState.RUNNING: {
				mRigidbody.AddForce( ( -Input.GetAxis( "Left_Stick_Horizontal" ) * mSpeedScale ), 0.0f, ( -Input.GetAxis( "Left_Stick_Vertical" ) * mSpeedScale ) );
				break;
			}
			case PlayerState.STANDING: {
				mRigidbody.AddForce( ( -Input.GetAxis( "Left_Stick_Horizontal" ) * mSpeedScale ), 0.0f, ( -Input.GetAxis( "Left_Stick_Vertical" ) * mSpeedScale ) );
				break;
			}
		}
	}

	// ThrowFlare                                                                                     //
	// Throws a flare prefab based on current direction of right analogue stick and time trigger held //
	private void ThrowFlare() {
		if( !mFlare ) {
			mFlare = ( GameObject )Instantiate( mFlarePrefab, new Vector3( transform.position.x - Input.GetAxis( "Right_Stick_Horizontal" ), 1.5f, transform.position.z - Input.GetAxis( "Right_Stick_Vertical" ) ), transform.rotation );
			Rigidbody rigidbody = mFlare.GetComponent< Rigidbody >();
			rigidbody.AddForce( new Vector3( ( -Input.GetAxis( "Right_Stick_Horizontal" ) * ( mThrowTime * mThrowForce ) ), 0.0f, ( -Input.GetAxis( "Right_Stick_Vertical" ) * ( mThrowTime * mThrowForce ) ) ) );
		}
	}
}
