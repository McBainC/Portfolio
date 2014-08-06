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


// Constructor                //
// Initialize known variables //
NetworkController::NetworkController( void ) :
 mNetworkState( INITIALIZING ), mPacketProcessed( false ) {
	Initialize();
}


// Destructor    //
// Clear vectors //
NetworkController::~NetworkController( void ) {
	Reset();
}


// Initialize                                              //
// Sets local IpAddress and attempts to bind the UdpSocket //
// mNetworkState upgraded to CONNECTING if successful      //
void NetworkController::Initialize( void ) {
	Reset();

	// Set local address
	mLocal.address = sf::IpAddress::getLocalAddress();

	// Set local listening port number
	mLocal.port = sf::Socket::AnyPort;

	// Set UpdSocket to Non-Blocking
	mUdpSocket.setBlocking( false );

	// Bind Upd Socket
	if( mUdpSocket.bind( mLocal.port ) != sf::Socket::Done ) {
		std::cout << "Failed to bind UdpSocket to port: " << mLocal.port << "!" << std::endl;
	} else {
		std::cout << "Socket bound to port: " << mLocal.port << std::endl;

		// Change network state
		mNetworkState = ACTIVE;
	}

	// Pray
	SetServerInfo( "127.0.0.1"/*GetLocalIpAddress()*/, 44443 );
}


// GetLocalIpAddress                   //
// Returns local ipaddress as a string //
std::string NetworkController::GetLocalIpAddress( void ) {
	return mLocal.address.toString();
}


// SetServerInfo //
void NetworkController::SetServerInfo( std::string address, unsigned short port ) {
	// Set the string as IpAddress
	mServer.address = address;

	// Check its valid
	if( mServer.address == sf::IpAddress::None ) {
		std::cout << "Server Address: " << mServer.address << " - is invalid!" << std::endl;
	}

	// Set the port
	mServer.port = port;
}


// CheckNetworkState               //
// External check for NetworkState //
bool NetworkController::CheckNetworkState( NetworkState testState ) {
	if( mNetworkState == testState ) {
		return true;
	}

	return false;
}


// PacketReceived //
bool NetworkController::PacketReceived( void ) {
	if( mPacketProcessed ) {
		return true;
	}

	return false;
}


// CreateServerPacket               //
// Sends players data to the server //
void NetworkController::CreateServerPacket( PlayerObjectData playerData, sf::Uint8 hasFired ) {
	// Clear the outbound packet
	mOutboundPacket.clear();

	// Add players gameObject struct and hasFired int(bool)
	mOutboundPacket << playerData << hasFired;
}


// Send          //
// Reference fix //
void NetworkController::Send( void ) {
	SendToServer( mOutboundPacket );
}


// SendToServer //
void NetworkController::SendToServer( sf::Packet& packet ) {
	// Attempt to send packet to the server
	if( mUdpSocket.send( packet, mServer.address, mServer.port ) != sf::Socket::Done ) {
		std::cout << "Failed to send packet to: " << mServer.address.toString() 
			      << " Port: " << mServer.port << "!" << std::endl;
	} else {
		std::cout << "Sent packet to: " << mServer.address.toString() 
			      << " Port: " << mServer.port << "!" << std::endl;
	}
}


// Receive       //
// Reference fix //
void NetworkController::Receive( void ) {
	ReceiveFromServer( mInboundPacket );
}


// ReceiveFromServer //
void NetworkController::ReceiveFromServer( sf::Packet& packet ) {
	sf::IpAddress localAddress;
	unsigned short localPort;
	while( mUdpSocket.receive( packet, localAddress, localPort ) == sf::Socket::Done ) {
		// Unpack Challengers object data / trajectory and hasFired bool
		mInboundPacket >> mChallengerData >> mChallengerHasFired;

		// Flag that a packet has been processed
		mPacketProcessed = true;

		// Clear the packet
		mInboundPacket.clear();
	}
}


// Reset                                        //
// Reset the packet variables for the next loop //
void NetworkController::Reset( void ) {
	// Set the packet processed flag to false
	mPacketProcessed = false;

	// Reset hasFired 
	mChallengerHasFired = 0;
}


// ReturnChallengerData //
PlayerObjectData NetworkController::ReturnChallengerData( void ) {
	return mChallengerData;
}


// ChallengerHasFired                               //
// Returns true if newBullet data has been received //
bool NetworkController::ChallengerHasFired( void ) {
	if( mChallengerHasFired ) {
		return true;
	}

	return false;
}

