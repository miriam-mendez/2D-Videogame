#pragma once
#include "Object.h"

class PhysicPlayer :
    public Object {

    enum PlayerAnims {
        STAND_LEFT_UP,
        STAND_RIGHT_UP,
        MOVE_LEFT_UP,
        MOVE_RIGHT_UP,
        /* STAND_LEFT_DOWN,
        STAND_RIGHT_DOWN,
        MOVE_LEFT_DOWN,
        MOVE_RIGHT_DOWN */
    };

public:
    virtual void init(b2World* physics, ShaderProgram& shaderProgram);
    void update(int deltaTime) override;
};

