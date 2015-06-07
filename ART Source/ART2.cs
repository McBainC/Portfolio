#region Includes
// Unity Includes //
using UnityEngine;
using System.Collections;
using System.Collections.Generic; // ART2 only
#endregion

#region Details
// ----------------------------------------------------------------------- //
// Name:    RealOutputNode                                                 //
// Author:  Chris McBain                                                   //
// Created:                                                                //
// Purpose:                                                                //
// - Represents an ART2 network output node                                //
// - Must be initialised with the number of input/comparison nodes         //
// - Contains functionality to return its exemplar, weighting and train it //
// ----------------------------------------------------------------------- //
// Binary -> Real Change Log:                                              //
// - Converison to floats for ART2 vectors                                 //
// - GetWeighting() - HACKED to only return positive weightings            //
// - Feed-Forward weight scaling equation altered:                         //
// - T vector = value / ( 1 + magnitude of X vector )                      //
// - const L removed (see above)                                           //
// - initial exemplar vector now all 0's                                   //
// ----------------------------------------------------------------------- //
// As Gizmos will not draw for the final application,                      //
// drawing functionality has been added!                                   // 
#endregion

// RealOutputNode Class //
public class RealOutputNode : MonoBehaviour {
	// Private Variables //
	private float[] mFeedForwardWeights;         // BottomUp weights / (W)Vector
	private float[] mFeedBackWeights;            // TopDown weights / (T)Vector / Exemplar    
	private int     mInputLength;                // Number of Input/Comparison nodes
	private bool    mDisabled           = false; // Flag for failing a vigilance test
	private bool    mTrained            = false; // Flag for recieving an Exemplar vector

	// Revised ART Network Drawing
	private List< GameObject > mARTSpheres         = new List< GameObject >(); // List containing ARTSphere prefabs

	// Initialise                                                //
	// Creates weight arrays at ART networks passed input length //
	// Sets initial feedforward and feedback weights
	public void Initialise( int inputLength, Vector3 ARTPosition, float outputIndex ) {
		mInputLength        = inputLength;
		mFeedForwardWeights = new float[ mInputLength ];
		mFeedBackWeights    = new float[ mInputLength ];
		
		InitialiseWeights();

		// Instantiate an ARTSphere prefab for each value 
		for( int i = 0; i <= mInputLength; i++ ) {
			GameObject ARTSphere;
			ARTSphere = Instantiate( Resources.Load( "ARTSphere", typeof( GameObject ) ) ) as GameObject;
			ARTSphere.transform.position   = new Vector3( ARTPosition.x + ( outputIndex * 2.0f ), ARTPosition.y, ARTPosition.z + ( i * 1.9f ) );
			ARTSphere.transform.localScale = new Vector3( 0.5f, 0.5f, 0.5f );
			mARTSpheres.Add( ARTSphere );
		}
	}

	// UpdateARTPosition            //
	// Changes anchor of ARTSpheres //
	public void UpdateARTPosition( float newX, int outputIndex ) {
		for( int i = 0; i <= mInputLength; i++ ) {
			mARTSpheres[ i ].transform.position = new Vector3( newX + ( outputIndex * 2.0f ), mARTSpheres[ i ].transform.position.y , mARTSpheres[ i ].transform.position.z );
		}
	}

	// SetAsTrained                     //
	// Sets the final ARTSpheres colour //
	public void SetAsTrained( Color colour ) {
		mARTSpheres[ mInputLength ].GetComponent< Renderer >().material.color = colour;
	}

	// IsDisabled                                                  //
	// To identify if this output node has failed a vigilance test //
	public bool IsDisabled() {
		return mDisabled;
	}
	
	// SetDisabled         //
	// Sets mDisabled flag //
	public void SetDisabled( bool isDisabled ) {
		mDisabled = isDisabled;
	}
	
	// IsTrained                                                //
	// To identify if this output node has recieved an exemplar //
	public bool IsTrained() {
		return mTrained;
	}
	
	// SetTrained         //
	// Sets mTrained flag //
	public void SetTrained( bool IsTrained ) {
		mTrained = IsTrained;
	}
	
	// GetFeedForwardWeights                                //
	// Returns this output nodes feedforward weights vector //
	public float[] GetFeedForwardWeights() {
		return mFeedForwardWeights;
	}
	
