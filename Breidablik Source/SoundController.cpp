#include "SoundController.h"
#include <stdio.h>


#ifndef NULL 
#define NULL 0
#endif


SoundController* SoundController::instance = NULL;


/*
===================================
Constructor
Sets up audio devices DSP0 and DSP1
===================================
*/
SoundController::SoundController() :
 DSP0( 0 ), DSP1( 1 ) {
}


/*
=======================================================================
GetInstance
Returns instance of SoundController - or creates it if doesnt exist yet
=======================================================================
*/
SoundController* SoundController::GetInstance() {
	if( instance == NULL ) {
		instance = new SoundController();
	}
	return instance;
}


/*
================================
Destructor
Tidyup vectors and sound devices
================================
*/
SoundController::~SoundController() {
	for( vector< SoundSample* >::iterator it = mSounds.begin(); it != mSounds.end(); it++ ) {
		delete *it;
	}
	mSounds.clear();
	for( vector< SoundSample* >::iterator it = mMusic.begin(); it != mMusic.end(); it++ ) {
		delete *it;
	}
	mMusic.clear();
	
	DSP0.Close();
	DSP1.Close();
}


/*
=====================================================================
Init
Loads in required music files for DSP0 and sfx into DSP1 into vectors
=====================================================================
*/
void SoundController::Init( void ) {
	/* Load Music */
	mMusic.push_back( new SoundSample( "IntroMusic", &DSP0 ) );    // 0 - Baulders Gate Theme
	mMusic.push_back( new SoundSample( "LevelMusic", &DSP0 ) );    // 1 - Level Music
	
	/* Load Sound Effects */
	mSounds.push_back( new SoundSample( "ItWillBeDone", &DSP1 ) ); // 0
	mSounds.push_back( new SoundSample( "WithEase", &DSP1 ) );     // 1
	mSounds.push_back( new SoundSample( "NotAProblem", &DSP1 ) );  // 2
	mSounds.push_back( new SoundSample( "Hmm", &DSP1 ) );          // 3
	mSounds.push_back( new SoundSample( "IStandReady", &DSP1 ) );  // 4
	mSounds.push_back( new SoundSample( "Yes", &DSP1 ) );          // 5
}


/*
=========================================
Update
HandleAudio must be called once per frame
=========================================
*/
void SoundController::Update( void ) {
	DSP0.HandleAudio();
	DSP1.HandleAudio();
}


/*
==========================
PlaySound
Plays a sound from mSounds
==========================
*/
void SoundController::PlaySound( int soundIndex ) {
	if( ( soundIndex > ( int( mSounds.size() ) ) ) || ( soundIndex < 0 ) ) {
		printf( "Illegal Access to mSounds vector! \n" );
		soundIndex = 0;
	}
	mSounds[ soundIndex ] -> Play();
}


/*
=================================
PlayMusic
Plays and loops music from mMusic
=================================
*/
void SoundController::PlayMusic( int musicIndex ) {	
	if( ( musicIndex > ( int( mMusic.size() ) ) ) || ( musicIndex < 0 ) ) {
		printf( "Illegal Access to Music vector! \n" );
		musicIndex = 0;
	}
	mMusic[ musicIndex ] -> PlayLoop();
}