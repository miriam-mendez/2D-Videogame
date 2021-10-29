#pragma once
#include "Object.h"

class Box :
    public Object {

    bool inverted = false;
    bool in_water = false;
    virtual void begin_overlap(b2Contact* contact);
public:
    Box(Object::uuid_t uuid) : Object(uuid) {}

    virtual void init(b2World* physics, ShaderProgram& shaderProgram, bool inverted);
    void update(int deltaTime) override;
};