	// GetExemplar                               //
	// Returns this output nodes exemplar vector //
	public float[] GetExemplar() {
		return mFeedBackWeights;
	}
	
	// GetWeighting                                                 //               
	// Returns dot product of feedforward and input ( W.X ) vectors //
	public float GetWeighting( float[] xVector ) {
		float weighting = 0.0f;
		
		// Dot product
		for( int i = 0; i < mInputLength; i++ ) {
			weighting += mFeedForwardWeights[ i ] * xVector[ i ];
		}

		// ART2 Dirty Hack! - ensuring no negitive weightings ***REMOVED***
		/*if( weighting < 0.0f ) {
			weighting *= -1.0f;
		}*/

		return weighting;
	}
	
	// Train                                                                       //
	// FeedForword weights are set to become the normalised values of the exemplar //
	// FeedBack weights now match the zVector - the new Exemplar                   // 
	public void Train( float[] zVector, float magnitude ) {
		for( int i = 0; i < mInputLength; i++ ) {
			mFeedForwardWeights[ i ] = zVector[ i ] / ( 1 + magnitude );
			//mFeedForwardWeights[ i ] = zVector[ i ] / ( zVector[ i ] + magnitude );
			mFeedBackWeights[ i ]    = zVector[ i ];

			// Update ARTSpheres colour and radii with new feedforward weight value 
			if( mFeedBackWeights[ i ] < 0.0f ) {
				mARTSpheres[ i ].GetComponent< Renderer >().material.color = Color.blue;
				mARTSpheres[ i ].transform.localScale = new Vector3( ( mFeedForwardWeights[ i ] * -1.0f ),
				                                                     ( mFeedForwardWeights[ i ] * -1.0f ),
				                                                     ( mFeedForwardWeights[ i ] * -1.0f ) );
			} else if ( mFeedBackWeights[ i ] == 0 ) {
				mARTSpheres[ i ].GetComponent< Renderer >().material.color = Color.white;
				mARTSpheres[ i ].transform.localScale = new Vector3( 0.5f, 0.5f, 0.5f );
			} else {
				mARTSpheres[ i ].GetComponent< Renderer >().material.color = Color.red;
				mARTSpheres[ i ].transform.localScale = new Vector3( ( mFeedForwardWeights[ i ] * 1.0f ),
				                                                     ( mFeedForwardWeights[ i ] * 1.0f ),
				                                                     ( mFeedForwardWeights[ i ] * 1.0f ) );
			}
		}

		// Flag as trained
		SetTrained( true );
	}
	
	// InitialiseWeights                                                                              //
	// FeedBack weights = 0, FeedForward weights is dependant on the number of Input/Comparison nodes //
	private void InitialiseWeights() {
		// FeedForward weight scaling
		float feedForwardWeight = 1.0f / ( 1 + mInputLength );
		
		// Initialise weights
		for( int i = 0; i < mInputLength; i++ ) {
			mFeedForwardWeights[ i ] = feedForwardWeight;
			mFeedBackWeights[ i ]    = 0.0f;
		}
	}
}

#region Includes
// Includes //
//using UnityEngine;
//using System.Collections;
//using System.Collections.Generic;
#endregion

#region Details
// ---------------------------------------------------------------------------------------- //
// Name: ART2 Class                                                                         //
// Author: Chris McBain                                                                     //
// Created:                                                                                 //
// Purpose:                                                                                 //
// - Represents a hacked version of an ART2 network, based upon ART1's architecture         //
// - Details based around the biological/neural elements have been reworked                 //
// - Requires the length of the input vector (mNumberOfInputNodes)                          //
// -    "     the vigilance threshold value to initialise                                   //
// - Currently neither can be changed without destroying and recreating the network         //
// - Output/Recognition Field structure expands as required (List) - ***NO LIMITS SET***    //
// ---------------------------------------------------------------------------------------- //
// ART1 -> ART2 Change Log:                                                                 //
// - Input now accepts real number values (positive and negative)                           //
// - ART vectors changed to floats                                                          //
// - Vigilance of 0 = perfect match -> scaling upwards for increasing mis-match             //
// - Control1()      - reworked to use GetVectorMagnitude()                                 //
// - Comparison()    - heavily reworked (see function comments)                             //
// - VigilanceTest() - now returns magnitude of comparison (C/Z) vector                     //
// - SearchPhase()   - second call to Comparison() required - due to its reworking          //
// - Train()         - now used (X) vector and its length to train nodes                    //
//                   - error in adding new nodes fixed (only when training a blank node)    //
// ---------------------------------------------------------------------------------------- //
// As Gizmos will not draw for the final application, drawing functionality has been added! // 
#endregion

