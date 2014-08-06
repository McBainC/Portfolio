#include "MyPS2Application.h"
#include <sps2tags.h>
#include <sps2regstructs.h>
#include <signal.h>
#include "PS2Defines.h"
#include "pad.h"
#include "sps2wrap.h"
#include "dma.h"
#include "texture.h"
#include "ps2matrix4x4.h"
#include "GameTextureController.h"
#include "GameStateController.h"
#include "ScreenViewController.h"
#include "SoundController.h"
#include "PathController.h"
#include <stdio.h>


/* Quitting Flag */
bool MyPS2Application::quitting_ = false;


/* If somethings gone wrong... */
void sig_handle( int sig ) {
	static bool hasSeen = false;
	if( !hasSeen ) {
		hasSeen = true;
		printf( "signal!\n" );
	}
	MyPS2Application::set_quitting( true );
}


/*
===================
Default Constructor
===================
*/
MyPS2Application::MyPS2Application() {
}


/*
==================================================================
Run
Initialises the game, then updates then renders while not quitting
==================================================================
*/
void MyPS2Application::Run() {
	Init();
	while( !quitting_ ) {
		Update();
		Render();
	}
	CleanUp();
}


/*
=======================================================
Init
Initialises the PS2 pad, allocates graphics memory,
initialises game singletons and creates all gameobjects
=======================================================
*/
void MyPS2Application::Init() {
	/* Initialise control pad 0 */
	if( !pad_init( PAD_0, PAD_INIT_LOCK | PAD_INIT_ANALOGUE | PAD_INIT_PRESSURE ) ) {
		printf( "Failed to initialise control pad\n" );
		pad_cleanup( PAD_0 );
		exit( 0 );
	}	
	
	/* Enable pad0's actuators */
	enable_actuator( 0, 1, 1 ); 
	
	/* Allocate memory for the graphics data */
	SPS2Manager.Initialise( 5120 );	  // 5120 * 4K Pages = 20MB Total
	VIFStaticDMA.Initialise( 2560 );  // 2560 * 4K Pages = 10MB Static DMA
	VIFDynamicDMA.Initialise( 1280 ); // 1280 * 4K Pages * 2 Buffers =
									  // 10MB Dynamic DMA
																	
	/* Register our signal function for every possible signal (e.g. ctrl + c) */
	for( int sig=0; sig<128; sig++ ) {
		signal( sig, sig_handle );
	}

	/* Define the clear screen colour - Black */
	SPS2Manager.InitScreenClear( 0, 0, 0 );
	
	/* Create instance of Controller Singletons and initialise them */
	GAMESTATE -> Init();
	GAMETEXTURES -> Init(); 
	SCREENVIEW -> Init();
	PATHFINDER -> Init();
	
	/* Setup Level One */
	pCurrentMap = new Map( LEVEL_ONE_WIDTH, LEVEL_ONE_HEIGHT ); 
	pCurrentMap -> Load2DVectors( mLevelData.mLevelOneMap );
	PATHFINDER -> LoadPathfindingArray( mLevelData.mLevelOneArray ); 
		
	/* Setup Sound */
	SOUND -> Init();
	SOUND -> PlayMusic( mIntroMusic );
	
	/* Setup GameObjects */
	pPlayer = new Player( 0.0f, 0.0f, 1000.0f );
	SetupEnemies();
	SetupObjects();
	CreateGameSprites();
	
	/* Initialise Variables */
	mResetTimer = 0;
    mResetTime = 200;
	mIntroMusic = 0;
	mLevelMusic = 1;
}


/*
=============================
CleanUp
Tidyup pointers and new usage
=============================
*/
void MyPS2Application::CleanUp() {
	if( pCurrentMap ) {
		delete pCurrentMap;
		pCurrentMap = NULL;
	}
	if( pPlayer ) {
		delete pPlayer;
		pPlayer = NULL;
	}
	
	/* Vectors of pointers */
	for( vector< PS2TexQuad* >::iterator it = mGameSprites.begin(); it != mGameSprites.end(); it++ ) {
		delete *it;
	}
	mGameSprites.clear();
	for( vector< Enemy* >::iterator it = mEnemies.begin(); it != mEnemies.end(); it++ ) {
		delete *it;
	}
	mEnemies.clear();
	for( vector< Object* >::iterator it = mObjects.begin(); it != mObjects.end(); it++ ) {
		delete *it;
	}
	mObjects.clear();
	
	/* Priority Queue of pointers */
	while( !mDisplayVector.empty() ) {
		delete mDisplayVector.top();
		mDisplayVector.pop();
	}
	
	/* PS2 Stuff */
	pad_cleanup( PAD_0 );
	set_actuator( 0, 0, 0 ); 
	SPS2Manager.CleanUp();
}


