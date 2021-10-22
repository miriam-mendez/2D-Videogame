#pragma once
#include "Player.h"


class FlippedPlayer : public Player {
protected:
public:
    FlippedPlayer(Object::uuid_t uuid) : Player(uuid) {}
    //virtual void init(b2World* physics, ShaderProgram& shaderProgram);
    virtual void update(int deltaTime);
};

