#region Includes
// Unity Includes //
using UnityEngine;
using System.Collections;
using System.Collections.Generic; // ART1 only
#endregion

#region Details
// ----------------------------------------------------------------------- //
// Name:    BinaryOutputNode                                               //
// Author:  Chris McBain                                                   //
// Created:                                                                //
// Purpose:                                                                //
// - Represents an ART1 Network output node                                //
// - Must be initialised with the number of input/comparison nodes         //
// - Contains functionality to return its exemplar, weighting and train it //
// ----------------------------------------------------------------------- //
#endregion

// BinaryOutputNode Class //
public class BinaryOutputNode {
	// Constants //
	private const double L = 2.0f; // ART network feed forward weight scaling const

	// Private Variables //
	private double[] mFeedForwardWeights; // BottomUp weights / (W)Vector
	private int[]    mFeedBackWeights;    // TopDown weights / (T)Vector     
	private int      mInputLength;        // Number of Input/Comparison nodes
	private bool     mDisabled = false;   // Flag for failing a vigilance test
	private bool     mTrained  = false;   // Flag for recieving an Exemplar vector

	// Public Functions //

	// Initialise                                                //
	// Creates weight arrays at ART networks passed input length //
	// Sets initial feedforward and feedback weights
	public void Initialise( int inputLength ) {
		mInputLength        = inputLength;
		mFeedForwardWeights = new double[ mInputLength ];
		mFeedBackWeights    = new int[ mInputLength ];

		InitialiseWeights();
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

	// GetFeedForwardWeights                               //
	// Returns this output nodes feedforward weights array //
	public double[] GetFeedForwardWeights() {
		return mFeedForwardWeights;
	}

	// GetExemplar                               //
	// Returns this output nodes exemplar vector //
	public int[] GetExemplar() {
		return mFeedBackWeights;
	}

	// GetWeighting                                                 //               
	// Returns dot product of feedforward and input ( W.X ) vectors //
	public double GetWeighting( int[] xVector ) {
		double weighting = 0.0;

		// Dot product
		for( int i = 0; i < mInputLength; i++ ) {
			weighting += mFeedForwardWeights[ i ] * xVector[ i ];
		}

		return weighting;
	}

	// Train                                                                  //
	// FeedForword weights recalculated with zVectors number of active 'bits' //
	// FeedBack weights now match the zVector - the new Exemplar              // 
	public void Train( int[] zVector, int numberOfActiveBits ) {
		for( int i = 0; i < mInputLength; i++ ) {
			mFeedForwardWeights[ i ] = L * zVector[ i ] / ( L - 1 + numberOfActiveBits );
			mFeedBackWeights[ i ]    = zVector[ i ];
		}

		// Flag as trained
		SetTrained( true );
	}

	// Private Functions //

	// InitialiseWeights                                                                              //
	// FeedBack weights = 1, FeedForward weights is dependant on the number of Input/Comparison nodes //
	private void InitialiseWeights() {
		// FeedForward weight scaling
		double feedForwardWeight = L / ( L - 1 + mInputLength );

		// Initialise weights
		for( int i = 0; i < mInputLength; i++ ) {
			mFeedForwardWeights[ i ] = feedForwardWeight;
			mFeedBackWeights[ i ]    = 1;
		}
	}
}

#region Includes
// Unity Includes //
//using UnityEngine;
//using System.Collections;
//using System.Collections.Generic;
#endregion

#region Details
// ------------------------------------------------------------------------------------- //
// Name:    ART1 Class                                                                   //
// Author:  Chris McBain                                                                 //
// Created:                                                                              //
// Purpose:                                                                              //
// - Represents a simplified version of an ART1 (binary) network                         //
// - Details based around the biological/neural elements have been reworked              //
// - Requires the length of the input vector (mNumberOfInputNodes)                       //
// -    "     the vigilance threshold value to initialise                                //
// - Currently neither can be changed without destroying and recreating the network      //
// - Output/Recognition Field structure expands as required (List) - ***NO LIMITS SET*** //
// ------------------------------------------------------------------------------------- //
#endregion

// ART1 Class //
public class ART1 : MonoBehaviour {
	// Unity Editor Variables //
	[ SerializeField ]                         int    mNumberOfInputNodes = 0;   // number of input/comparison nodes
	[ Range( 0.0f, 1.0f ) ] [ SerializeField ] double mVigilanceThreshold = 0.0; // vigilance threshold setting

	// Private Variables //
	// ART Vectors //
	private int[] mXVector; // input (X) vector
	private int[] mCVector; // input (W/Z) comparison vector
	private int[] mTVector; // output (T) comparison vector / exemplar 