/*
======
Update
======
*/
void MyPS2Application::Update() {
	/* Tell DMAC to send previous graphics data to Graphics Synthesiser (GS) */
	VIFDynamicDMA.Fire();
	
	/* Always update sound */
	SOUND -> Update();
	
	/* Update Pad0 */
	pad_update( PAD_0 ); // read the control pad into data buffer
	UpdatePad();         // update analogue stick variables
	
	/* Menu Screen */
	if( GAMESTATE -> CheckGameState( GameStateController::MENU_SCREEN ) ) {
		/* Update user input */
		UpdateMenuInput();
		
		/* Start Game Condition */
		if( mMenu.IsRunning() ) {
			GAMESTATE -> ChangeGameState( GameStateController::RUNNING );
			SOUND -> PlayMusic( mLevelMusic );
			
			/* HardCoreMode Condition */
			if( mMenu.IsHardCoreMode() ) {
				HardCoreMode();
			}
		}
		
		/* Quitting Condition */
		if( mMenu.IsQuitting() ) {
			GAMESTATE -> ChangeGameState( GameStateController::QUITTING );
			quitting_ = true;
		}
	}
	
	/* Game Running */
	if( GAMESTATE -> CheckGameState( GameStateController::RUNNING ) ) {
		/* Main Game Update Calls */
		
		/* Update ScreenViewController */
		SCREENVIEW -> Update( RXAxisVal, RYAxisVal );
		
		/* Update Player and hits */
		pPlayer -> Update();
		CheckPlayerHits();
		
		/* Update Enemies and hits */
		UpdateEnemies( pPlayer -> GetArrayXYCoords() ); 
		CheckEnemyHits();
		
		/* UpdateObjects as SCREENVIEW moves */
		UpdateObjects();
		
		/* Fill vector in correct order for rendering */
		UpdateDisplayVector();
		
		/* Update Cursor */
		mCursor.Update( LXAxisVal, LYAxisVal ); 
		
		/* Update user input */
		UpdateInput();
	}
	
	/* Check for exit condition */
	if( ( pad[ 0 ].buttons & PAD_START ) && ( pad[ 0 ].buttons & PAD_SELECT ) ) {
		quitting_ = true;
	}
}


/*
======
Render
======
*/
void MyPS2Application::Render() {
	/* All drawing commands should be between BeginScene and EndScene */
	SPS2Manager.BeginScene();
	
	/* Menu Screen */
	if( GAMESTATE -> CheckGameState( GameStateController::MENU_SCREEN ) ) {
		mMenu.Render();
	}
	
	/* Game Running */
	if( GAMESTATE -> CheckGameState( GameStateController::RUNNING ) ) {
		/* Main Game Render Calls */
		
		/* Render WorldMap */
		SCREENVIEW -> Draw( pCurrentMap -> mWorldMap );
		
		/* Render GameObjects */
		RenderDisplayVector();
		
		/* Render Cursor */
		mCursor.Render();
		
		/* If grid option is choosen */
		if( mMenu.DisplayingGrid() ) {
			/* Render Pathfinding Grid */
			SCREENVIEW -> DrawGrid( pCurrentMap -> mWorldMap, PATHFINDER -> mPathfindingArray );
		}
		
		/* Death / Victory Conditions */
		if( pPlayer -> CheckCharacterState( Character::DIEING ) ) {
			YouDied();
			mResetTimer++;
		} else if( AllEnemiesDefeated() ) {
			Victory();
			mResetTimer++;
		}
		
		/* Reset Condition */
		if( mResetTimer > mResetTime ) {
			GAMESTATE -> ChangeGameState( GameStateController::MENU_SCREEN );
			SOUND -> PlayMusic( mIntroMusic );
			ResetGame();
		}
	}
	
	SPS2Manager.EndScene();	
}


/*
====================================
UpdatePad
Updates pad analogue stick positions
====================================
*/
void MyPS2Application::UpdatePad() {
	LXAxisVal = pad[ 0 ].axes[ PAD_AXIS_LX ]; // pad0 left analogue stick X position
	LYAxisVal = pad[ 0 ].axes[ PAD_AXIS_LY ]; // pad0 left analogue stick Y position
	RXAxisVal = pad[ 0 ].axes[ PAD_AXIS_RX ]; // pad0 right analogue stick X position
	RYAxisVal = pad[ 0 ].axes[ PAD_AXIS_RY ]; // pad0 right analogue stick Y position
}


