/*
	File:	            Sound3D.cpp
	Version:	        1.0
	Date:	            19th April 2014
	Original Author:	Allan C. Milne
	Contributor:	    Chris McBain

	Requires:	XASound, Updateable.
	
	Description:
	<insert description here>
	
	This is the implementation of the Sound3D class
	*** see Sound3D.hpp for details ***
*/

// System includes //
#include <memory>
#include <string>
//using namespace std;

// Framework includes //
#include "XACore.hpp"
using namespace AllanMilne::Audio;
#include "XASound.hpp"
#include "Updateable.hpp"
using AllanMilne::Audio::XASound;
using AllanMilne::Updateable;

#include "Sound3D.hpp"

// Implementation of the Updateable interface //

// OnUpdateActivation                            //
// When activated the sound should begin playing //
void Sound3D::OnUpdateActivation() {
	mSound->Play();
} // end OnUpdateActivation function


// OnUpdateDeactivation                           //
// When deactivated the sound should stop playing //
void Sound3D::OnUpdateDeactivation() {
	mSound->Stop();
} // end OnUpdateDeactivation function


// OnUpdate                                                                                          //
// Called every frame to recalculate and set DSP settings for the sound position; deltaTime not used //
void Sound3D::OnUpdate( const float deltaTime ) {
	// TODO - Calculate 3D DSP settings and apply to mSound
	XACore::GetInstance()->Apply3D( mSound->GetSourceVoice(), &mEmitter, &mListener, X3DAUDIO_CALCULATE_MATRIX );
} // end OnUpdate functionn


// Class specific behaviour //

// Sound3D                                       //
// Creates XASound object for supplied .wav file //
Sound3D::Sound3D( const string& aWavFile ) 
: mPosition( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ), mVelocity( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ) {
	mSound = make_shared< XASound >( aWavFile, true ); // true = filters enabled
	mSound->SetLooped( true );
} // end Sound3D constructor
	
	
// ~Sound3D                                                            //
// Stops sound playing; no deletion required since smart pointers used //
Sound3D::~Sound3D() {
	mSound->Stop();
} // end Sound3D destructor


// Set3DPosition             //
// Sets the position vector3 //
void Sound3D::Set3DPosition( D3DXVECTOR3 position ) {
	mPosition = position;
} // end of Set3DPosition function


// Set3DVelocity             //
// Sets the velocity vector3 //
void Sound3D::Set3DVelocity( D3DXVECTOR3 velocity ) {
	mVelocity = velocity;
} // end of Set3DVelocity function


// Setup3DEmitter //
void Sound3D::Setup3DEmitter() {
	// Voice details struct
	XAUDIO2_VOICE_DETAILS details;
	
	// Get source voice details
	IXAudio2SourceVoice* source = mSound->GetSourceVoice();

	// Set details struct
	source->GetVoiceDetails( &details );

	// Set emitters channel count from details
	mEmitter.ChannelCount        = details.InputChannels;
	mEmitter.CurveDistanceScaler = 1.0f;
	mEmitter.Position = mPosition;
	mEmitter.Velocity = mVelocity;
} // end of Setup3DEmitter function


// Update3DEmitter                            //
// Updates the emitters position and velocity //
void Sound3D::Update3DEmitter() {
	mEmitter.Position = mPosition;
	mEmitter.Velocity = mVelocity;
} // end of Update3DEmitter function


/*
// SetupDSPSettings //
void Sound3D::SetupDSPSettings() {
	SecureZeroMemory( &mDSPSettings, sizeof( X3DAUDIO_DSP_SETTINGS ) );

	mDSPSettings.SrcChannelCount     = mEmitter.ChannelCount;
	mDSPSettings.DstChannelCount     = XACore::GetInstance()->GetChannelCount();
	mDSPSettings.pMatrixCoefficients = new FLOAT32[ mDSPSettings.SrcChannelCount * mDSPSettings.DstChannelCount];

} // end of SetupDSPSettings function
*/

// end of code