#include "Object.h"
#include "utils.h"
#include "Constants.h"

void Object::physics_update(int deltaTime) {
    if (physical_body) {
        position = to_glm(physical_body->GetPosition()) * Constants::Units::pixels_per_meter;
        rotation = physical_body->GetAngle();
        if (sprite) {
            sprite->setPosition(position);
            sprite->setRotation(rotation);
        }
    }
}

void Object::update(int deltaTime) {
    physics_update(deltaTime);
}

void Object::render() {
    sprite->render();
}

void Object::set_position(glm::vec2 const& position) {
    if (physical_body) {
        physical_body->SetTransform(to_box2d(position * Constants::Units::meters_per_pixel), physical_body->GetAngle());
    }
    if (sprite) {
        sprite->setPosition(position);
    }
    this->position = position;
}

void Object::set_rotation(float radians) {
    if (physical_body) {
        physical_body->SetTransform(physical_body->GetPosition(), radians);
    }
    if (sprite) {
        sprite->setRotation(radians);
    }
    rotation = radians;
}
