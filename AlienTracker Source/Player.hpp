/*
	File:	            Player.hpp
	Version:	        1.0
	Date:	            21th April 2014
	Author:	            Chris McBain
	Based on Sound3D -  by Allan C. Milne

	Requires:	        XACore, XASound, Updateable
	
	Description:
	<insert description here>
*/

#ifndef __PLAYER__
#define __PLAYER__

// System includes //
#include <memory>
#include <string>
#include <X3DAudio.h>
#include <d3dx10math.h>
using namespace std;

// Framework includes //
#include "XACore.hpp"
#include "Updateable.hpp"
using AllanMilne::Audio::XACore;
using AllanMilne::Updateable;

// Forward declarations //
class XASound;

// Anonymous namespace for sound file constants
namespace {
	// Player class sound files
	const string trackerSound  = "Sounds/Tracker.wav";
	const string deathSound    = "Sounds/Death.wav";
	const string gameOverSound = "Sounds/GameOver.wav";

	// Kill sounds - to be stored in a vector
	const int numOfKillSounds = 5;
	const string killSounds[] = {
		"Sounds/Kill1.wave",
		"Sounds/Kill2.wave",
		"Sounds/Kill3.wave",
		"Sounds/Kill4.wave",
		"Sounds/Kill5.wave"
	};
}

// Player class //
class Player : public Updateable {
public:

	// Implementation of the Updateable interface //

	// When activated the sound should begin playing
	void OnUpdateActivation ();

	// When deactivated the sound should stop playing
	void OnUpdateDeactivation (); 

	// Called every frame to recalculate and set DSP settings for the sound position; deltaTime not used
	void OnUpdate( const float deltaTime );

	// Class specific behaviour

	// Player                                       //
	// Creates XASound object for supplied .wav file //
	Player( const string& aWavFile ); 
	
	// ~Player                                                           //
	// Stop sound playing; no deletion required since smart pointers used //
	virtual ~Player();
	
	// IsValid                                      //
	// Indicates if a valid object has been created //
	inline bool IsValid() const { 
		return mSound->IsValid(); 
	}

	// GetXASound                                                                         //
	// Returns reference to the XASound object encapsulating the underlying sound element //
	inline shared_ptr<XASound> GetXASound() const { 
		return mSound; 
	}

	// GetListener
	// Returns reference to the listener object
	inline X3DAUDIO_LISTENER GetListener() const {
		return mListener;
	}

	// Set3DPosition
	//
	void Set3DPosition( D3DXVECTOR3 position );

	// Set3DVelocity
	//
	void Set3DVelocity( D3DXVECTOR3 velocity );

private:
	
	shared_ptr<XASound> mSound; // The sound element to be played
	
	// Listener & vector3's //
	X3DAUDIO_LISTENER mListener;
	X3DAUDIO_VECTOR   mPosition;
	X3DAUDIO_VECTOR   mVelocity;
	X3DAUDIO_VECTOR   kUp;
    X3DAUDIO_VECTOR   mForward;

	// UpdateListener
	//
	void UpdateListener(); 

}; // end Player class

#endif