// ART2 Class //
public class ART2 : MonoBehaviour {
	// Unity Editor Variables //
	[ SerializeField ]                           int   mNumberOfInputNodes = 0;    // number of input/comparison nodes
	[ Range( 0.0f, 100.0f ) ] [ SerializeField ] float mVigilanceThreshold = 0.0f; // vigilance threshold setting

	[ SerializeField ] private Vector3 mARTPosition = new Vector3( 0.0f, -10.0f, 0.0f ); // network drawing anchor 

	// Private Variables //
	// ART2 Vectors //
	private float[] mXVector; // input (X) vector
	private float[] mCVector; // input (W/Z) comparison vector
	private float[] mTVector; // output (T) comparison vector / exemplar 
	
	private int   mBestNeuron = -1;    // index for 'best match' output node in mRecognitionField list
	private float mARTX       = 0.0f;  // ARTSpheres anchor z coordinate 
	private bool  mControl2   = false; // control2 signal flag
	private bool  mReset      = false; // search flag
	
	private List< RealOutputNode > mRecognitionField = new List< RealOutputNode > (); // output/recognition node list (field)
	
	//private List< float[] > mInputPatterns = new List< float[] >(); // test list to hold input data ***REMOVED***

	// Unity Functions //

	// Start                    //
	// Initialises ART2 network //
	void Start() {
		Initialise();
	}

	// ***Test Data Runs Removed***
	// Update                                                                //
	// Test function rebuilds network each frame                             //
	// Allows impact of changing vigilance threshold to be seen in real-time //
	void Update() {
		//Initialise();
		
		// Add test data in vector in float[] format
		//mInputPatterns.Add( new float[] {   9.94f,    0.5f,   6.42f,    0.0f,    0.0f,    0.0f,   2.24f,    0.5f, -12.96f, -15.28f,    0.5f,   6.59f } );
		//mInputPatterns.Add( new float[] {  10.13f,    0.5f,   7.04f,    0.0f,    0.0f,    0.0f,   1.77f,    0.5f, -12.97f, -15.27f,    0.5f,   6.58f } );
		//mInputPatterns.Add( new float[] {  14.44f,    0.5f, -02.40f, -11.94f,    0.5f, -23.35f, -04.75f,    0.5f,  -4.28f,    0.0f,    0.0f,    0.0f } );
		//mInputPatterns.Add( new float[] {  15.01f,    0.5f, -02.25f, -12.01f,    0.5f, -23.57f, -05.25f,    0.5f,  -4.12f,    0.0f,    0.0f,    0.0f } );
		//mInputPatterns.Add( new float[] {  11.15f,    0.5f, -17.08f,    0.0f,    0.0f,    0.0f, -13.07f,    0.5f,  -9.57f,    0.0f,    0.0f,    0.0f } );
		//mInputPatterns.Add( new float[] {  13.45f,    0.5f, -19.80f,    0.0f,    0.0f,    0.0f,  13.52f,    0.5f,  -4.24f,    0.0f,    0.0f,    0.0f } );
		
		// Loop through test data
		//for( int i = 0; i < mInputPatterns.Count; i++ ) {
		//	Debug.Log( "START RUN -> " + i );
		//	Run( mInputPatterns[ i ] );
		//	Debug.Log( "END RUN -> " + i );
		//}

		// Move ART Spheres
		UpdateARTSpheres();
	}

	// Public Functions //

	// Run            //
	// Main ART2 Loop //
	public void Run( float[] inputVector ) {
		// Clear comparison vector
		ClearTVector();
		
		// Setting new input vector
		mXVector = inputVector;
		
		// ART cycle
		RecognitionPhase();
		ComparisonPhase();
		SearchPhase();
		HighLightTrained();
	}

	// ReturnExemplar               //
	// Returns the exemplar pattern //
	public float[] ReturnExemplar() {
		return mRecognitionField[ mBestNeuron ].GetExemplar();
	}

	// Private Functions //

