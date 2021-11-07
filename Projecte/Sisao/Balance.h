#pragma once
#include "Object.h"
#include "Sprite.h"
class Balance :
    public Object {
    b2Body* axis_physic_body = nullptr;
    Sprite* axis = nullptr;
    Texture axisTex;
public:
    Balance(Object::uuid_t uuid) : Object(uuid) {}
    virtual void init(b2World* physics, ShaderProgram& shaderProgram, int length, float angle);

    void set_position(glm::vec2 const& position) override;
    void update(int deltaTime) override;
    void render() override;
};