	private int   mBestNeuron = -1;    // index for 'best match' output node in mRecognitionField list
	private bool  mControl2   = false; // control2 signal flag
	private bool  mReset      = false; // search flag

	private List< BinaryOutputNode > mRecognitionField = new List< BinaryOutputNode > (); // output/recognition node list (field)

	private List< int[] > mInputPatterns = new List< int[] >(); // test list to hold input data

	// Unity Functions //

	// Update                                                                //
	// Test function rebuilds network each frame                             //
	// Allows impact of changing vigilance threshold to be seen in real-time //
	void Update() {
		Initialise();

		// Add test data in vector int[] format
		mInputPatterns.Add( new int[] { 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1 } );
		mInputPatterns.Add( new int[] { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 } );
		mInputPatterns.Add( new int[] { 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1 } );
		mInputPatterns.Add( new int[] { 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0 } );
		mInputPatterns.Add( new int[] { 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1 } );
		mInputPatterns.Add( new int[] { 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1 } );

		// Loop through test data
		for( int i = 0; i < mInputPatterns.Count; i++ ) {
			Run( mInputPatterns[ i ] );
		}
	}

	// Public Functions //

	// Initialise                                   //
	// Initialises ART1 vectors at required length  //
	// Clears lists from previous runs              //
	// Adds single output node to recognition field //
	public void Initialise() {
		// Create ART vector arrays
		mXVector = new int[ mNumberOfInputNodes ];
		mCVector = new int[ mNumberOfInputNodes ];
		mTVector = new int[ mNumberOfInputNodes ];

		// Tidy lists
		mInputPatterns.Clear();
		mRecognitionField.Clear();

		// Create and initialise one outputnode and add to recognition field
		BinaryOutputNode blankNode = new BinaryOutputNode();
		blankNode.Initialise( mNumberOfInputNodes );
		mRecognitionField.Add ( blankNode );
	}

	// Run           //
	// Main ART Loop //
	public void Run( int[] inputVector ) {
		// Clear comparison vector
		ClearTVector();
		
		// Setting new input vector
		mXVector = inputVector;
		
		// ART cycle
		RecognitionPhase();
		ComparisonPhase();
		SearchPhase();
	}

	// Private Functions //

