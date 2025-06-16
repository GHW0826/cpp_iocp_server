#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

Session::Session()
	: _recvBuffer(BUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

void Session::Send(SendBufferRef sendBuffer)
{
	if (IsConnected() == false) {
		cout << "is not connected" << endl;
		return;
	}

	// q?
	// 1) ���� ����
	// 2) sendEvent ����?, ����, ������, WSASend ��ø.
	// 
	// 
	// ����RegisterSend�� �ɸ��� ���� ���¸�, �ɾ��ش�.
	bool registerSend = false;
	{
		WRITE_LOCK;

		_sendQueue.push(sendBuffer);
		if (_sendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend) {
		// cout << "if register Send" << endl;
		RegisterSend();
	}
}

bool Session::Connect()
{
	// cout << "Session::Connect" << endl;
	return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
	// cout << "Session::Disconnect" << endl;

	if (_connected.exchange(false) == false)
		return;

	// TEMP
	// wcout << "Disconnect : " << cause << endl;

	RegisterDiconnect();
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IOCPEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->_eventType) {
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		break;
	default:
		break;
	}
}




bool Session::RegisterConnect()
{
	if (IsConnected())
		return false;

	if (GetService()->GetServiceType() != ServiceType::Client)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::BindAnyAddress(_socket, 0) == false)
		return false;

	_connectEvent.Init();
	_connectEvent._owner = shared_from_this(); // add ref

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (false == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent)) {
		int32 errCode = ::WSAGetLastError();
		if (errCode != WSA_IO_PENDING) {
			cout << errCode << endl;
			_connectEvent._owner = nullptr; // release ref
			return false;
		}
	}

	return true;
}

bool Session::RegisterDiconnect()
{
	_disconnectEvent.Init();
	_disconnectEvent._owner = shared_from_this(); // add ref
	if (false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0)) {
		int32 errCode = ::WSAGetLastError();
		if (errCode != WSA_IO_PENDING) {
			cout << errCode << endl;
			_disconnectEvent._owner = nullptr; // release ref
			return false;
		}
	}

	return true;
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	_recvEvent.Init();
	_recvEvent._owner = shared_from_this(); // ADD Ref

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr)) {
		int32 errCode = ::WSAGetLastError();
		if (errCode != WSA_IO_PENDING) {
			cout << errCode << endl;
			HandleError(errCode);
			_recvEvent._owner = nullptr; // Release Ref
		}
	}
}

// �ѹ��� �ѽ����常 ȣ��
void Session::RegisterSend()
{
	if (IsConnected() == false) {
		cout << "[RegisterSend] is not connected" << endl;
		return;
	}

	_sendEvent.Init();
	_sendEvent._owner = shared_from_this(); // ADD Ref

	// ���� �����͸� sendEvent�� ���
	{
		WRITE_LOCK;
		int32 writeSize = 0;
		while (_sendQueue.empty() == false) {
			SendBufferRef sendBuffer = _sendQueue.front();

			writeSize += sendBuffer->WriteSize();
			// TODO ���� üũ

			_sendQueue.pop();
			_sendEvent._sendBuffers.push_back(sendBuffer);
		}
	}

	Vector<WSABUF> wsaBufs;
	wsaBufs.reserve(_sendEvent._sendBuffers.size());
	for (SendBufferRef sendBuffer : _sendEvent._sendBuffers) {
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT &numOfBytes, 0, &_sendEvent, nullptr)) {
		int32 errCode = ::WSAGetLastError();
		if (errCode != WSA_IO_PENDING) {
			cout << errCode << endl;
			HandleError(errCode);
			_sendEvent._owner = nullptr; // release ref
			_sendEvent._sendBuffers.clear(); // release ref
			_sendRegistered.store(false);
		}
	}
}

void Session::ProcessConnect()
{
	_connectEvent._owner = nullptr; // release ref
	_connected.store(true);

	// ���� ���
	GetService()->AddSession(GetSessionRef());

	// ������ �ڵ忡�� �����ε�
	OnConnected();

	// ���� ���
	RegisterRecv();
}

void Session::ProcessDisconnect()
{
	_disconnectEvent._owner = nullptr; // release ref


	OnDisconnected(); // ������ �ڵ忡�� �����ε�
	GetService()->ReleaseSession(GetSessionRef());
}

void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent._owner = nullptr; // Release Ref

	if (numOfBytes == 0) {
		Disconnect(L"Recv 0)");
		return;
	}


	if (_recvBuffer.OnWrite(numOfBytes) == false) {
		Disconnect(L"OnWrite Overflow");
		return;
	}

	// ������
	int32 dataSize = _recvBuffer.DataSize();
	int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);
	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false) {
		Disconnect(L"OnRead Overflow");
		return;
	}

	// Ŀ�� ����
	_recvBuffer.Clean();

	// ���� ���
	RegisterRecv();
}



void Session::ProcessSend(int32 numOfBytes)
{
	_sendEvent._owner = nullptr; // release ref
	_sendEvent._sendBuffers.clear(); 
	if (numOfBytes == 0) {
		Disconnect(L"Send 0");
		return;
	}

	// ������ �ڵ忡�� �����ε�
	OnSend(numOfBytes);

	WRITE_LOCK;
	if (_sendQueue.empty())
		_sendRegistered.store(false);
	else
		RegisterSend();
}

void Session::HandleError(int32 errCode)
{
	switch (errCode) {
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		// TODO Log (Log Thread)
		cout << "Handle Error: " << errCode << endl;
		break;
	}
}





PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
	int32 processLen = 0;
	while (true) {
		int32 dataSize = len - processLen;
		// �ּ��� ����� �Ľ��� �� �־�� �Ѵ�.
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		// ����� ��ϵ� ��Ŷ ũ�⸦ �Ľ��� �� �־�� ��.
		if (dataSize < header.size)
			break;

		// ��Ŷ ���� ����
		OnRecvPacket(&buffer[processLen], header.size);

		processLen += header.size;
	}

	return processLen;
}
