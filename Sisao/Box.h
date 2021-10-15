#pragma once
#include "Object.h"

class Box :
    public Object {
public:

    virtual void init(b2World* physics, ShaderProgram& shaderProgram);
    //void update(int deltaTime) override;
    //void physics_update(int deltaTime) override;
    //void render() override;

};

