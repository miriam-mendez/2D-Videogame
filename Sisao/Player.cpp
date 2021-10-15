#include "Player.h"
#include "utils.h"
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include "Game.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include "Constants.h"

void Player::init(b2World& physics, ShaderProgram& shaderProgram) {
    const glm::ivec2 sprite_size_pixels = glm::ivec2(28, 33);

    spritesheet.loadFromFile("images/yoshi.png", TEXTURE_PIXEL_FORMAT_RGBA);
    sprite = Sprite::createSprite(sprite_size_pixels, glm::vec2(1 / 12.f, 1 / 2.f), &spritesheet, &shaderProgram);
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

    // PHYSICS: BODY CREATION
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(position.x, position.y);
    physic_body = physics.CreateBody(&body_def);
    // PHYSICS: SHAPE SETUP
    const glm::vec2 sprite_size_meters = glm::vec2(sprite_size_pixels) * Constants::Units::meters_per_pixel;
    b2PolygonShape box_shape;
    box_shape.SetAsBox(sprite_size_meters.x / 2.f, sprite_size_meters.y / 2.f);
    b2FixtureDef fixture_def;
    fixture_def.shape = &box_shape;
    fixture_def.density = 1.0f;
    fixture_def.friction = 0.2f;
    fixture_def.filter.categoryBits = (int)Constants::Physics::Category::Regular;
    fixture_def.filter.maskBits = (int)Constants::Physics::Mask::Regular;
    auto fixture = physic_body->CreateFixture(&fixture_def);
    physic_body->SetFixedRotation(true);
}

void Player::update(int deltaTime) {
    physics_update(deltaTime);

    b2Vec2 velocity = physic_body->GetLinearVelocity();

    float max_xspeed_meters = Constants::Regular::max_movement_speed * Constants::Units::meters_per_pixel;
    float impulse = physic_body->GetMass() * (max_xspeed_meters - glm::abs(velocity.x));

    auto h_impulse = glm::vec2(0, 0);
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
            physic_body->ApplyLinearImpulseToCenter(b2Vec2(0, -impulse), true);
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
    auto gravity = Constants::Physics::gravity * Constants::Units::meters_per_pixel;
    physic_body->ApplyForceToCenter(to_box2d(gravity), true);
}

void Player::begin_overlap(b2Contact* contact) {
    Object::uuid_t id1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    Object::uuid_t id2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;

    if (uuid == id1 || uuid == id2) {
        jumping = false;
    }
}
