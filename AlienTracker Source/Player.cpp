/*
	File:	            Player.hpp
	Version:	        1.0
	Date:	            21th April 2014
	Author:	            Chris McBain
	Based on Player -  by Allan C. Milne

	Requires:	        XACore, XASound, Updateable
	
	Description:
	<insert description here>
	
	This is the implementation of the Player class
	*** see Player.hpp for details ***
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

#include "Player.hpp"

// Implementation of the Updateable interface //

// OnUpdateActivation                            //
// When activated the sound should begin playing //
void Player::OnUpdateActivation() {
	mSound->Play();
} // end OnUpdateActivation function


// OnUpdateDeactivation                           //
// When deactivated the sound should stop playing //
void Player::OnUpdateDeactivation() {
	mSound->Stop();
} // end OnUpdateDeactivation function


// OnUpdate                                                                                          //
// Called every frame to recalculate and set DSP settings for the sound position; deltaTime not used //
void Player::OnUpdate( const float deltaTime ) {
	// TODO - Calculate 3D DSP settings and apply to mSound
	//XACore::GetInstance()->Apply3D( mSound->GetSourceVoice(), &mEmitter, &mListener, X3DAUDIO_CALCULATE_MATRIX );
} // end OnUpdate functionn


// Class specific behaviour //

// Player                                       //
// Creates XASound object for supplied .wav file //
Player::Player( const string& aWavFile ) 
: mPosition( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ), mVelocity( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ) {
	mSound = make_shared< XASound >( aWavFile, true ); // true = filters enabled
	mSound->SetLooped( true );
} // end Player constructor
	
	
// ~Player                                                            //
// Stops sound playing; no deletion required since smart pointers used //
Player::~Player() {
	mSound->Stop();
} // end Player destructor


// Set3DPosition             //
// Sets the position vector3 //
void Player::Set3DPosition( D3DXVECTOR3 position ) {
	mPosition = position;
} // end of Set3DPosition function


// Set3DVelocity             //
// Sets the velocity vector3 //
void Player::Set3DVelocity( D3DXVECTOR3 velocity ) {
	mVelocity = velocity;
} // end of Set3DVelocity function


// Setup3DListener //
void Player::UpdateListener() {
	mListener.Position    = mPosition;
	mListener.Velocity    = mVelocity;
	mListener.OrientFront = mForward;
	//mListener.OrientTop   = kUp;
} // end of Setup3DListener function

// end of code