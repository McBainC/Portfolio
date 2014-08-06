#ifndef _MENU_H_
#define _MENU_H_


#include "primitives.h"


/*
==================================================
Menu
Class to control game options and switch gamestate
Holds menu text, sprites and functionality 
==================================================
*/
class Menu {
public:
	Menu::Menu();
	Menu::~Menu();
	
	/* Main/Options Menu */
	enum MenuStatus {
		MAIN,
		SETTINGS
	};
	
	/* Menu State */
	enum MenuState {
		PLAY,
		OPTIONS,
		QUIT,
		DISPLAY_GRID,
		HARDCOREMODE,
		RETURN
	};
	
	void Update( void );
	void Render( void );
	
	/* External Setter */
	void Select( void );
	void SetRunning( bool newBool );
	
	/* External Menu Cursor Movement */
	void MoveUp( void );
	void MoveDown( void );
	
	/* External Mode Checks */
	bool DisplayingGrid( void );
	bool IsHardCoreMode( void );
	bool IsRunning( void );
	bool IsQuitting( void );
	
private:
	float mMenuX, mMenuY;             // menu icons positions
	bool mDisplayGrid, mHardCoreMode; // game setting flags
	bool mRunning, mQuitting;         // game state flags
	
	PS2TexQuad* pMenuSkull;           // menu skull symbol
	PS2TexQuad* pMenuCursor;          // menu cursor's sprite
	
	MenuStatus mMenuStatus;
	MenuState mState;
};

#endif