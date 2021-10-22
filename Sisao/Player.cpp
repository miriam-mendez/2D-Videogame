#include "Player.h"
#include "utils.h"
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include "Game.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include "Constants.h"

void Player::init(b2World* physics, ShaderProgram& shaderProgram, bool inverse) {
    inverted = inverse;
    const glm::ivec2 sprite_size_pixels = glm::ivec2(28, 33);

    spritesheet.loadFromFile("images/yoshi.png", TEXTURE_PIXEL_FORMAT_RGBA);
    sprite = Sprite::init(sprite_size_pixels, glm::vec2(1 / 12.f, 1 / 2.f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(4);

    sprite->setAnimationSpeed(STAND_LEFT_UP, 8);
    sprite->addKeyframe(STAND_LEFT_UP, glm::vec2(0.f, 0.5f));

    sprite->setAnimationSpeed(STAND_RIGHT_UP, 8);
    sprite->addKeyframe(STAND_RIGHT_UP, glm::vec2(0.f, 0.f));

    sprite->setAnimationSpeed(MOVE_LEFT_UP, 8);
    sprite->addKeyframe(MOVE_LEFT_UP, glm::vec2(0.f, 0.5f));
    sprite->addKeyframe(MOVE_LEFT_UP, glm::vec2(2 / 12.f, 0.5f));
    sprite->addKeyframe(MOVE_LEFT_UP, glm::vec2(3 / 12.f, 0.5f));

    sprite->setAnimationSpeed(MOVE_RIGHT_UP, 8);
    sprite->addKeyframe(MOVE_RIGHT_UP, glm::vec2(0.f, 0.f));
    sprite->addKeyframe(MOVE_RIGHT_UP, glm::vec2(2 / 12.f, 0.f));
    sprite->addKeyframe(MOVE_RIGHT_UP, glm::vec2(3 / 12.f, 0.f));

    sprite->changeAnimation(0);
    sprite->setPosition(position);
    sprite->setFlip(false, inverted);

    // PHYSICS: BODY CREATION
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(position.x, position.y);
    physic_body = physics->CreateBody(&body_def);
    // PHYSICS: SHAPE SETUP (CAPSULE: 2 circles and 1 rectangle)
    const glm::vec2 sprite_size_meters = glm::vec2(sprite_size_pixels) * Constants::Units::meters_per_pixel;
    float radius = glm::min(sprite_size_meters.x, sprite_size_meters.y) / 2.f;
    b2CircleShape c1;
    c1.m_radius = radius;
    float c1_y = sprite_size_meters.y / 2.f - radius;
    c1.m_p.Set(0, c1_y);
    b2CircleShape c2;
    c2.m_radius = radius;
    float c2_y = -sprite_size_meters.y / 2.f + radius;
    c2.m_p.Set(0, c2_y);
    b2PolygonShape box;
    box.SetAsBox(sprite_size_meters.x / 2.f, glm::abs(c1_y - c2_y) / 2.f, b2Vec2(0, 0), 0.f);

    b2FixtureDef fixture_c1;
    fixture_c1.shape = &c1;
    fixture_c1.density = 1.5f;
    fixture_c1.friction = 1.f;
    fixture_c1.filter.categoryBits = (int)Constants::Physics::Category::Regular;
    fixture_c1.filter.maskBits = (int)Constants::Physics::Mask::Regular;
    physic_body->CreateFixture(&fixture_c1);

    b2FixtureDef fixture_c2;
    fixture_c2.shape = &c2;
    fixture_c2.density = 1.5f;
    fixture_c2.friction = 1.f;
    fixture_c2.filter.categoryBits = (int)Constants::Physics::Category::Regular;
    fixture_c2.filter.maskBits = (int)Constants::Physics::Mask::Regular;
    physic_body->CreateFixture(&fixture_c2);

    b2FixtureDef fixture_box;
    fixture_box.shape = &box;
    fixture_box.density = 1.5f;
    fixture_box.friction = 1.f;
    fixture_box.filter.categoryBits = (int)Constants::Physics::Category::Regular;
    fixture_box.filter.maskBits = (int)Constants::Physics::Mask::Regular;
    physic_body->CreateFixture(&fixture_box);
    physic_body->SetFixedRotation(true);
}

void Player::update(int deltaTime) {
    physics_update(deltaTime);

    b2Vec2 velocity = physic_body->GetLinearVelocity();

    float max_xspeed_meters = Constants::Regular::max_movement_speed * Constants::Units::meters_per_pixel;
    float impulse = physic_body->GetMass() * (max_xspeed_meters - glm::abs(velocity.x));
    auto h_impulse = glm::vec2(0, 0);
    float gravity_direction_y = (inverted) ? -1 : 1;
    if (Game::instance().getSpecialKey(GLUT_KEY_LEFT)) {
        if (sprite->animation() != MOVE_LEFT_UP)
            sprite->changeAnimation(MOVE_LEFT_UP);
        h_impulse += glm::vec2(-impulse, 0);
        /*if (map->collisionMoveLeft(posPlayer, glm::ivec2(28, 33))) {
            posPlayer.x += 2;
            sprite->changeAnimation(STAND_LEFT_UP);
        }*/
    }
    if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) {
        if (sprite->animation() != MOVE_RIGHT_UP)
            sprite->changeAnimation(MOVE_RIGHT_UP);
        h_impulse += glm::vec2(impulse, 0);
        /*if (map->collisionMoveRight(posPlayer, glm::ivec2(28, 33))) {
            posPlayer.x -= 2;
            sprite->changeAnimation(STAND_RIGHT_UP);
        }*/
    }
    physic_body->ApplyLinearImpulseToCenter(to_box2d(h_impulse), true);

    if (Game::instance().getSpecialKey(GLUT_KEY_UP)) {
        float max_jump = Constants::Regular::max_jump_speed * Constants::Units::meters_per_pixel;
        float vvchange = max_jump - glm::abs(velocity.y);
        float impulse = physic_body->GetMass() * vvchange;
        if (!jumping) {
            physic_body->ApplyLinearImpulseToCenter(b2Vec2(0, -gravity_direction_y * impulse), true);
            jumping = true;
        }
    }
    /*else {
        if (sprite->animation() == MOVE_LEFT_UP)
            sprite->changeAnimation(STAND_LEFT_UP);
        else if (sprite->animation() == MOVE_RIGHT_UP)
            sprite->changeAnimation(STAND_RIGHT_UP);
    }*/

    // gravity
    auto gravity = Constants::Physics::gravity * Constants::Units::meters_per_pixel * gravity_direction_y;
    physic_body->ApplyForceToCenter(to_box2d(gravity * physic_body->GetMass()), true);
}

void Player::begin_overlap(b2Contact* contact) {
    Object::uuid_t id1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    Object::uuid_t id2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;

    //Game::instance().get_scene().get_object(id2); might be useful with dynamic_cast ;D

    if (jumping && (uuid == id1 || uuid == id2)) {
        b2WorldManifold b;
        contact->GetWorldManifold(&b);
        auto normal = glm::vec2(b.normal.x, b.normal.y);
        float gravity_direction_y = (inverted) ? -1 : 1;
        auto dot = glm::dot(normal, glm::vec2(0.f, 1.f)) * -gravity_direction_y;
        jumping = dot < 0.9;
    }
}
