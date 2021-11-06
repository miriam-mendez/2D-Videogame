#include "Balance.h"
#include "Sprite.h"
#include "Constants.h"
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include "Utils.h"

void Balance::init(b2World* physics, ShaderProgram& shaderProgram, int length, float angle) {
    const glm::ivec2 sprite_size_pixels = glm::ivec2(length, 16);
    spritesheet.loadFromFile("images/balance.png", TEXTURE_PIXEL_FORMAT_RGBA);
    quad = Sprite::init(sprite_size_pixels, glm::vec2(1, 1), &spritesheet, &shaderProgram);
    auto sprite = static_cast<Sprite*>(quad);
    sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
    sprite->changeAnimation(0);
    sprite->set_position(position);
    sprite->set_rotation(glm::radians(angle));

    const glm::ivec2 axis_size_pixels = glm::ivec2(16, 16);
    axisTex.loadFromFile("images/bolt.png", TEXTURE_PIXEL_FORMAT_RGBA);
    axis = Sprite::init(axis_size_pixels, glm::vec2(1, 1), &axisTex, &shaderProgram);
    axis->addKeyframe(0, glm::vec2(0.f, 0.f));
    axis->changeAnimation(0);
    axis->set_position(position);


    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(position.x, position.y);
    body_def.angle = glm::radians(angle);
    physic_body = physics->CreateBody(&body_def);

    const glm::vec2 sprite_size_meters = glm::vec2(sprite_size_pixels) * Constants::Units::meters_per_pixel;
    b2PolygonShape box_shape;
    box_shape.SetAsBox(sprite_size_meters.x / 2.f, sprite_size_meters.y / 2.f);
    b2FixtureDef fixture_def;
    fixture_def.shape = &box_shape;
    fixture_def.density = 2.0f;
    fixture_def.friction = 0.5f;
    fixture_def.filter.categoryBits = (int)Constants::Physics::Category::Regular;
    fixture_def.filter.maskBits = (int)Constants::Physics::Mask::Regular;
    physic_body->CreateFixture(&fixture_def);

    b2BodyDef body2_def;
    body_def.type = b2_staticBody;
    body_def.position.Set(position.x, position.y);
    axis_physic_body = physics->CreateBody(&body_def);

    b2RevoluteJointDef jointDef;
    jointDef.bodyA = physic_body;
    jointDef.collideConnected = false;
    jointDef.bodyB = axis_physic_body;
    jointDef.lowerAngle = -0.25f * b2_pi; // -45 graus
    jointDef.upperAngle = 0.25f * b2_pi; // 45 graus
    jointDef.enableLimit = true;
    jointDef.maxMotorTorque = 50.f;
    jointDef.motorSpeed = 0.0f;
    jointDef.enableMotor = true;

    b2RevoluteJoint* joint = (b2RevoluteJoint*)physics->CreateJoint(&jointDef);

    b2BodyUserData data;
    data.pointer = get_id();
    physic_body->GetUserData() = data;
}



void Balance::set_position(glm::vec2 const& position) {
    physic_body->SetTransform(to_box2d(position * Constants::Units::meters_per_pixel), physic_body->GetAngle());
    axis_physic_body->SetTransform(to_box2d(position * Constants::Units::meters_per_pixel), axis_physic_body->GetAngle());
    quad->set_position(position);
    axis->set_position(position);
    this->position = position;
}

void Balance::update(int deltaTime) {
    physics_update(deltaTime);

    // gravity
    auto gravity = Constants::Physics::gravity * Constants::Units::meters_per_pixel;
    physic_body->ApplyForceToCenter(to_box2d(gravity * physic_body->GetMass()), true);
}

void Balance::render() {
    quad->render();
    axis->render();
}
