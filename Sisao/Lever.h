#pragma once
#include "Object.h"

class Lever :
    public Object {

    bool inverted = false;
public:
    Lever(Object::uuid_t uuid) : Object(uuid) {}

    virtual void init(b2World* physics, ShaderProgram& shaderProgram, bool inverted);
    void update(int deltaTime) override;
};

