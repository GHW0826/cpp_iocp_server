#pragma once
#include "Session.h"
#include "HttpParser.h"
#include "HttpDispatcher.h"
#include <algorithm>

class HttpSession : public PacketSession
{
public:
	HttpSession()
		:_dispatcher(nullptr)
	{
	}
	HttpSession(HttpDispatcher* dispatcher) 
		: _dispatcher(dispatcher) 
	{}

	~HttpSession()
	{
		cout << "~HttpSession" << endl;
	}

	virtual int32 OnRecv(BYTE* buffer, int32 len) override
	{
		size_t contentLength = 0;
		int32 processLen = 0;
		while (true) {
			int32 dataSize = len - processLen;
			std::string msg(reinterpret_cast<const char*>(buffer), len);
			auto it = std::search(msg.begin(), msg.end(), std::begin(_separator), std::end(_separator));
			if (it == msg.end())
				return processLen;

			size_t headerEnd = std::distance(msg.begin(), it);
			size_t bodyStart = headerEnd + 4;
			// Çì´õ ÆÄ½Ì
			std::istringstream headerStream(msg.substr(0, headerEnd));
			std::string line;
			std::map<std::string, std::string> headers;
			while (std::getline(headerStream, line) && !line.empty()) {
				auto colon = line.find(":");
				if (colon != std::string::npos) {
					std::string key = line.substr(0, colon);
					std::string value = line.substr(colon + 1);
					if (!value.empty() && value.front() == ' ') value.erase(0, 1);
					if (key == "Content-Length")
						contentLength = std::stoi(value);
				}
			}

			// ÀüÃ¼ ÆÐÅ¶ µµÂø ¿©ºÎ È®ÀÎ
			if (msg.size() < bodyStart + contentLength)
				break; // ¹Ùµð ´ú ¿È

			OnRecvPacket(buffer, len);
		}

		return contentLength;
	}

	void OnRecvPacket(BYTE* buffer, int32 len)
	{
		PacketSessionRef session = GetPacketSessionRef();

	}

	void OnSend(int32 len)
	{
		// cout << "OnSend Len = " << len << endl;
	}

	void OnConnected()
	{
		// GSessionManager.Add(static_pointer_cast<HttpSession>(shared_from_this()));
	}

	void OnDisconnected()
	{
		// GSessionManager.Remove(static_pointer_cast<HttpSession>(shared_from_this()));
	}
private:
    HttpParser _parser;
	HttpDispatcher* _dispatcher;
	const std::string _separator = "\r\n\r\n";
};

