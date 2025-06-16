#pragma once
#include "IHttpController.h"
#include "HttpBaseMessage.h"
#include "ControllerRegistry.h"

class HelloController : public IHttpController {
public:
    void RegisterRoutes(ControllerRegistry& registry) override {
        registry.RegisterHandler("GET", "/hello", [](const HttpRequest& req) {
            HttpResponse res;
            res.body = "Hello, IOCP HTTP!";
            res.headers["Content-Type"] = "text/plain";
            return res;
            });

        registry.RegisterHandler("POST", "/echo", [](const HttpRequest& req) {
            HttpResponse res;
            res.body = "You sent: " + req.body;
            res.headers["Content-Type"] = "text/plain";
            return res;
            });

        registry.RegisterHandler("GET", "/json", [](const HttpRequest& req) {
            HttpResponse res;
            res.body = R"({\"message\": \"This is JSON\"})";
            res.headers["Content-Type"] = "application/json";
            return res;
            });
    }
};