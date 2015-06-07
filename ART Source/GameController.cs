#region Details
// -----------------------------------------------------------------------------------------------------------//
// Name:    GameController Class                                                                              //
// Author:  Chris McBain                                                                                      //
// Created: 220115                                                                                            //
// Purpose:                                                                                                   //
// - Main Game Controller - Singleton                                                                         //
// - has references to the Player (and therefore Flare) and Decoy objects which can be recognised by the A.I. // 
// - is responsible for determining if each of these objects can currently be 'seen' or 'heard' by the A.I.   //
// - uses the attached ARTInput component to create a suitable input vector containing this data              //
// - sends the input vector to an ART2 to be classified                                                       //
// - manages 'Decoy' GameObjects - only one can be 'active' at once                                           //
// - contains and manages the PatrolNodes - list of positions and radius to allow the A.I. to patrol          //
// -----------------------------------------------------------------------------------------------------------//
#endregion

#region Includes
// Unity Includes //
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
#endregion

// GameController //
public class GameController : MonoBehaviour {
	// Static Instance of GameController
	private static GameController sInstance;
	
	// There can only be one!
	public static GameController Instance {
		get {
			if( !sInstance ) {
				UnityEngine.Object[] tmp_objects = GameObject.FindObjectsOfType( typeof( GameController ) );
				if( tmp_objects.Length <= 0 ) {
					Debug.LogError( "There is no Gameobject for GameController active!" );
				} else if( tmp_objects.Length > 1 ) {
					Debug.LogError( "There is more than one instance of GameController active!" );
				} else {
					sInstance = ( GameController )tmp_objects[ 0 ];
				}
			}
			
			return sInstance;
		}
	}

	// Unity Editor Variables //
	[ SerializeField ] private List< PatrolNode > mPatrolNodes      = new List< PatrolNode >();
	[ SerializeField ] private List< Decoy >      mDecoys           = new List< Decoy >();
	[ SerializeField ] private AI                 mAI               = null;
	[ SerializeField ] private Player             mPlayer           = null;
	[ SerializeField ] private Vector3            mStartingPosition = Vector3.zero;

	// Const Variables //
	private const float PLAYER_CROUCHED_HEIGHT = 0.25f;
	private const float PLAYER_STANDING_HEIGHT = 0.75f;
	private const float FLARE_HEIGHT           = 0.125f;
	private const float DECOY_HEIGHT           = 0.75f;

	// Private Variables //
	private Vector3 mPlayerPosition = Vector3.zero;
	private Vector3 mFlarePosition  = Vector3.zero;
	private Vector3 mDecoyPosition  = Vector3.zero;
	private Vector3 mAIPosition     = Vector3.zero;

	// ART2 Network and ART2 Input 
	private ART2     mART2Network = null;            // reference to ART2 network component
	private ARTInput mARTInput    = null;            // reference to ARTInput component
	private float[]  mARTOutput   = new float[ 18 ]; // ART2 output vector

	// Unity Functions //

	// Start                                         //
	// Get reference to ARTInput and ART2 components //
	void Start() {
		mARTInput    = GetComponent< ARTInput >();
		mART2Network = GetComponent< ART2 >();
	}

	// Update                                                         //
	// Gets and sets the required positional data for the ART network //
	void Update() {
		// Win Condition
		CheckForContact();

		// Update ART2 input vectors
		mPlayerPosition = mPlayer.transform.position;
		mFlarePosition  = mPlayer.FlarePosition();
		mDecoyPosition  = GetActiveDecoyPosition();
		mAIPosition     = mAI.transform.position;

		UpdatePatrolNodes();
		TakeAction();

		// Quit Condition
		if( Input.GetButtonDown( "Cancel" ) ) {
			Application.Quit();
		}
	}

	// FixedUpdate                                                                                                  //
	// Sends the required positional data to be turned into an ART2 input vector                                    //
	// Checks if the ART2Input object contains a new vector - then sends its to the ART2 network for classification //
	void FixedUpdate() {
		SetInputData();

		// If there is a new valid input - send it to the network
		if( mARTInput.NewInputAvailable() ) {
			mART2Network.Run( mARTInput.GetARTInput() );
			mARTOutput = mART2Network.ReturnExemplar();
			TakeAction();
			mARTInput.ResetARTInput();
		}
	}

	// Public Functions //

	// SetDecoys                                                           //
	// Sets all Decoys as either available (true) or not available (false) //
	public void SetDecoys( bool locked ) {
		for( int i = 0; i < mDecoys.Count; i++ ) {
			if( locked ) {
				mDecoys[ i ].SetAvailability( false );
			} else {
				mDecoys[ i ].SetAvailability( true );
			}
		}
	}

	// GetPatrolNodes                       //
	// Returns an array of the patrol nodes //
	public PatrolNode[] GetPatrolNodes() {
		return mPatrolNodes.ToArray ();
	}

