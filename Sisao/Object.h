#pragma once
#include <box2d/b2_body.h>
#include <glm/glm.hpp>
#include "Sprite.h"
#include "TileMap.h"
#include "PhysicsListener.h"
#include <limits>

class Object : public PhysicsListener<Object> {
public:
    using uuid_t = uint32_t;
    static constexpr uuid_t undefined_uuid = std::numeric_limits<uuid_t>::max();

    Object() : uuid(undefined_uuid) {};
    Object(uuid_t uuid) : uuid(uuid) {};

    virtual void update(int deltaTime);
    virtual void render();

    uuid_t get_id() const;
    void set_position(glm::vec2 const& position);
    void set_rotation(float radians);
    glm::vec2 get_position();
    float get_rotation();

protected:
    uuid_t uuid;
    b2Body* physic_body = nullptr; // automatically invalidated
    glm::vec2 position = glm::vec2(0, 0); // IN PIXELS
    float rotation = 0.0f; // IN RADIANS

    Texture spritesheet;
    Sprite* sprite = nullptr;

    virtual void physics_update(int deltaTime);
};