/*
============================================= 
CreateGameSprites
Creates sprites not linked to the map or grid
=============================================
*/
void MyPS2Application::CreateGameSprites( void ){
	mGameSprites.push_back( new PS2TexQuad( 0.0f, 0.0f, 1002.0f, 256.0f, 64.0f, 0, 32, 256, 64 ) ); // YOU DIED
	mGameSprites.push_back( new PS2TexQuad( 0.0f, 0.0f, 1002.0f, 256.0f, 64.0f, 0, 96, 256, 64 ) ); // VICTORY
}


/*
==========================
UpdateMenuInput
Updates mMenu by pad input
==========================
*/
void MyPS2Application::UpdateMenuInput( void ) {
	/* Move up */
	if( pad[ 0 ].pressed & PAD_UP ) {
		mMenu.MoveUp();
	}
	/* Move down */
	if( pad[ 0 ].pressed & PAD_DOWN ) {
		mMenu.MoveDown();
	}
	/* Select option */
	if( pad[ 0 ].pressed & PAD_CROSS ) {
		mMenu.Select();
	}
	mMenu.Update();
}


/*
===============
UpdateInput
Player Controls
===============
*/
void MyPS2Application::UpdateInput( void ) {
	/* Move Player */
	if( pad[ 0 ].pressed & PAD_CROSS ) {
		pPlayer -> SetTarget( mCursor.GetArrayXYCoords() ); // set players target
		SOUND -> PlaySound( rand() % 3 );                   // play random sfx (0 - 2)
	}
	/* Heal Player */
	if( pad[ 0 ].pressed & PAD_SQUARE ) {
		pPlayer -> ChangeCharacterState( Character::ACTION ); // set players next action
		SOUND -> PlaySound( ( rand() % 3 ) + 3 );             // play random sfx (3 - 5)
	}
}


/*
====================
Victory/YouDied
Win/Death Conditions
====================
*/
void MyPS2Application::Victory( void ) {
	/* Render "VICTORY" */
	if( mGameSprites[ 1 ] ) {
		GAMETEXTURES -> SelectTexture( 91 );
		mGameSprites[ 1 ] -> Render();
	}
}
void MyPS2Application::YouDied( void ) {
	/* Render "YOU DIED" */
	if( mGameSprites[ 0 ] ) {
		GAMETEXTURES -> SelectTexture( 91 );
		mGameSprites[ 0 ] -> Render();
	}
}


/*
==============================
AllEnemiesDefeated
Checks if all enemies are dead
==============================
*/
bool MyPS2Application::AllEnemiesDefeated( void ) {
	int numOfDefeated = 0;
	/* Check through mEnemies to see how many are dead */
	for( int i = 0; i < int( mEnemies.size() ); i++ ) {
		if( mEnemies[ i ] -> CheckCharacterState( Character::DIEING ) ) {
			numOfDefeated++;
		}
	}
	/* If it equals the size of mEnemies -> all enemies are defeated! */
	if( numOfDefeated == int( mEnemies.size() ) ) {
		return true; 
	}
	return false;
}


/* 
===================================
ResetGame
Resets Player, Enemies and the Menu
===================================
*/
void MyPS2Application::ResetGame( void ) {
	/* Cleanup previous positions */
	PATHFINDER -> ClearObjectArray();

	/* Delete then recreate Player */
	if( pPlayer ) {
		delete pPlayer;
		pPlayer = new Player( 0.0f, 0.0f, 1000.0f );
	}
	
	/* Delete Enemies Vector */
	for( vector< Enemy* >::iterator it = mEnemies.begin(); it != mEnemies.end(); it++ ) {
		delete *it;
	}
	mEnemies.clear();
	
	/* Recreate Enemies */
	SetupEnemies();
	
	/* Reset Menu */
	mMenu.SetRunning( false );
			
	/* Reset Cursor and Timer */		
	mCursor.Reset();
	mResetTimer = 0;
}


/*
============================
HardCoreMode
Adds a new enemy if selected
============================
*/
void MyPS2Application::HardCoreMode( void ) {
	mEnemies.push_back( new Enemy( 192.0f, -64.0f, 1000.0f ) );
}


