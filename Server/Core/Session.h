#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

class Session : public IocpObject
{
	friend class Listener;
	friend class IOCPCore;
	friend class Service;

	enum {
		BUFFER_SIZE = 0x10000 // 64KB
	};
public:
	Session();
	virtual ~Session();
public:
	void SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress GetAddress() { return _netAddress; }
	SOCKET GetSocket() { return _socket; }
	bool IsConnected() { return _connected; }
	SessionRef GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
	shared_ptr<Service> GetService() { return _service.lock(); }
	void SetService(shared_ptr<Service> service) { _service = service; }
public:
	// �ܺο��� ���
	void Send(SendBufferRef sendBuffer);
	bool Connect();
	void Disconnect(const WCHAR* cause);
private:
	// �������̽� ����
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IOCPEvent* iocpEvent, int32 numOfBytes = 0) override;
private:
	// ���� ����
	bool RegisterConnect();
	bool RegisterDiconnect();
	void RegisterRecv();
	void RegisterSend();
	
	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(int32 numOfBytes);
	void ProcessSend(int32 numOfBytes);

	void HandleError(int32 errCode);
protected:
	// ������ �ڵ忡�� �����ε�
	virtual void OnConnected() {}
	virtual int32 OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void OnSend(int32 len) {}
	virtual void OnDisconnected() {}
private:
	weak_ptr<Service> _service;
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	Atomic<bool> _connected = false;
private:
	USE_LOCK;
	// ����
	RecvBuffer _recvBuffer;
	// �۽�
	Queue<SendBufferRef> _sendQueue;
	Atomic<bool> _sendRegistered = false;
private:
	RecvEvent _recvEvent;
	SendEvent _sendEvent;
	ConnectEvent _connectEvent;
	DisconnectEvent _disconnectEvent;
};



//
struct PacketHeader
{
	uint16 size;
	uint16 id;		
};

// [size][id][data...]
class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();
public:
	PacketSessionRef GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }
protected:
	virtual int32 OnRecv(BYTE* buffer, int32 len);
	virtual void OnRecvPacket(BYTE* buffer, int32 len) abstract;
};