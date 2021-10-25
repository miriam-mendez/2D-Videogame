#include "Object.h"
#include "Utils.h"
#include "Constants.h"

void Object::physics_update(int deltaTime) {
    if (physic_body) {
        position = to_glm(physic_body->GetPosition()) * Constants::Units::pixels_per_meter;
        rotation = physic_body->GetAngle();
        if (sprite) {
            sprite->set_position(position);
            sprite->set_rotation(rotation);
        }
    }
}

Object::~Object() {
    delete sprite;
}

void Object::update(int deltaTime) {
    physics_update(deltaTime);
}

void Object::render() {
    if (sprite)
        sprite->render();
}

Object::uuid_t Object::get_id() const {
    return uuid;
}

void Object::set_position(glm::vec2 const& position) {
    if (physic_body) {
        physic_body->SetTransform(to_box2d(position * Constants::Units::meters_per_pixel), physic_body->GetAngle());
    }
    if (sprite) {
        sprite->set_position(position);
    }
    this->position = position;
}

void Object::set_rotation(float radians) {
    if (physic_body) {
        physic_body->SetTransform(physic_body->GetPosition(), radians);
    }
    if (sprite) {
        sprite->set_rotation(radians);
    }
    rotation = radians;
}

glm::vec2 Object::get_position() {
    return position;
}

float Object::get_rotation() {
    return rotation;
}
