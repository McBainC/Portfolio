#ifndef _GAMESTATECONTOLLER_H_
#define _GAMESTATECONTOLLER_H_


/*
======================================
GameStateController
Singleton to control the GameState
Used by PS2Application to control Game
======================================
*/


/* Mask for singleton object */
#define GAMESTATE GameStateController::GetInstance()


class GameStateController {
public:
	static GameStateController* GetInstance();
	
	/* Game State */
	enum GameState {
		LOADING_SCREEN,
		MENU_SCREEN,
		CHARACTER_CREATION,
		RUNNING,
		PAUSED,
		QUITTING
	};
	
	void Init( void );
	
	/* External State Functions */
	bool CheckGameState( GameState testState );
	void ChangeGameState( GameState newState );
	
private:
	GameStateController::GameStateController();
	static GameStateController* instance;
	
	GameState mState; // current GameState
};

#endif