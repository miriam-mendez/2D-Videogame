#pragma once
#include "Object.h"
class Water :
    public Object {

public:
    Water(Object::uuid_t uuid) : Object(uuid) {}

    virtual void init(b2World* physics, ShaderProgram& shaderProgram);
    void update(int deltaTime) override;
    void render() override;
};

