#pragma once
#include "ControllerRegistry.h"

class HttpDispatcher {
public:
    HttpDispatcher(ControllerRegistry& registry) 
        : _registry(registry) 
    {}

    HttpResponse Dispatch(const HttpRequest& request) {
        auto handler = _registry.FindHandler(request.method, request.url);
        if (handler) 
            return handler(request);

        HttpResponse res;
        res.statusCode = 404;
        res.reason = "Not Found";
        res.body = "404 Not Found";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }
private:
    ControllerRegistry& _registry;
};
