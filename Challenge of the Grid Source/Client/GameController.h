#ifndef _GAMECONTROLLER_H_
#define _GAMECONTROLLER_H_


// GameController                                  //
// Controls flow of the Game through the Main Loop //
class GameController {
public:
	GameController( void );
	~GameController( void );

	void Initialize( void );

	enum GameState {
		NETWORK_SETUP,
		WAITING_TO_START,
		RUNNING,
		QUITTING
	};

	// External State Functions
	bool CheckGameState( GameState testState );
	void ChangeGameState( GameState newState );

private:
	GameState mGameState; // current GameState
};


#endif