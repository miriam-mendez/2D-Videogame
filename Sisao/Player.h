#pragma once
#include "Object.h"

class Player :
    public Object {
protected:
    bool inverted = false;
    bool jumping = false;
    int contacts = 0;
    virtual void begin_overlap(b2Contact* contact);
    virtual void end_overlap(b2Contact* contact);
public:
    enum PlayerAnims {
        STAND,
        JUMP_UP,
        FALL,
        FALL_TO_STAND,
        RUN,
        RUN_TO_STAND,
        HANG
        /*STAND_LEFT_UP,
        STAND_RIGHT_UP,
        MOVE_LEFT_UP,
        MOVE_RIGHT_UP,*/
        /* STAND_LEFT_DOWN,
        STAND_RIGHT_DOWN,
        MOVE_LEFT_DOWN,
        MOVE_RIGHT_DOWN */
    };

    Player(Object::uuid_t uuid) : Object(uuid) {}

    virtual void init(b2World* physics, ShaderProgram& shaderProgram, bool invert);
    virtual void update(int deltaTime);
};

