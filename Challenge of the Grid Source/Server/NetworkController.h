#ifndef _NETWORKCONTROLLER_H_
#define _NETWORKCONTROLLER_H_


#include <SFML/Network.hpp> // sfml netowrk
#include <deque>            // deque


// ConnectionInfo //
struct ConnectionInfo {
	sf::IpAddress  address;
	unsigned short port;

	// = overload
	ConnectionInfo& operator =( const ConnectionInfo &otherInfo ) {
		address = otherInfo.address;
		port    = otherInfo.port;

		return *this;
	}

	// == overload
	bool operator ==( const ConnectionInfo &otherInfo ) const {
		return ( address == otherInfo.address && port == otherInfo.port );
	}

	// != overload
	bool operator !=( const ConnectionInfo &otherInfo ) const {
		return ( address != otherInfo.address || port != otherInfo.port );
	}
};


// PlayerObjectData (Packet struct)
struct PlayerObjectData {
    float pX;
	float pY;
	float dX;
	float dY;
	float angle;
};
// << overload //
sf::Packet& operator <<( sf::Packet& packet, const PlayerObjectData& playerObjectData ); 
// >> overload //
sf::Packet& operator >>( sf::Packet& packet, PlayerObjectData& playerObjectData );


// GameObjectData (Packet struct)
struct GameObjectData {
	float pX;
	float pY;
	float dX;
	float dY;
};
// << overload //
sf::Packet& operator <<( sf::Packet& packet, const GameObjectData& gameObjectData ); 
// >> overload //
sf::Packet& operator >>( sf::Packet& packet, GameObjectData& gameObjectData );


// NetworkController      //
// Handles all networking //
class NetworkController {
public:
	NetworkController( void );
	~NetworkController( void );

	void Initialize( void );

	enum NetworkState {
		INITIALIZING,
		WAITING_ON_CLIENTS,
		ACTIVE
	};

	// Client info setter
	void SetClientInfo( int clientIndex, std::string address, unsigned short port );

	// External checks
	bool CheckNetworkState( NetworkState testState );
	bool PacketReceived( int clientNumber );

	// Create client data packets
	void CreateClientPacket( PlayerObjectData playerData, sf::Uint8 hasFired );

	// Send / Receive / Reset
	void Send( int clientIndex );
	void SendToClient( ConnectionInfo clientInfo, sf::Packet& packet );
	void Receive( void );
	void ReceiveFromClient( sf::Packet& packet );
	void Reset( void );

	// Getters
	PlayerObjectData ReturnClientData( int clientIndex );
	sf::Uint8 ClientHasFired( int clientIndex );

private:
	// Current state
	NetworkState mNetworkState; 

	// PacketReceived flag
	bool mClientPacketProcessed[ 2 ];

	// Out&Inbound Packets
	sf::Packet mOutboundPacket;
	sf::Packet mInboundPacket;

	// Connection details
	ConnectionInfo mLocal;
	ConnectionInfo mIncoming;
	ConnectionInfo mClient[ 2 ];

	// UdpSocket
	sf::UdpSocket mUdpSocket;

	// Client connected flag
	sf::Uint8 mClientConnected[ 2 ];

	// Client PlayerDataObjects
	PlayerObjectData mClientData[ 2 ];

	// Client hasFiredBullet flags
	sf::Uint8 mClientHasFired[ 2 ];

	// Unpack packet data into correct array indices
	void UnpackPacket( int clientIndex );
};


#endif