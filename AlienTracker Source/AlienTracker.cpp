/*
	File:	         AlienTracker.cpp
	Version:         1.0
	Date:	         11th March 2014
	Original Author: Allan C. Milne
	Contributor:     Chris McBain

	Uses:	         IState, XACore, XASound, Updateable

	Description:
	<insert description here>
*/

// System includes //
#include <windows.h>

// Framework includes
#include "XACore.hpp"
using namespace AllanMilne::Audio;
#include "XASound.hpp"
using AllanMilne::Audio::XASound;

//#include "Bird.hpp"

// Application includes //
#include "AlienTracker.hpp"


// Anonymous namespace defining constants and helper functions used in the implementation //
namespace {

	// Display instructions on using this soundscape 
	void Instructions () {
		MessageBox ( NULL, 
			         TEXT( "up/down = increase/decrease frame time for rendering the bird sound. \nD = display current bird frame time. \nI = display these instructions. \nescape = exit application. " ), 
			         TEXT( " Updateable and XASound Behaviour. " ), 
			         MB_OK | MB_ICONINFORMATION  | MB_SETFOREGROUND );
	} // end Instructions function

} // end anonymous namespace


// Implementation of the IState interface //

// Initialize                                                    //
// Will create and initialize the sounds to be played            //
// Since this game has no graphics the HWND argument is not used //
bool AlienTracker::Initialize( HWND aWindow ) {
	// Guard against uninitialized or invalid XACore
	if( XACore::GetStatus() != XACore::OK ) { 
		return false;
	}

	// Create mTracker XASound
	mTracker = new XASound( "Sounds/Tracker.wav" );

	// Check its been created correctly
	if( mTracker->GetSourceVoice() == NULL ) {
		MessageBox( NULL, "Error loading Rain.wav", TEXT ( "Initialize() - FAILED" ), MB_OK | MB_ICONERROR );
		delete mTracker;
		mTracker = NULL;
		return false;
	}

	// Set mTracker to loop
	mTracker->SetLooped( true );

	/*
	// Create a bird object
	Bird* newBird = new Bird();
	if( newBird->IsOk() ) {
		mUpdateableSounds.push_back( newBird );
	} else {
		MessageBox( NULL, "Error creating bird", TEXT ( "Initialize() - FAILED" ), MB_OK | MB_ICONERROR );
		delete newBird;
		newBird = NULL;
		return false;
	}
	*/

	return true; // everything initialized ok
} // end Initialize function
	

// Setup                                                                         //
// Called at the start of processing; displays instructions and plays the sounds //
bool AlienTracker::Setup () {
Instructions();
	// Play the rain sounds and activate the updating
	mTracker->Play();	

	// Loop through mUpdateableSounds - ActivateUpdate
	for( int i = 0; i < ( int )mUpdateableSounds.size(); i++ ) {
		mUpdateableSounds[ i ]->ActivateUpdate();
	}

	return true; // All has been setup without error
} // end Setup function


// ProcessFrame                                        //
// Process a single application frame                  //
// Check for key pressess, perform appropriate actions //
bool AlienTracker::ProcessFrame( const float deltaTime ) {
	float newTime = deltaTime;

	// Check if keys were pressed since last key state call - least significant bit
	if( GetAsyncKeyState( VK_UP ) & 0x0001 ) {
		newTime += 0.01f; // increase bird sound frame time 
	}
	if(GetAsyncKeyState( VK_DOWN ) & 0x0001 ) {
		newTime -= 0.01f; // decrease bird sound frame time 
	}
	if( GetAsyncKeyState( 'D' ) & 0x0001 ) {
		MessageBox( NULL, "Current bird frame time", TEXT ( "newTime" ), MB_OK ); // display current bird sound frame time setting
	}
	if( GetAsyncKeyState( 'I' ) & 0x0001 ) {
		Instructions();
	}

	// Loop through mUpdateableSounds - Update
	for( int i = 0; i < ( int )mUpdateableSounds.size(); i++ ) {
		mUpdateableSounds[ i ]->Update( deltaTime );
	}

	return true;
} // end ProcessFrame function


// CleanUp                                                                   //
// Will stop the sounds playing and reset any relevant attributes            //
// Sets the clean up flag to true                                            //
// This will be called by the destructor if not previously called explicitly //
void AlienTracker::Cleanup () {
	// Stop the rain
	mTracker->Stop();

	// Loop through mUpdateableSounds - DeactivateUpdate
	for( int i = 0; i < ( int )mUpdateableSounds.size(); i++ ) {
		mUpdateableSounds[ i ]->DeactivateUpdate();
	}

	// Flag as cleaned up
	mCleanedUp = true;
} // end Cleanup function


// Implementation of application specific functions //

// ~AlienTracker                                                   // 
// Releases all the resources allocated by the Initialize function //
AlienTracker::~AlienTracker() {
	// If not cleaned up
	if( !mCleanedUp ) {
		Cleanup();
	}

	if( mTracker != NULL ) {
		delete mTracker;
		mTracker = NULL;
	}

	// Loop through and free memory
	for( int i = 0; i < ( int )mUpdateableSounds.size(); i++ ) {
		delete mUpdateableSounds[ i ];
	}

	// Clear container
	mUpdateableSounds.clear();

} // end AlienTracker destructor function

// end of code
