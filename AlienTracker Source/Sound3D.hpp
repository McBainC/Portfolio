/*
	File:	            Sound3D.hpp
	Version:	        1.0
	Date:	            19th April 2014
	Original Author:	Allan C. Milne
	Contributor:	    Chris McBain
	
	Requires:	        XACore, XASound, Updateable
	
	Description:
	<insert description here>
*/

#ifndef __SOUND3D__
#define __SOUND3D__

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

class Sound3D : public Updateable {
public:

	// Implementation of the Updateable interface //

	// When activated the sound should begin playing
	void OnUpdateActivation ();

	// When deactivated the sound should stop playing
	void OnUpdateDeactivation (); 

	// Called every frame to recalculate and set DSP settings for the sound position; deltaTime not used
	void OnUpdate( const float deltaTime );

	// Class specific behaviour

	// Sound3D                                       //
	// Creates XASound object for supplied .wav file //
	Sound3D( const string& aWavFile ); 
	
	// ~Sound3D                                                           //
	// Stop sound playing; no deletion required since smart pointers used //
	virtual ~Sound3D();
	
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

	// SetListener
	//
	//public void SetListener( 

	// Set3DPosition
	//
	void Set3DPosition( D3DXVECTOR3 position );

	// Set3DVelocity
	//
	void Set3DVelocity( D3DXVECTOR3 velocity );

private:
	
	shared_ptr<XASound> mSound; // The sound element to be played
	
	// Emitter //
	X3DAUDIO_EMITTER mEmitter;
	X3DAUDIO_VECTOR  mPosition;
	X3DAUDIO_VECTOR  mVelocity;

	void Setup3DEmitter();
	void Update3DEmitter();

	/*
	// DSP //
	X3DAUDIO_DSP_SETTINGS mDSPSettings;

	void SetupDSPSettings();
	*/

	// Listener //
	X3DAUDIO_LISTENER mListener;

}; // end Sound3D class

#endif