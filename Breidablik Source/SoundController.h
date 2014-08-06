#ifndef _SOUNDCONTOLLER_H_
#define _SOUNDCONTOLLER_H_


#include <vector>
#include "audio.h"


/*
================================================
SoundController
Singleton to control the sound devices
Sets up Audio Devices DSP0 and DSP1
Loads in music and sound .raw files into vectors
Loops Music and plays a sound once
================================================
*/


/* Mask for singleton object */
#define SOUND SoundController::GetInstance()


class SoundController {
public:
	static SoundController* GetInstance();
	SoundController::~SoundController();
	
	void Init( void );
	void Update( void );
	void PlaySound( int soundIndex );
	void PlayMusic( int musicIndex );
	
private:
	SoundController::SoundController();
	static SoundController* instance;
	
	vector< SoundSample* > mMusic;
	vector< SoundSample* > mSounds;
	
	AudioDevice DSP0, DSP1;                            
};

#endif