#pragma once

class IHttpController 
{
public:
    virtual ~IHttpController() = default;
    virtual void RegisterRoutes(class ControllerRegistry& registry) = 0;
};