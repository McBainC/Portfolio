#include "Input.h"


/*
============================================
Update 
If a key is pressed sets its boolean to true
============================================
*/
void Input::Update() {
	if( mKeys[ 0x41 ] ) { A = true; } else { A = false; }
	if( mKeys[ 0x42 ] ) { B = true; } else { B = false; }
	if( mKeys[ 0x43 ] ) { C = true; } else { C = false; }
	if( mKeys[ 0x44 ] ) { D = true; } else { D = false; }
	if( mKeys[ 0x45 ] ) { E = true; } else { E = false; }
	if( mKeys[ 0x46 ] ) { F = true; } else { F = false; }
	if( mKeys[ 0x47 ] ) { G = true; } else { G = false; }
	if( mKeys[ 0x48 ] ) { H = true; } else { H = false; }
	if( mKeys[ 0x49 ] ) { I = true; } else { I = false; }
	if( mKeys[ 0x4A ] ) { J = true; } else { J = false; }
	if( mKeys[ 0x4B ] ) { K = true; } else { K = false; }
	if( mKeys[ 0x4C ] ) { L = true; } else { L = false; }
	if( mKeys[ 0x4D ] ) { M = true; } else { M = false; }
	if( mKeys[ 0x4E ] ) { N = true; } else { N = false; }
	if( mKeys[ 0x4F ] ) { O = true; } else { O = false; }
	if( mKeys[ 0x50 ] ) { P = true; } else { P = false; }
	if( mKeys[ 0x51 ] ) { Q = true; } else { Q = false; }
	if( mKeys[ 0x52 ] ) { R = true; } else { R = false; }
	if( mKeys[ 0x53 ] ) { S = true; } else { S = false; }
	if( mKeys[ 0x54 ] ) { T = true; } else { T = false; }
	if( mKeys[ 0x55 ] ) { U = true; } else { U = false; }
	if( mKeys[ 0x56 ] ) { V = true; } else { V = false; }
	if( mKeys[ 0x57 ] ) { W = true; } else { W = false; }
	if( mKeys[ 0x58 ] ) { X = true; } else { X = false; }
	if( mKeys[ 0x59 ] ) { Y = true; } else { Y = false; }
	if( mKeys[ 0x5A ] ) { Z = true; } else { Z = false; }
}