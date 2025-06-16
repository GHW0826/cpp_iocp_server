#pragma once

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	// PreRecv, 0byte recv
	Recv,
	Send
};

// �����Լ� ��� x
// �����Լ� ���̺��� �Ǿտ� ��ġ��,
// OVERLAPPED ������ ���� ��ġ�� �����Ͱ� ������ �� ����.
// OVERLAPPED EX ����
class IOCPEvent : public OVERLAPPED
{
public:
	IOCPEvent(EventType type);
public:
	void Init();
public:
	EventType _eventType;
	IocpObjectRef _owner;
};

// ConenctEvent
class ConnectEvent : public IOCPEvent
{
public:
	ConnectEvent() 
		: IOCPEvent(EventType::Connect)
	{
	}
};

// DisconenctEvent
class DisconnectEvent : public IOCPEvent
{
public:
	DisconnectEvent() 
		: IOCPEvent(EventType::Disconnect)
	{
	}
};


// AcceptEvent
class AcceptEvent : public IOCPEvent
{
public:
	AcceptEvent() 
		: IOCPEvent(EventType::Accept)
	{
	}
public:
	SessionRef _session = nullptr;
};


// RecvEvent
class RecvEvent : public IOCPEvent
{
public:
	RecvEvent() 
		: IOCPEvent(EventType::Recv)
	{
	}
};

// SendEvent
class SendEvent : public IOCPEvent
{
public:
	SendEvent() 
		: IOCPEvent(EventType::Send)
	{
	}

	Vector<SendBufferRef> _sendBuffers;
};