	// Control1                                                     //
	// ART1 Control1 Signal                                         //
	// Returns '1' if any entry in the input vector (x) is non-zero //
	// Network will not run with a zero input                       //
	private int Control1() {
		// Returns 1 if any node in the input (X) vector is active (Control1 signal)
		for( int i = 0; i < mNumberOfInputNodes; i++ ) {
			if( mXVector[ i ] == 1 ) {
				return 1;
			}
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
	private void SetTVector( int[] exemplar ) {
		mTVector = exemplar;
	}

	// ClearTVector                    //
	// Clears mTVector back to all 0's //
	private void ClearTVector() {
		mTVector = new int[ mNumberOfInputNodes ];
		for( int i = 0; i < mNumberOfInputNodes; i++ ) {
			mTVector[ i ] = 0;
		}
	}

	// Comparison                                                                                               //
	// Function to cover both 2/3 rules for passing input to recognition and creating the comparison (Z) vector //
	private void Comparison() {
		int thirds; // third counter
		
		for( int i = 0; i < mNumberOfInputNodes; i++ ) {
			//       > Input <       > Control1/2 <  > T/Comparison <
			thirds = mXVector[ i ] + ControlSignal() + mTVector[ i ];
			
			// If 2/3 are active - activate that bit in comparison/z vector
			if( thirds >= 2 ) {
				mCVector[ i ] = 1;
			} else {
				mCVector[ i ] = 0;
			}
		} 
	}

	// VigilanceTest                                   //
	// Adds 'bits' of vectors to get comparison ratio  //
	// DOT product of (X) and (Z) X.Z = Vigilance Test //
	private double VigilanceTest() {
		double comparison, input;
		comparison = input = 0.0;
		
		// Get magnitudes of comparison and input vectors
		for( int i = 0; i < mNumberOfInputNodes; i++ ) {
			comparison += mCVector[ i ];
			input      += mXVector[ i ];
		} 
		
		return ( comparison / input );
	}

	// Recognition             //
	// Looking for 'BestMatch' //
	private void Recognition() {
		mBestNeuron = -1; // reset index

		// Temp variables to determine best match
		double[] net    = new double[ mRecognitionField.Count ];
		double   netMax = -1;

		// Loop for number of output/recognition nodes
		for( int i = 0; i < mRecognitionField.Count; i++ ) {
			// Get weights for each recognition node
			net[ i ] = mRecognitionField[ i ].GetWeighting( mXVector );
			// Looking for highest value (DOT product/weight)
			// Also making sure that this node has not previously been disabled by a failed vigilance test
			if( ( net[ i ] > netMax ) && ( !mRecognitionField[ i ].IsDisabled() ) ) {
				mBestNeuron = i;        // as were looping through output nodes, this index represents the 'best match' or 'winning node'
				netMax      = net[ i ]; // new highest value or weight for above condition
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
		Comparison();   // first 2/3 rule - passing a non-zero input to the output 
		Recognition();
		SetTVector( mRecognitionField[ mBestNeuron ].GetExemplar() );
	}
	
	// ComparisonPhase                                                   //
	// Comparing the winning nodes exemplar against the input (X) vector //
	private void ComparisonPhase() {
		Comparison(); // second 2/3 rule - setting mCVector as the comparison (Z) vector

		// Disabling neuron upon failed vigilance test
		double s = VigilanceTest();                              
		if( s < mVigilanceThreshold ) {
			mReset    = true;
			mControl2 = false;
			mRecognitionField[ mBestNeuron ].SetDisabled( true );
		} else {
			mReset = false;
		}
	}
	
	// SearchPhase                                                                                                        //
	// Resonates comparison vector T (W and Z) between input and output (comparison and recognition) fields               //
	// Till the best match that passes a vigilance test is found                                                          //
	// Recognition field always contains a single 'blank' node that if selected as best match will pass a vigilance check //
	// This represents a new exemplar / class being created / learned                                                     //
	private void SearchPhase() {
		double s;
		while( mReset ) {
			// Find a new winner with previous winners disabled
			ClearTVector();
			Comparison();
			Recognition();                    
			SetTVector( mRecognitionField[ mBestNeuron ].GetExemplar() );

			// Disabling neuron upon failed vigilance test
			s = VigilanceTest();              
			if( s < mVigilanceThreshold ){
				mReset    = true;                
				mControl2 = false;
				mRecognitionField[ mBestNeuron ].SetDisabled( true );
			} else {
				mReset = false;               
			}
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

	// ClearDisabled                                       //
	// Resetting disabled node list (after training/match) //
	private void ClearDisabled() {
		for( int i = 0; i < mRecognitionField.Count; i++ ) {
			mRecognitionField[ i ].SetDisabled( false );
		} 
	}

	// Train                                                                        //
	// Trains the best match node that passes a vigilance test                      //
	// Required to calculate the number of active bits in the comparison (Z) vector //
	// Sets the nodes feed forward weights using this number                        //
	// Sets the feed back weights to the comparison (Z) vector as new exemplar      //
	private void Train() {
		// Get number of active bits in comparison (Z) vector to rescale winning nodes feedforward weights
		int numberOfActiveBits = 0; // counter for number of active 'bits' in comparison vector
		
		// Looping through number of bits in input (X) vector
		for( int i = 0; i < mNumberOfInputNodes; i++ ) {
			numberOfActiveBits += mCVector[ i ]; // sum of number of active bits
		}

		// Train winning output node
		mRecognitionField[ mBestNeuron ].Train( mCVector, numberOfActiveBits );

		// Create and initialise one outputnode and add to recognition field
		BinaryOutputNode blankNode = new BinaryOutputNode();
		blankNode.Initialise( mNumberOfInputNodes );
		mRecognitionField.Add( blankNode );
	}
	
	// Debug Functions //

	// Debug                                        //
	// Visual Representation of ART Networks Output //
	void OnDrawGizmos() {
		Vector3 position = transform.position; 
		for( int i = 0; i < mRecognitionField.Count; i++ ) {
			if( mRecognitionField[ i ].IsTrained() ) {
				// Get output nodes details
				int[]    weight             = mRecognitionField[ i ].GetExemplar();
				double[] feedForwardWeights = mRecognitionField[ i ].GetFeedForwardWeights();
				for( int j = 0; j < mNumberOfInputNodes; j++ ) {
					if( weight[ j ] == 0 ) {
						Gizmos.color = Color.white;
						Gizmos.DrawSphere( new Vector3( position.x + i, position.y, position.z + j ), ( float )feedForwardWeights[ j ] + 0.1f );
					} else {
						Gizmos.color = Color.red;
						Gizmos.DrawSphere( new Vector3( position.x + i, position.y, position.z + j ), ( float )feedForwardWeights[ j ] );
					}
				}
			}
		}
	}
}
