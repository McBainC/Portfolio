#include "MyNetworkApplication.h"
#include "Window.h"


// Constructor            //
// Setup the RenderWindow //
MyNetworkApplication::MyNetworkApplication( void ) {
	Window::GetInstance()->Initialize();
	mPlayerHasFired = false;
}


// Run                            //
// Initializes then runs the game //
void MyNetworkApplication::Run( void ) {
	Initialize();

	// Main Game Loop
	while( Window::GetInstance()->mWindow.isOpen() ) {
		Events();
		Update();
		Render();
	}

	Window::GetInstance()->mWindow.close();
}


// Initialize                                //
// Sets up the games sprite and text objects //
void MyNetworkApplication::Initialize( void ) {
	// Create background texture
	if( !mBackgroundTexture.loadFromFile( "Background.png" ) ) {
		std::cout << "Error loading BackgroundTexture" << std::endl;
	}
	mBackgroundSprite.setTexture( mBackgroundTexture );

	// Load in font
	mDestructoBeamFont.loadFromFile( "DestructoBeamFontReg.ttf" );

	// Game title text setup
	mChallengeOfTheGrid.setFont( mDestructoBeamFont );
	mChallengeOfTheGrid.setCharacterSize( 60 );
	mChallengeOfTheGrid.setString( "Challenge of the Grid!") ;
	mChallengeOfTheGrid.setColor( sf::Color::White );
	mChallengeOfTheGrid.setPosition( 150.0f, -5.0f );

	// Into text setup
	mPrepareForBattle.setFont( mDestructoBeamFont );
	mPrepareForBattle.setCharacterSize( 50 );
	mPrepareForBattle.setString( "Prepare for Battle!" );
	mPrepareForBattle.setColor( sf::Color::Red );
	mPrepareForBattle.setPosition( 200.0f, 512.0f - 25.0f );

	// Player score text setup
	mPlayerScore.setFont( mDestructoBeamFont );
	mPlayerScore.setCharacterSize( 40 );
	mPlayerScore.setColor( sf::Color::Cyan );
	mPlayerScore.setPosition( 20.0f, 1024.0f - 120.0f );

	// Challenger score text setup
	mChallengerScore.setFont( mDestructoBeamFont );
	mChallengerScore.setCharacterSize( 40 );
	mChallengerScore.setColor( sf::Color( 255, 255, 0, 255 ) );
	mChallengerScore.setPosition( 20.0f, 1024.0f - 60.0f );

	// Set player & challenger positions
	mPlayer.SetPosition( sf::Vector2f( 512.0f, 512.0f ) );
	mChallenger.SetPosition( sf::Vector2f( 512.0f, 512.0f ) );

	// Set object bounds
	mPlayer.SetBounds( Window::GetInstance()->GetWindowDimensions() );
	mChallenger.SetBounds( Window::GetInstance()->GetWindowDimensions() );
}


// Events                                                                       //
// Checks all the windows events triggered since the last iteration of the loop //
void MyNetworkApplication::Events( void ) {
	sf::Event event;
	while ( Window::GetInstance()->mWindow.pollEvent( event ) ) {
		switch( event.type ) {
			// Window being closed
			case sf::Event::Closed: {
				Window::GetInstance()->mWindow.close();
				break;
			}
				
			// Text being entered
			case sf::Event::TextEntered: {
				/*
				// Handle ASCII characters only
				if( ( event.text.unicode < 128 ) && ( event.text.unicode != '\b' ) ) {
					//std::cout << "ASCII character typed: " << static_cast<char>( event.text.unicode ) << std::endl;
					str += static_cast< char >( event.text.unicode );
					mPlayerAddress.setString( str );
				} 
				// Handle 'Backspace' events
				if( event.text.unicode == '\b' ) {
					if( !str.empty() ) {
						str.erase( str.length() - 1, 1 );
						mPlayerAddress.setString( str );
					}
				}
				*/
			}

			// Left Mouse Click
			case sf::Event::MouseButtonPressed: {
				// If the game is running - FIRE!
				if( mGameController.CheckGameState( GameController::RUNNING ) ) {
					if( event.mouseButton.button == sf::Mouse::Left ) {
						mPlayer.Fire();
						mPlayerHasFired = true;
					}
				}
			}
		}
	}
}


