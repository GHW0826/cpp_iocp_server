#pragma once

class SendBufferChunk;

class SendBuffer : public enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize);
	~SendBuffer();
public:
	BYTE* Buffer() { return _buffer; }
	uint32 AllocSize() { return _allocSize; }
	uint32 WriteSize() { return _writeSize; }
public:
	void Close(uint32 writeSize);
private:
	BYTE* _buffer;
	uint32 _allocSize = 0;
	uint32 _writeSize = 0;
	SendBufferChunkRef _owner;
};

class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	enum {
		SEND_BUFFER_CHUNK_SIZE = 6000
	};
public:
	SendBufferChunk();
	~SendBufferChunk();
public:
	bool IsOpen() { return _open; }
	BYTE* Buffer() { return &_buffer[_usedSize]; }
	uint32 FreeSize() { return static_cast<uint32>(_buffer.size()) - _usedSize; }
public:
	void Reset();
	SendBufferRef Open(uint32 allocSize);
	void Close(uint32 wrtieSize);
private:
	Array<BYTE, SEND_BUFFER_CHUNK_SIZE> _buffer = {};
	bool _open = false;
	uint32 _usedSize = 0;
};

class SendBufferManager
{
public:
	SendBufferRef Open(uint32 size);
private:
	SendBufferChunkRef Pop();
	void Push(SendBufferChunkRef buffer);
public:
	static void PushGlobal(SendBufferChunk* buffer);
private:
	USE_LOCK;
	Vector<SendBufferChunkRef> _sendBufferChunks;
};