/*
==========================================
CheckPlayerHits
Check if player has hit any of the enemies
==========================================
*/
void MyPS2Application::CheckPlayerHits( void ) {
	/* If the player is at teh correct attack animation frame */
	if( pPlayer -> CheckIfDealingDamage() ) {
		for( int i = 0; i < int( mEnemies.size() ); i++ ) {
			/* If the players target is the enemies position */
			if( pPlayer -> GetTargetXYCoords() == mEnemies[ i ] -> GetArrayXYCoords() ) {
				mEnemies[ i ] -> TakeDamage( pPlayer -> WeaponDamage() );
			}
		}
	}
}


/*
================================================================
SetupEnemies
Creates the level enemies and pushes them to the mEnemies vector
================================================================
*/
void MyPS2Application::SetupEnemies( void ) {
	mEnemies.push_back( new Enemy( 192.0f, 32.0f, 1000.0f ) );
	mEnemies.push_back( new Enemy( -192.0f, 0.0f, 1000.0f ) );
}


/*
====================================================================
UpdateEnemies
Updates all the enemies and checks if the player has moved too close
====================================================================
*/
void MyPS2Application::UpdateEnemies( XYCoords playerPosition ) {
	for( int i = 0; i < int( mEnemies.size() ); i++ ) {
		mEnemies[ i ] -> CheckAggro( playerPosition );
		mEnemies[ i ] -> Update();
	}
}


/*
===============================================
CheckEnemyHits
Check if any of the enemies have hit the player
===============================================
*/
void MyPS2Application::CheckEnemyHits( void ) {
	for( int i = 0; i < int( mEnemies.size() ); i++ ) {
		/* If the enemy is in the correct attack animation frame */
		if( mEnemies[ i ] -> CheckIfDealingDamage() ) {
			/* If the enemies target is the players position */
			if( mEnemies[ i ] -> GetTargetXYCoords() == pPlayer -> GetArrayXYCoords() ) {
				pPlayer -> TakeDamage( mEnemies[ i ] -> WeaponDamage() );
			}
		}
	}
}


/*
=================================================================
SetupObjects
Sets the levels object sprites and pushes them to mObjects vector
=================================================================
*/
void MyPS2Application::SetupObjects( void ) {
	mObjects.push_back( new Object( 0.0f, 32.0f, 1000.0f, 64.0f, 64.0f, 0, 0, 64, 64, 32.0f, 10 ) );          // bonfire
	mObjects.push_back( new Object( -256.0f, 64.0f, 1000.0f, 128.0f, 192.0f, 0, 64, 128, 192, 64.0f, 10 ) );  // tree1
	mObjects.push_back( new Object( 256.0f, 0.0f, 1000.0f, 128.0f, 192.0f, 128, 64, 128, 192, 64.0f, 10 ) );  // tree2
	mObjects.push_back( new Object( 196.0f, -160.0f, 1000.0f, 128.0f, 192.0f, 0, 64, 128, 192, 64.0f, 9 ) );  // tree3 
	mObjects.push_back( new Object( 64.0f, -128.0f, 1000.0f, 128.0f, 192.0f, 128, 64, 128, 192, 64.0f, 9 ) ); // tree4
}


/*
============================================================
UpdateObjects
Updates all the objects positions relative to the ScreenView
============================================================
*/
void MyPS2Application::UpdateObjects( void ) {
	for( int i = 0; i < int( mObjects.size() ); i++ ) {
		mObjects[ i ] -> Update();
	}
} 


/*
====================================================================
UpdateDisplayVector
Update with all the games sprites to be sorted and rendered by depth
====================================================================
*/
void MyPS2Application::UpdateDisplayVector( void ) {
	/* Add Player */
	mDisplayVector.push( pPlayer );
	
	/* Add Enemies */
	for( int i = 0; i < int( mEnemies.size() ); i++ ) {
		mDisplayVector.push( mEnemies[ i ] );
	}
	
	/* Add Objects */
	for( int i = 0; i < int( mObjects.size() ); i++ ) {
		mDisplayVector.push( mObjects[ i ] );
	}
}


/* 
===================================================
RenderDisplayVector
Renders the priority queue which is sorted by depth
===================================================
*/
void MyPS2Application::RenderDisplayVector( void ) {
	while( !mDisplayVector.empty() ) {
		if( mDisplayVector.top() ) {
			mDisplayVector.top() -> Render(); // render top
			mDisplayVector.pop();             // pop it
		}
	}
}