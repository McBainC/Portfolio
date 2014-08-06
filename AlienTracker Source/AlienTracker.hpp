/*
	File:	 AlienTracker.hpp
	Version: 1.0
	Date:	 11th March 2014
	Author:	 Chris McBain
	
	Uses:	 IState, XACore, XASound, Updateable

	Description:
	<insert description here>
*/

#ifndef __ALIENTRACKER_HPP__
#define __ALIENTRACKER_HPP__

// System Includes //
#include <vector>
#include <xaudio2.h>

// Framework includes //
#include "IState.hpp"
#include "XACore.hpp"
#include "XASound.hpp"
#include "Updateable.hpp"
using AllanMilne::IState;
using AllanMilne::Audio::XACore;
using AllanMilne::Audio::XASound;
using AllanMilne::Updateable;

// AlienTracker                   //
// Inherits from IState interface //
class AlienTracker : public IState {
public:

	// Implementation of the IState interface //

	// Initialize                                               //
	// Creates and initializes the required sound to be played  //
	// Since this game has no graphics the argument is not used //
	bool Initialize( HWND aWindow );


	// Setup                                                                         //
	// Called at the start of processing; displays instructions and plays the sounds //
	bool Setup();


	// ProcessFrame                                                                      //
	// Processes a single application frame                                              //
	// Checks for key pressess, perform the corresponding changes to the bird frame time //
	// This will call the relevant Update() functions                                    //
	bool ProcessFrame( const float deltaTime );


	// Cleanup                                                                   //
	// Stops the sounds playing and reset bird frame time                        //
	// Sets the clean up flag to true                                            //
	// This will be called by the destructor if not previously called explicitly //
	void Cleanup();


	// Application specific behaviour //

	// AlienTracker        //
	// Default constructor //
	AlienTracker() 
	: mCleanedUp( false ), mTracker( NULL ) { 
	} // end constructor


	// ~AlienTracker                                 //
	// Deletes all resources allocated by Initialize //
	virtual ~AlienTracker();

private:

	bool mCleanedUp; // flag to indicate whether resources have already been cleaned up

	XASound *mTracker; // constant tracker sound

	std::vector< Updateable* > mUpdateableSounds; // list of all updateable sounds
}; // end AlienTracker class

#endif