	// ResetPatrolNodes                                //
	// Sets all the patrol nodes mVisted flag to false //
	public void ResetPatrolNodes() {
		for( int i = 0; i < mPatrolNodes.Count; i++ ) {
			mPatrolNodes[ i ].SetVisited( false );
		}
	}

	// Private Functions //

	// TakeAction    //
	// Dirty Hack... //
	private void TakeAction() {
		//Debug.Log( "Take Action Called" );
		if( WithinAILOS( mPlayerPosition, PLAYER_CROUCHED_HEIGHT ) ) {
			mAI.SetContact( mPlayerPosition, 6.0f, AI.AIState.CHASING );
			//Debug.Log( "Action = Chase Player" );
		} else if( ( mPlayer.GetPlayerState() == Player.PlayerState.STANDING ) && ( WithinAILOS( mPlayerPosition, PLAYER_STANDING_HEIGHT ) ) ) {
			mAI.SetContact( mPlayerPosition, 6.0f, AI.AIState.CHASING );
			//Debug.Log( "Action = Chase Player" );
		} else if( ( mFlarePosition != Vector3.zero ) && ( WithinAILOS( mFlarePosition, FLARE_HEIGHT ) ) ) {
			mAI.SetContact( mFlarePosition, 6.0f, AI.AIState.CHASING );
			//Debug.Log( "Action = Chase Flare" );
		} else if( ( mDecoyPosition != Vector3.zero ) && ( WithinAILOS( mDecoyPosition, DECOY_HEIGHT ) ) ) {
			mAI.SetContact( mDecoyPosition, 6.0f, AI.AIState.CHASING );
			//Debug.Log( "Action = Chase Decoy" );
		} else if( ( mPlayer.GetPlayerState() == Player.PlayerState.RUNNING ) && ( !WithinAILOS( mPlayerPosition, PLAYER_STANDING_HEIGHT ) ) ) {
			mAI.SetContact( mPlayerPosition, 8.0f, AI.AIState.INVESTIGATING );
			//Debug.Log( "Action = Investigate Player" );
		} else if( mFlarePosition != Vector3.zero ) {
			mAI.SetContact( mFlarePosition, 8.0f, AI.AIState.INVESTIGATING );
			//Debug.Log( "Action = Investigate Flare" );
		} else if( mDecoyPosition != Vector3.zero ) {
			mAI.SetContact( mDecoyPosition, 8.0f, AI.AIState.INVESTIGATING );
			//Debug.Log( "Action = Investigate Decoy" );
		} else {
			//Debug.Log( "Action = Patrolling" );
		}
	}
	
	// CheckForContact            //
	// Resets the player and A.I. //
	private void CheckForContact() {
		// Player Contact
		Vector3 distanceVector = mAIPosition - mPlayerPosition;
		if( distanceVector.magnitude < 1.0f ) {
			mPlayer.Reset();
			mAI.Reset();
			mAI.transform.position = mStartingPosition;
		}

		// Flare Contact
		distanceVector = mAIPosition - mFlarePosition;
		if( distanceVector.magnitude < 1.0f ) {
			mPlayer.DestroyFlare();
		}

		// Decoy Contact
		/*distanceVector = mAIPosition - mDecoyPosition;
		if( distanceVector.magnitude < 1.0f ) {
			SetDecoys( true );
		}*/
	}

