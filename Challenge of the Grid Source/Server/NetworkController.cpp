#include "NetworkController.h"


#include <iostream> // cout


// << overload //
sf::Packet& operator <<( sf::Packet& packet, const PlayerObjectData& playerObjectData ) {
	return packet << playerObjectData.pX << playerObjectData.pY << playerObjectData.dX << playerObjectData.dY << playerObjectData.angle;
}
// >> overload //
sf::Packet& operator >>( sf::Packet& packet, PlayerObjectData& playerObjectData ) {
	return packet >> playerObjectData.pX >> playerObjectData.pY >> playerObjectData.dX >> playerObjectData.dY >> playerObjectData.angle;
}


// << overload //
sf::Packet& operator <<( sf::Packet& packet, const GameObjectData& gameObjectData ) {
    return packet << gameObjectData.pX << gameObjectData.pY << gameObjectData.dX << gameObjectData.dY;
}
// >> overload //
sf::Packet& operator >>( sf::Packet& packet, GameObjectData& gameObjectData ) {
    return packet >> gameObjectData.pX >> gameObjectData.pY >> gameObjectData.dX >> gameObjectData.dY;
}


// Constructor                 //
// Initializes known variables //
NetworkController::NetworkController( void ) :
 mNetworkState( INITIALIZING ) {
	Initialize();
}


// Destructor    //
// Clear vectors //
NetworkController::~NetworkController( void ) {
	Reset();
}


// Initialize                                                                     //
// Sets local IpAddress and attempts to bind UdpSocket and set it to non-blocking //
void NetworkController::Initialize( void ) {
	Reset();

	// Initialize client connection status flags
	mClientConnected[ 0 ] = 0;
	mClientConnected[ 1 ] = 0;

	// Set local address
	mLocal.address = sf::IpAddress::getLocalAddress();

	// Set local listening port number
	mLocal.port = 44443;

	// Set UpdSocket to Non-Blocking
	mUdpSocket.setBlocking( false );

	// Bind Upd Socket
	if( mUdpSocket.bind( mLocal.port ) != sf::Socket::Done ) {
		std::cout << "Failed to bind UdpSocket to port: " << mLocal.port << "!" << std::endl;
	} else {
		std::cout << "Socket bound to port: " << mLocal.port << std::endl;

		// Change network state
		mNetworkState = WAITING_ON_CLIENTS;
	}
}


// SetClientInfo                                            //
// Sets clients info struct with an address and port number //
void NetworkController::SetClientInfo( int clientIndex, std::string address, unsigned short port ) {
	// Set the string as IpAddress
	mClient[ clientIndex ].address = address;

	// Check its valid
	if( mClient[ clientIndex ].address == sf::IpAddress::None ) {
		std::cout << "Client1 Address: " << mClient[ clientIndex ].address << " - is invalid!" << std::endl;
	}

	// Set the port
	mClient[ clientIndex ].port = port;
}


// CheckNetworkState               //
// External check for NetworkState //
bool NetworkController::CheckNetworkState( NetworkState testState ) {
	if( mNetworkState == testState ) {
		return true;
	}

	return false;
}


// PacketRecevied //
bool NetworkController::PacketReceived( int clientNumber ) {
	if( mClientPacketProcessed[ clientNumber ] ) {
		return true;
	}

	return false;
}


// CreateClientPacket //
void NetworkController::CreateClientPacket( PlayerObjectData playerData, sf::Uint8 hasFired ) {
	// Clear the outbound packet
	mOutboundPacket.clear();

	// Add players gameObject struct and hasFired int(bool)
	mOutboundPacket << playerData << hasFired;
}


// Send                                                   //
// Calls SendToClient - to pass a reference to the packet //
void NetworkController::Send( int clientIndex ) {
	SendToClient( mClient[ clientIndex ], mOutboundPacket );
}


// SendToClient //
void NetworkController::SendToClient( ConnectionInfo clientInfo, sf::Packet& packet ) {
	// Attempt to send packet to the server
	if( mUdpSocket.send( packet, clientInfo.address, clientInfo.port ) != sf::Socket::Done ) {
		std::cout << "Failed to send packet to: " << clientInfo.address.toString() 
			      << " Port: " << clientInfo.port << "!" << std::endl;
	} else {
		std::cout << "Packet sent to: " << clientInfo.address.toString() 
			      << " Port: " << clientInfo.port << "!" << std::endl;
	}
}


// Receive       //
// Reference fix //
void NetworkController::Receive( void ) {
	ReceiveFromClient( mInboundPacket );
}


// ReceiveFromClient                                             //
// Receives incoming packets - and sets the correct local copies //
void NetworkController::ReceiveFromClient( sf::Packet& packet ) {
	// Accept all available packets
	while( mUdpSocket.receive( packet, mIncoming.address, mIncoming.port ) == sf::Socket::Done ) {
		// If waiting for both clients to be connected
		if( mNetworkState == WAITING_ON_CLIENTS ) {
			// Check and assign clientInfo and connection status for new client
			if( !mClientConnected[ 0 ] ) {
				mClient[ 0 ] = mIncoming;
				mClientConnected[ 0 ] = 1;
				std::cout << "Client 0 has connected!" << std::endl;
			// If one client has been set and this is from a different client
			} else if ( !mClientConnected[ 1 ] && mIncoming != mClient[ 0 ] ) {
				mClient[ 1 ] = mIncoming;
				mClientConnected[ 1 ] = 1;
				std::cout << "Client 1 has connected!" << std::endl;
				mNetworkState = ACTIVE;
			}
		// If connection is active receive packet
		} else if( mNetworkState == ACTIVE ) {
			// Client 0
			if( mIncoming == mClient[ 0 ] ) {
				UnpackPacket( 0 );
			// Client 1
			} else if( mIncoming == mClient[ 1 ] ) {
				UnpackPacket( 1 );
			}

			// Clear the packet
			mInboundPacket.clear();
		}
	}
}


// UnpackPacket                                       //
// Unpacks the inboundPacket into the specified index //
void NetworkController::UnpackPacket( int clientIndex ) {
	// Unpack client object data and hasFired bool
	mInboundPacket >> mClientData[ clientIndex ] >> mClientHasFired[ clientIndex ];

	// Flag that a packet has been processed
	mClientPacketProcessed[ clientIndex ] = true;
}


// Reset                                             //
// Resets all the packet variables for the next loop //
void NetworkController::Reset( void ) {
	// Reset packet recieved flags
	mClientPacketProcessed[ 0 ] = false;
	mClientPacketProcessed[ 1 ] = false;
}


// ReturnClientData                          //
// Returns the specified indices client data //
PlayerObjectData NetworkController::ReturnClientData( int clientIndex ) {
	return mClientData[ clientIndex ];
}


// ClientHasFired                  //
// Returns if the client has fired //
sf::Uint8 NetworkController::ClientHasFired( int clientIndex ) {
	return mClientHasFired[ clientIndex ];
}

