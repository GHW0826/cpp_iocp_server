#pragma once
#include <functional>

#include "HttpBaseMessage.h"
#include "IHttpController.h"

class ControllerRegistry {
    using HttpHandler = std::function<HttpResponse(const HttpRequest&)>;
public:
    void RegisterHandler(const std::string& method, const std::string& path, HttpHandler handler) {
        _handlers[method + path] = handler;
    }

    HttpHandler FindHandler(const std::string& method, const std::string& path) const {
        auto it = _handlers.find(method + path);
        if (it != _handlers.end()) {
            return it->second;
        }
        return nullptr;
    }

    void RegisterController(std::unique_ptr<IHttpController> controller) {
        controller->RegisterRoutes(*this);
        _controllers.emplace_back(std::move(controller));
    }
private:
    std::unordered_map<std::string, HttpHandler> _handlers;
    std::vector<std::unique_ptr<IHttpController>> _controllers;
};