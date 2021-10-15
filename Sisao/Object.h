#pragma once
#include <box2d/b2_body.h>
#include <glm/glm.hpp>
#include "Sprite.h"
#include "TileMap.h"

class Object {
protected:
    b2Body* physical_body = nullptr; // automatically invalidated
    glm::vec2 position = glm::vec2(0, 0); // IN PIXELS
    float rotation = 0.0f; // IN RADIANS

    Texture spritesheet;
    Sprite* sprite = nullptr;

    virtual void physics_update(int deltaTime);

public:
    virtual void update(int deltaTime);
    virtual void render();

    void set_position(glm::vec2 const& position);
    void set_rotation(float radians);
    /*glm::vec2 get_position();
    float get_rotation(); */
};

