#include "Window.h"


Window* Window::mInstance = NULL;


// Constructor //
Window::Window( void ) {
}


// GetInstance                                                        //
// Returns instance of the window - or creates it if doesnt exist yet //
Window* Window::GetInstance( void ) {
	if( mInstance == NULL ) {
		mInstance = new Window();
	}

	return mInstance;
}


// Initialize //
void Window::Initialize( void ) {
	mWindowDimensions.x = mWindowDimensions.y = 1024;
	mWindow.create( sf::VideoMode( mWindowDimensions.x, mWindowDimensions.y ), "Networking Coursework" );
	mWindow.setVerticalSyncEnabled( true );
}


// GetWindowDimensions //
sf::Vector2i Window::GetWindowDimensions( void ) {
	return mWindowDimensions;
}

