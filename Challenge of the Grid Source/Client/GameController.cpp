#include "GameController.h"


// Constructor //
GameController::GameController( void ) {
	Initialize();
}


// Destructor //
GameController::~GameController( void ) {
}


// Initialize                              //
// Sets initial GameState to NETWORK_SETUP //
void GameController::Initialize( void ) {
	mGameState = NETWORK_SETUP;
}


// CheckGameState                        //
// Confrims current GameState externally //
bool GameController::CheckGameState( GameState testState ) {
	if( mGameState == testState ) {
		return true;
	}

	return false;
}


// ChangeGameState           //
// Changes current GameState //
void GameController::ChangeGameState( GameState newState ) {
	mGameState = newState;
}