	// Initialise                                   //
	// Initialises ART vectors at required length   //
	// Clears lists from previous runs              //
	// Adds single output node to recognition field //
	private void Initialise() {
		// Create ART vector arrays
		mXVector = new float[ mNumberOfInputNodes ];
		mCVector = new float[ mNumberOfInputNodes ];
		mTVector = new float[ mNumberOfInputNodes ];
		
		// Tidy lists
		//mInputPatterns.Clear();
		mRecognitionField.Clear();

		// Add new blank node
		AddNewRecognitionNode();
	}
	
	// Control1                                                          //
	// ART1 Control1 Signal                                              //
	// Returns '1' if the input vector (X) has a magnitude / is non-zero //
	// Network will not run with a zero input (pointless)                //
	private int Control1() {
		// Returns 1 if (X) vector has a magnitude (is non zero)->(Control1 signal)
		if( GetVectorMagnitude( mXVector ) > 0.0f ) {
			return 1;
		}
		
		return 0;
	}
	
	// ControlSignal                                             //
	// Functions as both ART control signals for two-thirds rule //
	private int ControlSignal() {
		// Returns '0' is mControl2 is true (ie and output node is currently active)
		if( mControl2 ) {
			return 0;
		}
		
		// Otherwise functions as Control1 signal
		return Control1();
	}
	
	// SetTVector                                                  //
	// Sets mTVector to match that of best match (Exemplar Vector) //
	private void SetTVector( float[] exemplar ) {
		mTVector = exemplar;
	}
	
	// ClearTVector                    //
	// Clears mTVector back to all 0's //
	private void ClearTVector() {
		mTVector = new float[ mNumberOfInputNodes ];
		for( int i = 0; i < mNumberOfInputNodes; i++ ) {
			mTVector[ i ] = 0.0f;
		}
	}
	
	// Comparison                                                                             //
	// No longer represents 2/3 rule - without dot product it could no longer function        //
	// Uses ControlSignal() to change functionality between Comparison and Recognition Phases //
	// Has been HACKED to detect when a 'blank' or un-trained node has been selected          //
	// Forces the blank nodes feedback/(T)/exemplar to equal the current input (X) vector     //
	// Therefore giving a 0 length comparison (C/Z) vector - passing vigilance                //
	private void Comparison() {
		// Comparison Phase //
		// Control2 should be flagged by an active output node
		if( ControlSignal() == 0 ) {
			//Debug.Log( "Comparison Phase" );
			// ***ART2 Dirty Hack!*** //
			// If an untrained node is selected as a match - guarentee a passed vigilance test by making comparison (Z) (difference vector) all 0's
			if( !mRecognitionField[ mBestNeuron ].IsTrained() ) {
				//Debug.Log( "Blank Node Selected -> " + mBestNeuron );
				for( int i = 0; i < mNumberOfInputNodes; i++ ) {
					mCVector[ i ] = mTVector[ i ];
				}
			// Create comparison vector (Z) as difference between (X) and (T) vectors
			} else {
				//Debug.Log( "Trained Node Selected -> " + mBestNeuron );
				for( int i = 0; i < mNumberOfInputNodes; i++ ) {
					// Comparison (Z) vector is difference between input (X) vector and passed exemplar (T) vector
					// (Z) = (X) - (T)
					mCVector[ i ] = ( mXVector[ i ] - mTVector[ i ] );
				}
			}
		// Recognition Phase //
		// Control1 should be flagged for a non-zero input vector
		// mTVector should currently be initialised to all zeros
		} else if( ( ControlSignal() == 1 ) && ( GetVectorMagnitude( mTVector ) == 0.0f ) ) {
			//Debug.Log( "Recognition Phase" );
			for( int i = 0; i < mNumberOfInputNodes; i++ ) {
				// Pass input (X) Vector for comparison
				mCVector[ i ] = mXVector[ i ];
			}
		// Error condition //
		} else {
			//Debug.Log( " Zero input vector detected!" );
		}
	}

	// VigilanceTest                                      //
	// Returns the magnitude of the comparison (Z) vector //
	private float VigilanceTest() {
		return GetVectorMagnitude( mCVector );
	}
	
