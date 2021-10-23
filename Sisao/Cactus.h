#pragma once
#include "Object.h"
class Cactus : public Object {
    virtual void begin_overlap(b2Contact* contact);
public:
    Cactus(Object::uuid_t uuid) : Object(uuid) {}

    virtual void update(int deltaTime);

    virtual void init(b2World* physics, ShaderProgram& shaderProgram, int orientation);
};

