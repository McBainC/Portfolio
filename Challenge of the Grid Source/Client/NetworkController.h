#ifndef _NETWORKCONTROLLER_H_
#define _NETWORKCONTROLLER_H_


#include <SFML/Network.hpp> // sfml netowrk
#include <deque>            // deque


// ConnectionInfo //
struct ConnectionInfo {
	sf::IpAddress  address;
	unsigned short port;
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
		CONNECTING,
		ACTIVE
	};

	// Local IpAddress getter
	std::string GetLocalIpAddress( void );

	// Server info setter
	void SetServerInfo( std::string address, unsigned short port );

	// External checks
	bool CheckNetworkState( NetworkState testState );
	bool PacketReceived( void );

	// Create player data packets
	void CreateServerPacket( PlayerObjectData playerData, sf::Uint8 hasFired );

	// Send / Receive / Reset
	void Send( void );
	void SendToServer( sf::Packet& packet );
	void Receive( void );
	void ReceiveFromServer( sf::Packet& packet );
	void Reset( void );

	// Challenger getters
	PlayerObjectData ReturnChallengerData( void );
	bool ChallengerHasFired( void );

private:
	// Current state
	NetworkState mNetworkState; 

	// Dead Reckoning required
	bool mPacketProcessed;

	// Out&Inbound Packets
	sf::Packet mOutboundPacket;
	sf::Packet mInboundPacket;

	// Connection details
	ConnectionInfo mLocal;
	ConnectionInfo mServer;

	// UdpSocket
	sf::UdpSocket mUdpSocket;

	// Challenger Data
	PlayerObjectData mChallengerData;
	sf::Uint8 mChallengerHasFired;
};


#endif