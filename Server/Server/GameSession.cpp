#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	// Echo
	// cout << "OnRecv Len = " << len << endl;

	// ServerPacketHandler::HandlePacket(buffer, len);
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO: packetID 대역 체크
}

void GameSession::OnSend(int32 len)
{
	// cout << "OnSend Len = " << len << endl;
}

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
	_currentPlayer = nullptr;
	_players.clear();
}