	// SetInputData
	// Checks each of the three objects that the A.I. can see and hear - the player, flare and a decoy //
	// Checks if each object is either emitting an audio contact, or is visible to the A.I             //
	// Sends position data along with its contact type to the ARTInput object                          //
	private void SetInputData() {
		// Player Vector Logic //
		// Crouched OR Sneaking - possible visual contact only //
		if( ( mPlayer.GetPlayerState() == Player.PlayerState.CROUCHED ) || ( mPlayer.GetPlayerState() == Player.PlayerState.SNEAKING ) ) {
			if( WithinAILOS( mPlayerPosition, PLAYER_CROUCHED_HEIGHT ) ) {
				mARTInput.AddPlayerContact( mPlayerPosition, ARTInput.ContactType.VISUAL );
				//Debug.Log(" AI can see Player CROUCHED or SNEAKING @ " + mPlayerPosition );
			}
		// Standing - possible visual contact only
		} else if ( mPlayer.GetPlayerState() == Player.PlayerState.STANDING ) {
			if( WithinAILOS( mPlayerPosition, PLAYER_STANDING_HEIGHT ) ) {
				mARTInput.AddPlayerContact( mPlayerPosition, ARTInput.ContactType.VISUAL );
				//Debug.Log(" AI can see Player STANDING @ " + mPlayerPosition );
			}
		// Running - possible visual contact AND constant audio contact
		} else {
			if( WithinAILOS( mPlayerPosition, PLAYER_STANDING_HEIGHT ) ) {
				mARTInput.AddPlayerContact( mPlayerPosition, ARTInput.ContactType.VISUAL_AND_AUDIO );
				//Debug.Log(" AI can see and hear Player RUNNING @ " + mPlayerPosition );
			} else {
				mARTInput.AddPlayerContact( mPlayerPosition, ARTInput.ContactType.AUDIO );
				//Debug.Log(" AI can hear Player RUNNING @ " + mPlayerPosition );
			}
		}
		
		// Flare Vector Logic //
		if( mFlarePosition != Vector3.zero ) {
			if( WithinAILOS( mFlarePosition, FLARE_HEIGHT ) ) {
				mARTInput.AddFlareContact( mFlarePosition, ARTInput.ContactType.VISUAL_AND_AUDIO );
				//Debug.Log(" AI can see and hear Flare @ " + mFlarePosition );
			} else {
				mARTInput.AddFlareContact( mFlarePosition, ARTInput.ContactType.AUDIO );
				//Debug.Log(" AI can hear Flare @ " + mFlarePosition );
			}
		}
		
		// Decoy Vector Logic //
		if( mDecoyPosition != Vector3.zero ) {
			if( WithinAILOS( mDecoyPosition, DECOY_HEIGHT ) ) {
				mARTInput.AddDecoyContact( mDecoyPosition, ARTInput.ContactType.VISUAL_AND_AUDIO );
				//Debug.Log(" AI can see and hear Decoy @ " + mDecoyPosition );
			} else {
				mARTInput.AddDecoyContact( mDecoyPosition, ARTInput.ContactType.AUDIO );
				//Debug.Log(" AI can hear Decoy @ " + mDecoyPosition );
			}
		}
	}

	// UpdatePatrolNodes                                               //
	// Detects if A.I. reaches a patrol node - and flags it as visited //
	private void UpdatePatrolNodes() {
		// Patrol Logic
		if( mAI.GetAIState() == AI.AIState.PATROLLING ) {
			// PatrolNodes
			for( int i = 0; i < mPatrolNodes.Count; i++ ) {
				float distance = Vector3.Distance( mAIPosition, mPatrolNodes[ i ].transform.position );
				if( distance < 1.0f ) {
					mPatrolNodes[ i ].SetVisited( true );
				}
			}
		}
	}

	// GetActiveDecoyPosition                         //
	// Gets the position of whichever decoy is active //
	private Vector3 GetActiveDecoyPosition() {
		// Check if any of the decoys is active
		for( int i = 0; i < mDecoys.Count; i++ ) {
			if( mDecoys[ i ].IsActive() ) {
				return mDecoyPosition = mDecoys[ i ].transform.position;
			}
		}

		// Otherwise return zero vector
		return Vector3.zero;
	}

	// WithinAILOS                                                                 //
	// Checks if passed position can be seen by A.I's primary or perhiperal vision //
	private bool WithinAILOS( Vector3 targetPosition, float height ) {
		// -> Perpiheral Vision Check, short range / wide arc    <-  -> Primary vision check, long range / small arc         <-
		if( ( WithinAIArc( targetPosition, height, 7.0f, 70.0f ) ) || ( WithinAIArc( targetPosition, height, 15.0f, 30.0f ) ) ) { // ***watch AI magic numbers!***
			return true;
		}

		// Otherwise false
		return false;
	}

	// WithinAIArc                                                                                                      //
	// Uses vector based angle detection and a raycast to determine if the passed target position is in 'line-of-sight' //
	// Operates in 3D space to allow wall colliders, truely works in a 2D fashion - requires a 'height' or depth        //
	private bool WithinAIArc( Vector3 targetPosition, float height, float radius, float angle ) {
		// Distance vector from A.I. to passed position 
		Vector3 distanceVector = targetPosition - mAIPosition;

		// If within passed radius (detection distance)
		if( distanceVector.magnitude < radius ) {
			// Dot product of normalised direction vectors
			// Returns 1 for same direction to 0 for 90 degrees
			Vector3 sight     = mAI.transform.forward.normalized;
			Vector3 direction = distanceVector.normalized;
			float   cosine    = Vector3.Dot( sight, direction );
			float   degrees   = Mathf.Acos( cosine ) * Mathf.Rad2Deg;
			
			// If within passed arc
			if( degrees <= angle ) {
				// Raycast to target location - not hits (false) means line-of-sight is clear
				if( !Physics.Linecast( new Vector3( mAIPosition.x, height, mAIPosition.z ),
				                       new Vector3( targetPosition.x, height, targetPosition.z ) ) ) {
					return true;
				}
			}
		}

		// Otherwise out of range (radius), out of arc (angle) or concealed (positive linecast check) = false
		return false;
	}
}
