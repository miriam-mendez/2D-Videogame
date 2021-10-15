#pragma once
#include "Object.h"

class Box :
    public Object {
public:
    Box(Object::uuid_t uuid) : Object(uuid) {}

    virtual void init(b2World& physics, ShaderProgram& shaderProgram);
    void update(int deltaTime) override;
    //void render() override;

};

