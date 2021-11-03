#pragma once
#include "Object.h"



class Planck : public Object
{
    bool inverted = false;

public:
	Planck(Object::uuid_t uuid) : Object(uuid) {}

    virtual void update(int deltaTime) override;

    virtual void init(b2World* physics, ShaderProgram& shaderProgram, bool inverted);
};

