#ifndef _MYPS2APPLICATION_H
#define _MYPS2APPLICATION_H


#include "primitives.h"
#include "font.h"
#include "Player.h"
#include "Enemy.h"
#include "Object.h"
#include "LevelData.h"
#include "ps2vector4.h"
#include "texture.h"
#include "Map.h"
#include "Menu.h"
#include "Cursor.h"
#include "XYCoords.h"
#include <queue>
#include <vector>


/*
========================================================================
Comparator
DisplayVector wont use regular operator overload as it contains pointers
========================================================================
*/
struct Comparator {
	/* Comparator for DisplayVector DepthSorting */
	bool operator () ( GameObject* G1, GameObject* G2) {
		return G1 -> GetmZ() > G2 -> GetmZ(); 
	}
};

	
/*
===========================================================================================
MyPS2Application - Main Game Class

Characters - Player Object / Vector of Enemy Objects
Objects - Vector of Objects
Map - 2D vector of <Tile>'s representing the map
Cursor - Users method of interaction 
Menu - Controls games options
LevelData - Container class for the games levels and <Tile> data
GameStateController - Controls main game state
GameTextureController - Handles all the games bitmaps
ScreenViewController - Takes all game variables and displays the current view of the game
SoundController - Handles the games audio
PathController - Holds the maps pathfinding array and all characters positions and movement
===========================================================================================
*/
class MyPS2Application {
public:
	MyPS2Application();
	
	void Run();
	
	/* Set Quitting Condition */
	static void set_quitting( bool quitting ) { 
		quitting_ = quitting; 
	}
	
	/* Quitting Condition */
	static bool quitting() { 
		return quitting_; 
	}
	
private:	
	/*
	=============
	PS2 Framework
	=============
	*/
	static bool quitting_;       // game loop boolean
	float LXAxisVal, LYAxisVal;  // left analogue stick positions
	float RXAxisVal, RYAxisVal;  // right analogue stick positions
	      
	/* MyPS2Application Functions */
	void Init();
	void CleanUp();
	void Update();
	void Render();
	void UpdatePad();
		
	/*
	=======================
	Main Game Functionality
	=======================
	*/ 
	
	Player* pPlayer;                             // players character
	vector< Enemy* > mEnemies;                   // levels enemies
	vector< Object* > mObjects;                  // level objects
	vector< PS2TexQuad* > mGameSprites;          // game sprites
	
	priority_queue< GameObject*,
                	vector< GameObject* >, 
					Comparator > mDisplayVector; // priority vector from depth sorting
	
	Menu mMenu;                                  // handles menu drawing and options
	LevelData mLevelData;                        // container for tile and map data
	Map* pCurrentMap;                            // holds and manages current maps data
	Cursor mCursor;                              // mouse (PS2 Controller) cursor
	int mResetTimer,  mResetTime;                // time before resetting and timer
	int mIntroMusic, mLevelMusic;                // music vector indices
	
	/* General Game Sprites */
	void CreateGameSprites( void );
	
	/* Menu Input Functions */
	void UpdateMenuInput( void );
	void UpdateInput( void );
	
	/* Win/Lose Condition Functions */
	void Victory( void );
	void YouDied( void );
	bool AllEnemiesDefeated( void );
	void ResetGame( void );
	
	/* Enable HardCoreMode */
	void HardCoreMode( void );
	
	/* Player Hits Enemy */
	void CheckPlayerHits( void );
	
	/* Enemy Functions */
	void SetupEnemies( void );
	void UpdateEnemies( XYCoords playerPosition );
	void CheckEnemyHits( void );
	
	/* GameObject Fucntions */
	void SetupObjects( void );
	void UpdateObjects( void );
	
	/* Object DepthSorting Functions */
	void UpdateDisplayVector( void );
	void RenderDisplayVector( void );
};

#endif 