	// Recognition             //
	// Looking for 'BestMatch' //
	private void Recognition() {
		mBestNeuron = -1; // reset index
		
		// Temp variables to determine best match
		float[] net    = new float[ mRecognitionField.Count ];
		float   netMax = -10000;
		
		// Loop for number of output/recognition nodes
		for( int i = 0; i < mRecognitionField.Count; i++ ) {
			// Get weights for each recognition node
			net[ i ] = mRecognitionField[ i ].GetWeighting( mXVector );
			//Debug.Log( "Getting Node -> " + i );
			//Debug.Log( "Weight = " + net[ i ] );
			// Looking for highest value (DOT product/weight)
			// Also making sure that this node has not previously been disabled by a failed vigilance test
			if( ( net[ i ] > netMax ) && ( !mRecognitionField[ i ].IsDisabled() ) ) {
				//Debug.Log( "New Best Neuron = " + i );
				mBestNeuron = i;        // as were looping through output nodes, this index represents the 'best match' or 'winning node'
				netMax      = net[ i ]; // new highest value or weight for above condition
			} else {
				//Debug.Log( "Something went wrong!" );
			}
		} 

		// Lateral Inhibition
		// If an output node has 'won', Control2 is now active
		if( mBestNeuron != -1 ) {
			mControl2 = true;
		}
	}
	
	// RecogntionPhase                                                      //
	// Comparing the comparison vector (W) against output/recognition field //
	private void RecognitionPhase() {
		mControl2 = false; // deactivate output / recognition field
		
		ClearTVector(); // reset comparison vector
		Comparison();   
		Recognition();
		SetTVector( mRecognitionField[ mBestNeuron ].GetExemplar() );
	}
	
	// ComparisonPhase                                                   //
	// Comparing the winning nodes exemplar against the input (X) vector //
	private void ComparisonPhase() {
		Comparison(); 
		VigilanceThresholdCheck();
	}
	
	// SearchPhase                                                                                                        //
	// Resonates comparison vector T (W and Z) between input and output (comparison and recognition) fields               //
	// Till the best match that passes a vigilance test is found                                                          //
	// Recognition field always contains a single 'blank' node that if selected as best match will pass a vigilance check //
	// This represents a new exemplar / class being created / learned                                                     //
	private void SearchPhase() {
		// Search Loop
		while( mReset ) {
			// Find a new winner with previous winners disabled
			ClearTVector();
			Comparison();
			Recognition();                    
			SetTVector( mRecognitionField[ mBestNeuron ].GetExemplar() );
			Comparison();
			VigilanceThresholdCheck();
		} 
		
		// If there is a best neuron
		if( mBestNeuron != -1 ) {
			Train();
		} else {
			// Failed to allocate a neuron for current pattern
			Debug.Log( "ERROR - Out of neurons in F2" );
		}
		
		// Clear nodes which have failed vigilance tests
		ClearDisabled();
	}

	// VigilanceThresholdCheck                                //
	// Tests the winning node against the Vigilance Threshold //
	// If it fails the node is blocked and a search begins    //
	// If it passes it is then trained                        //
	private void VigilanceThresholdCheck() {
		// Get magnitude of comparison vector
		float s = VigilanceTest();  
		//Debug.Log( "Neuron -> " + mBestNeuron );
		//Debug.Log( "Vigilance Check -> " + s );
		// Failed Vigilance
		if( s > mVigilanceThreshold ) {
			//Debug.Log( "Vigilance FAILED by node -> " + mBestNeuron );
			mReset    = true;
			mControl2 = false;
			mRecognitionField[ mBestNeuron ].SetDisabled( true );
		// Passed Vigilance
		} else {
			//Debug.Log( "Vigilance PASSED by node -> " + mBestNeuron );
			mReset = false;
		}
	}

	// ClearDisabled                                       //
	// Resetting disabled node list (after training/match) //
	private void ClearDisabled() {
		for( int i = 0; i < mRecognitionField.Count; i++ ) {
			mRecognitionField[ i ].SetDisabled( false );
		} 
	}
	
	// Train                                                                         //
	// Trains the best match node that passes a vigilance test                       //
	// Uses magnitudes of vectors for feddforward weight scaling                     //
	// Sets the nodes feed forward weights as normalised version on input (X) vector //
	// Sets the feed back weights to the input (X) vector as new exemplar            //
	private void Train() {
		// If winning node has not been previously trained
		if( !mRecognitionField[ mBestNeuron ].IsTrained() ) {
			//Debug.Log( "Training blank node -> " + mBestNeuron );
			// Use magnitude of input (X) vector for feedforward weight scaling
			mRecognitionField[ mBestNeuron ].Train( mXVector, GetVectorMagnitude( mXVector ) );

			// Add new blank node
			AddNewRecognitionNode();
		} else {
			//Debug.Log( "Re-Training node -> " + mBestNeuron );
			//Debug.Log( GetVectorMagnitude( mCVector ) );
			mRecognitionField[ mBestNeuron ].Train( mXVector, GetVectorMagnitude( mXVector ) );
			//mRecognitionField[ mBestNeuron ].Train( mXVector, GetVectorMagnitude( mCVector ) );
		}
	}

