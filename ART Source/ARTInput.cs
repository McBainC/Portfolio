#region Details
// ------------------------------------------------------------------ //
// Name:    ARTInput Class                                            //
// Author:  Chris McBain                                              //
// Created:                                                           //
// Purpose:                                                           //
// - Builds and manages the ART Network input vector                  //
// - Currently hard-coded for test environment:                       //
//   - input length of 18                                             //
//   - values of vector indices declared                              //
// - Has a public function to determine if a new valid input is ready //
// ------------------------------------------------------------------ //
#endregion

#region Includes
// Unity Includes //
using UnityEngine;
using System.Collections;
#endregion

// ARTInput Class //
public class ARTInput : MonoBehaviour {
	// Unity Editor Variables //
	//[ SerializeField ] private int mInputlength = 18;

	// Const Variables //
	private const int mInputLength = 18;

	// Input Index Variables - (0-17) 18 (mInputLength) Indices //
	// Visual Indices - (0-8) //
	private const int mPlayerVisualX = 0;
	private const int mPlayerVisualY = 1;
	private const int mPlayerVisualZ = 2;
	private const int mFlareVisualX  = 3;
	private const int mFlareVisualY  = 4;
	private const int mFlareVisualZ  = 5;
	private const int mDecoyVisualX  = 6;
	private const int mDecoyVisualY  = 7;
	private const int mDecoyVisualZ  = 8;

	// Audio Indices - (9-17) //
	private const int mPlayerAudioX = 9;
	private const int mPlayerAudioY = 10;
	private const int mPlayerAudioZ = 11;
	private const int mFlareAudioX  = 12;
	private const int mFlareAudioY  = 13;
	private const int mFlareAudioZ  = 14;
	private const int mDecoyAudioX  = 15;
	private const int mDecoyAudioY  = 16;
	private const int mDecoyAudioZ  = 17;

	// Private Variables //
	private float[] mInputVector;
	private float[] mPreviousInput;

	// Contact State Variables //
	public enum ContactType { VISUAL, AUDIO, VISUAL_AND_AUDIO };

	// Unity Functions //

	// Start                                                    //
	// Creates and initialises input and previous input vectors //
	void Start () {
		// Create float arrays at correct length
		mInputVector   = new float[ mInputLength ];
		mPreviousInput = new float[ mInputLength ];

		// Initialise to all zeros
		ResetInputVector();
		InitialisePreviousInput();
	}

	// Public Functions //

	// Initialise                            //
	// Creates float array at correct length //
	/*public void Initialise() {
		mInputVector = new float[ mInputLength ];
	}*/

	// ResetARTInput                        //
	// Resets both input and previous input //
	public void ResetARTInput() {
		ResetInputVector();
		InitialisePreviousInput();
	}

	// ResetInputVector                              //
	// Sets all of the input vectors entries to zero //
	public void ResetInputVector() {
		for( int i = 0; i < mInputLength; i++ ) {
			mInputVector[ i ] = 0.0f;
		}
	}

	// AddPlayerContact                                                                       //
	// Sets the player segments of the input vector based on passed position and contact type //
	public void AddPlayerContact( Vector3 playerPosition, ContactType playerContactType ) {
		switch( playerContactType ) {
			case ContactType.VISUAL: {
				mInputVector[ mPlayerVisualX ] = playerPosition.x;
				mInputVector[ mPlayerVisualY ] = playerPosition.y;
				mInputVector[ mPlayerVisualZ ] = playerPosition.z;
				break;
			}
			case ContactType.AUDIO:{
				mInputVector[ mPlayerAudioX ] = playerPosition.x;
				mInputVector[ mPlayerAudioY ] = playerPosition.y;
				mInputVector[ mPlayerAudioZ ] = playerPosition.z;
				break;
			}
			case ContactType.VISUAL_AND_AUDIO: {
				mInputVector[ mPlayerVisualX ] = playerPosition.x;
				mInputVector[ mPlayerVisualY ] = playerPosition.y;
				mInputVector[ mPlayerVisualZ ] = playerPosition.z;
				mInputVector[ mPlayerAudioX ]  = playerPosition.x;
				mInputVector[ mPlayerAudioY ]  = playerPosition.y;
				mInputVector[ mPlayerAudioZ ]  = playerPosition.z;
				break;
			}
		}
	}

