#pragma once


/*
==================================================
Input 
Class to deal with keyboard and mouse interactions
==================================================
*/
class Input {
public:

	int mMouseX, mMouseY;                                     // mouse coords
	bool mKeys[256];                                          // main keyboard bool array
	bool Q,W,E,R,T,Y,U,I,O,P,A,S,D,F,G,H,J,K,L,Z,X,C,V,B,N,M; // keyboard letter shortcut bools

	void Update(); 
};
