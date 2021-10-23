#pragma once
#include "Object.h"

class Wall :
    public Object {

    bool inverted = false;
public:
    Wall(Object::uuid_t uuid) : Object(uuid) {}

    virtual void init(b2World* physics, ShaderProgram& shaderProgram, bool inverted);
    void update(int deltaTime) override;
    void render() override;

    void set(bool active);
};

