#include "GameStateController.h"


#ifndef NULL 
#define NULL 0
#endif


GameStateController* GameStateController::instance = NULL;


/*
===========
Constructor
===========
*/
GameStateController::GameStateController() {
}


/*
===========================================================================
GetInstance
Returns instance of GameStateController - or creates it if doesnt exist yet
===========================================================================
*/
GameStateController* GameStateController::GetInstance() {
	if( instance == NULL ) {
		instance = new GameStateController();
	}
	return instance;
}


/*
==========================
Init
Starts at the Start Screen
==========================
*/
void GameStateController::Init( void ) {
	mState = MENU_SCREEN;
}


/*
=================================
CheckGameState
Confirms current state externally
=================================
*/
bool GameStateController::CheckGameState( GameState testState ) {
	if( mState == testState ) {
		return true;
	}
	return false;
}


/*
=====================
ChangeGameState
Changes current state
=====================
*/
void GameStateController::ChangeGameState( GameState newState ) {
	mState = newState;
}