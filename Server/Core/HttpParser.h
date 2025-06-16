#pragma once
#include "HttpBaseMessage.h"

class HttpParser {
public:
    bool Parse(const std::string& rawData, HttpRequest& outRequest) {
        std::istringstream stream(rawData);
        std::string line;

        if (!std::getline(stream, line)) return false;
        std::istringstream lineStream(line);
        lineStream >> outRequest.method >> outRequest.url;

        while (std::getline(stream, line) && line != "\r") {
            auto colon = line.find(":");
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string value = line.substr(colon + 1);
                if (!value.empty() && value.front() == ' ') value.erase(0, 1);
                if (!value.empty() && value.back() == '\r') value.pop_back();
                outRequest.headers[key] = value;
            }
        }

        std::ostringstream bodyStream;
        while (std::getline(stream, line)) {
            bodyStream << line << "\n";
        }
        outRequest.body = bodyStream.str();
        return true;
    }
};
