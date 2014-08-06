#ifndef _WINDOW_H_
#define _WINDOW_H_


// Includes //
#include <SFML/Graphics.hpp>


// Window                                          //
// Singleton to allow program access to the window //
class Window {
public:
	static Window* GetInstance();

	void Initialize( void );

	sf::RenderWindow mWindow; // the render window
	sf::Vector2i GetWindowDimensions( void );

private:
	Window( void );
	static Window* mInstance;

	sf::Vector2i mWindowDimensions;
};


#endif