#pragma once
#include "Object.h"
#include "Sprite.h"
class Water :
    public Object {
    Quad* quad2 = nullptr;
    bool in_range = false;
    enum State {
        SPLASH
    };
public:
    Water(Object::uuid_t uuid) : Object(uuid) {}

    virtual void init(b2World* physics, ShaderProgram& shaderProgram);
    void update(int deltaTime) override;
    void render() override;

    virtual void begin_overlap(b2Contact* contact);
    
    // checks if one of the ids references this water and the other is a box
    bool valid_contact(Object::uuid_t id1, Object::uuid_t id2);
};