	// AddNewRecognitionNode                                                                     //
	// Creates a new 'blank' outnode, initialises it, then adds it to the recognition field list //
	private void AddNewRecognitionNode() {
		RealOutputNode blankNode = gameObject.AddComponent< RealOutputNode >();

		//RealOutputNode blankNode = new RealOutputNode();
		blankNode.Initialise( mNumberOfInputNodes, mARTPosition, mRecognitionField.Count );
		mRecognitionField.Add( blankNode );
	}

	// GetVectorMagnitude       //
	// Returns length of vector //
	private float GetVectorMagnitude( float[] vector ) {
		float magnitude = 0.0f;
		for( int i = 0; i < mNumberOfInputNodes; i++ ) {
			magnitude += ( vector[ i ] * vector[ i ] );
		}
		
		return Mathf.Sqrt( magnitude );
	}

	// HighlightTrained                        //
	// Highlights the winning exemplar vector //
	private void HighLightTrained() {
		for( int i = 0; i < mRecognitionField.Count; i++ ) {
			if( i == mBestNeuron ) {
				mRecognitionField[ i ].SetAsTrained( Color.green );
			} else {
				mRecognitionField[ i ].SetAsTrained( Color.black );
			}
		}
	}

	// UpdateARTSpheres                                                        //
	// Moves the ARTSpheres anchor based on dpad controls, stays within bounds //
	private void UpdateARTSpheres() {
		// Move ARTSpheres up
		if( Input.GetAxis( "Scroll_ART" ) > 0.0f ) {
			mARTX--;
		}
		// Move ARTSpheres down
		if( Input.GetAxis( "Scroll_ART" ) < 0.0f ) {
			mARTX++;
		}

		// Stop at upper limit
		if( mARTX > 0.0f ) {
			mARTX = 0.0f;
		}

		// Field limits
		if( mARTX < ( -mRecognitionField.Count + 1 ) * 2.0f ) {
			mARTX = ( -mRecognitionField.Count + 1 ) * 2.0f;
		}
		if( mARTX < ( -mRecognitionField.Count + 8 ) * 2.0f ) {
			mARTX = ( -mRecognitionField.Count + 8 ) * 2.0f;
		}

		// Update ARTSpheres positions
		for( int i = 0; i < mRecognitionField.Count; i++ ) {
			mRecognitionField[ i ].UpdateARTPosition( mARTX, i );
		}
	}

	// Debug Functions //

	// ***Functionality moved into RealOutputNode***
	// Debug                                         //
	// Visual Representation of ART2 Networks Output //
	/*void OnDrawGizmos() {
		for( int i = 0; i < mRecognitionField.Count; i++ ) {
			if( mRecognitionField[ i ].IsTrained() ) {
				// Get output nodes details
				float[] weight             = mRecognitionField[ i ].GetExemplar();
				float[] feedForwardWeights = mRecognitionField[ i ].GetFeedForwardWeights();
				for( int j = 0; j < mNumberOfInputNodes; j++ ) {
					if( weight[ j ] < 0.0f ) {
						Gizmos.color = Color.blue;
						Gizmos.DrawSphere( new Vector3( mARTPosition.x + ( i * 2.0f ), mARTPosition.y, mARTPosition.z + ( j * 2.0f ) ), feedForwardWeights[ j ] * -1.0f );
					} else if( weight[ j ] == 0 ) {
						Gizmos.color = Color.white;
						Gizmos.DrawSphere( new Vector3( mARTPosition.x + ( i * 2.0f ), mARTPosition.y, mARTPosition.z + ( j * 2.0f ) ), 0.5f );
					} else {
						Gizmos.color = Color.red;
						Gizmos.DrawSphere( new Vector3( mARTPosition.x + ( i * 2.0f ), mARTPosition.y, mARTPosition.z + ( j * 2.0f ) ), feedForwardWeights[ j ] * 1.0f );
					}
				}
			}
		}
	}*/
}
