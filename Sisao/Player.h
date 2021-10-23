#pragma once
#include "Object.h"

class Player :
    public Object {
protected:
    bool inverted = false;
    bool jumping = false;
    bool falling = false;
    bool standing = false;
    virtual void begin_overlap(b2Contact* contact);
public:
    enum PlayerAnims {
        STAND,
        JUMP_UP,
        FALL,
        FALL_TO_STAND,
        RUN,
        RUN_TO_STAND
    };

    Player(Object::uuid_t uuid) : Object(uuid) {}

    virtual void init(b2World* physics, ShaderProgram& shaderProgram, bool invert);
    virtual void update(int deltaTime);
};