	// AddFlareContact                                                                       //
	// Sets the flare segments of the input vector based on passed position and contact type //
	public void AddFlareContact( Vector3 flarePosition, ContactType flareContactType ) {
		switch( flareContactType ) {
			case ContactType.VISUAL: {
				mInputVector[ mFlareVisualX ] = flarePosition.x;
				mInputVector[ mFlareVisualY ] = flarePosition.y;
				mInputVector[ mFlareVisualZ ] = flarePosition.z;
				break;
			}
			case ContactType.AUDIO:{
				mInputVector[ mFlareAudioX ] = flarePosition.x;
				mInputVector[ mFlareAudioY ] = flarePosition.y;
				mInputVector[ mFlareAudioZ ] = flarePosition.z;
				break;
			}
			case ContactType.VISUAL_AND_AUDIO: {
				mInputVector[ mFlareVisualX ] = flarePosition.x;
				mInputVector[ mFlareVisualY ] = flarePosition.y;
				mInputVector[ mFlareVisualZ ] = flarePosition.z;
				mInputVector[ mFlareAudioX ]  = flarePosition.x;
				mInputVector[ mFlareAudioY ]  = flarePosition.y;
				mInputVector[ mFlareAudioZ ]  = flarePosition.z;
				break;
			}
		}
	}

	// AddDecoyContact                                                                       //
	// Sets the decoy segments of the input vector based on passed position and contact type //
	public void AddDecoyContact( Vector3 decoyPosition, ContactType decoyContactType ) {
		switch( decoyContactType ) {
			case ContactType.VISUAL: {
				mInputVector[ mDecoyVisualX ] = decoyPosition.x;
				mInputVector[ mDecoyVisualY ] = decoyPosition.y;
				mInputVector[ mDecoyVisualZ ] = decoyPosition.z;
				break;
			}
			case ContactType.AUDIO:{
				mInputVector[ mDecoyAudioX ] = decoyPosition.x;
				mInputVector[ mDecoyAudioY ] = decoyPosition.y;
				mInputVector[ mDecoyAudioZ ] = decoyPosition.z;
				break;
			}
			case ContactType.VISUAL_AND_AUDIO: {
				mInputVector[ mDecoyVisualX ] = decoyPosition.x;
				mInputVector[ mDecoyVisualY ] = decoyPosition.y;
				mInputVector[ mDecoyVisualZ ] = decoyPosition.z;
				mInputVector[ mDecoyAudioX ]  = decoyPosition.x;
				mInputVector[ mDecoyAudioY ]  = decoyPosition.y;
				mInputVector[ mDecoyAudioZ ]  = decoyPosition.z;
				break;
			}
		}
	}

	// NewInputAvailable
	// Flag that the ARTInput has a new input array ready to send to the network //
	// - the current input is different from the previous - avoid duplicates     //
	// - the current input is non-zero - ART network wont accept all zero inputs //
	public bool NewInputAvailable() {
		if( ( !InputsAreEqual() ) && ( ValidInput() ) ) {
			return true;
		} else {
			return false;
		}
	}

	// GetARTInput                                                //
	// Return mInputVector and sets mPreviousInput at the current //
	public float[] GetARTInput() {
		// Set previous input
		for( int i = 0; i < mInputLength; i++ ) {
			mPreviousInput[ i ] = mInputVector[ i ];		
		}

		// Return curren input vector
		return mInputVector;
	}

	// Private Functions //
	
	// InitialisePreviousInput                        //
	// Sets all of the previous input entries to zero //
	private void InitialisePreviousInput() {
		for( int i = 0; i < mInputLength; i++ ) {
			mPreviousInput[ i ] = 0.0f;
		}
	}

	// InputsAreEqual                                       //
	// Checks for equality between inout and previous input //
	private bool InputsAreEqual() {
		// If any two of the vectors are not equal, return false
		for( int i = 0; i < mInputLength; i++ ) {
			if( mInputVector[ i ] != mPreviousInput[ i ] ) {
				return false;
			}
		}

		// Otherwise return true;
		return true;
	}

	// ValidInput                                //
	// Checks that input vector is not all zeros //
	private bool ValidInput() {
		// Initialise to false
		bool validInput = false;

		// If any part of the input vector is non-zero - set as validInput (true)
		for( int i = 0; i < mInputLength; i++ ) {
			if( mInputVector[ i ] != 0.0f ) {
				validInput = true;
			}
		}

		// Return valid input flag
		return validInput;
	}
}