// Update                                                                 //
// Checks the states of the Network & GameControllers to update correctly //
void MyNetworkApplication::Update( void ) {
	// Restart returns the value of mClock before resetting
	sf::Time elapsedTime = mClock.restart();
	mTimeSinceLastUpdate += elapsedTime;

	// GAME INITIALIZATION //
	// Proceed to WAITING_TO_START once NetworkController is ready
	if( mNetworkController.CheckNetworkState( NetworkController::CONNECTING ) ) {
		mGameController.ChangeGameState( GameController::WAITING_TO_START );
	}

	// NETWORK CONNECTION ESTABLISHED //
	// Proceed to RUNNING once NetworkController is ACTIVE
	if( mNetworkController.CheckNetworkState( NetworkController::ACTIVE ) ) {
		mGameController.ChangeGameState( GameController::RUNNING );
	}

	// GAME RUNNING //
	if( mGameController.CheckGameState( GameController::RUNNING ) ) {
		// Always check for new network data
		mNetworkController.Receive();

		// Update Player & Challenger
		UpdatePlayer();
		UpdateChallenger();
	}

	// GAME QUITTING //
	// Exit condition
	if( sf::Keyboard::isKeyPressed( sf::Keyboard::Escape ) ) {
		Window::GetInstance()->mWindow.close();
	}
}


// Render                                            //
// Renders based on the current GameController state //
void MyNetworkApplication::Render( void ) {
	// Clear the window (with black)
	Window::GetInstance()->mWindow.clear( sf::Color::Black );

	// WAITING TO START //
	// Render only the background and title 
	if( mGameController.CheckGameState( GameController::WAITING_TO_START ) ) {
		Window::GetInstance()->mWindow.draw( mBackgroundSprite );
		Window::GetInstance()->mWindow.draw( mChallengeOfTheGrid );
	}

	// RUNNING //
	// Render everything!
	if( mGameController.CheckGameState( GameController::RUNNING ) ) {
		// Render background first
		Window::GetInstance()->mWindow.draw( mBackgroundSprite );
		
		// Render challenger & bullets second
		mChallenger.Render();

		// Render player and bullets third
		mPlayer.Render();

		// Render text last
		Window::GetInstance()->mWindow.draw( mChallengeOfTheGrid );
		Window::GetInstance()->mWindow.draw( mPlayerScore );
		Window::GetInstance()->mWindow.draw( mChallengerScore );
	}

	// Render current frame
	Window::GetInstance()->mWindow.display();
}


// UpdatePlayer //
void MyNetworkApplication::UpdatePlayer( void ) {
	// Update Player
	mPlayer.Update();
	mPlayer.ChangeScore( 0 );
	mPlayerScore.setString( mPlayer.GetScoreString() );

	// Send updated position every 10th of a second
	if( mTimeSinceLastUpdate > mTimePerFrame ) {
		// Set playerObject data struct
		PlayerObjectData serverData;
		serverData.pX = mPlayer.GetPosition().x;
		serverData.pY = mPlayer.GetPosition().y;
		serverData.dX = mPlayer.GetDirection().x;
		serverData.dY = mPlayer.GetDirection().y;
		serverData.angle = mPlayer.mTrajectory;

		// If player has fired
		if( mPlayerHasFired ) {
			// Send additional bullet flag
			mNetworkController.CreateServerPacket( serverData, 1 );
			mPlayerHasFired = false;
		} else {
			// Send only player data
			mNetworkController.CreateServerPacket( serverData, 0 );
		}

		// Update to server
		mNetworkController.Send();

		// Reduce time
		mTimeSinceLastUpdate -= mTimePerFrame;
	}
}


// UpdateChallenger                                                      //
// Checks if new data has been received and send it on to the challenger //
void MyNetworkApplication::UpdateChallenger( void ) {
	// If new data has been received this loop - send it to the challenger
	if( mNetworkController.PacketReceived() ) {
		
		mChallenger.NewDataReceived( mNetworkController.ReturnChallengerData() );
		// If the challenger has fired 
		if( mNetworkController.ChallengerHasFired() ) {
			// Spawn a new challenger bullet 
			mChallenger.Fire();
		}

		// Reset 
		mNetworkController.Reset();
	}

	// Always update & score
	mChallenger.Update();
	mChallenger.ChangeScore( 0 );
	mChallengerScore.setString( mChallenger.GetScoreString() );
}

