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
    const glm::ivec2 sprite_size_pixels = glm::ivec2(31, 43);

    spritesheet.loadFromFile("images/player-sheet2.png", TEXTURE_PIXEL_FORMAT_RGBA);
    float h_frames = 23.f;
    sprite = Sprite::init(sprite_size_pixels, glm::vec2(1 / h_frames, 1.f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(5);

    sprite->setAnimationSpeed(JUMP_UP, 8);
    sprite->addKeyframe(JUMP_UP, glm::vec2(0.f, 0.f));

    sprite->setAnimationSpeed(FALL, 6);
    sprite->setAnimationLoop(FALL, false);
    sprite->addKeyframe(FALL, glm::vec2(1 / h_frames, 0.f));
    sprite->addKeyframe(FALL, glm::vec2(2 / h_frames, 0.f));

    sprite->setAnimationSpeed(FALL_TO_STAND, 6);
    sprite->setAnimationLoop(FALL_TO_STAND, false);
    sprite->addKeyframe(FALL_TO_STAND, glm::vec2(3 / h_frames, 0.f));
    sprite->addKeyframe(FALL_TO_STAND, glm::vec2(4 / h_frames, 0.f));


    sprite->setAnimationSpeed(STAND, 5);
    sprite->addKeyframe(STAND, glm::vec2(5 / h_frames, 0.f));
    sprite->addKeyframe(STAND, glm::vec2(6 / h_frames, 0.f));
    sprite->addKeyframe(STAND, glm::vec2(7 / h_frames, 0.f));
    sprite->addKeyframe(STAND, glm::vec2(8 / h_frames, 0.f));
    sprite->addKeyframe(STAND, glm::vec2(9 / h_frames, 0.f));
    sprite->addKeyframe(STAND, glm::vec2(10 / h_frames, 0.f));
    sprite->addKeyframe(STAND, glm::vec2(11 / h_frames, 0.f));

    sprite->setAnimationSpeed(RUN, 11);
    sprite->addKeyframe(RUN, glm::vec2(12 / h_frames, 0.f));
    sprite->addKeyframe(RUN, glm::vec2(13 / h_frames, 0.f));
    sprite->addKeyframe(RUN, glm::vec2(14 / h_frames, 0.f));
    sprite->addKeyframe(RUN, glm::vec2(15 / h_frames, 0.f));
    sprite->addKeyframe(RUN, glm::vec2(16 / h_frames, 0.f));
    sprite->addKeyframe(RUN, glm::vec2(17 / h_frames, 0.f));
    sprite->addKeyframe(RUN, glm::vec2(18 / h_frames, 0.f));
    sprite->addKeyframe(RUN, glm::vec2(19 / h_frames, 0.f));

    sprite->setAnimationSpeed(RUN_TO_STAND, 8);
    sprite->setAnimationLoop(RUN_TO_STAND, false);
    sprite->addKeyframe(RUN_TO_STAND, glm::vec2(20 / h_frames, 0.f));
    sprite->addKeyframe(RUN_TO_STAND, glm::vec2(21 / h_frames, 0.f));

    sprite->setAnimationSpeed(HANG, 8);
    sprite->addKeyframe(HANG, glm::vec2(22 / h_frames, 0.f));

    sprite->changeAnimation(STAND);
    sprite->setPosition(position);
    sprite->setFlip(false, inverted);

    // PHYSICS: BODY CREATION

    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(position.x, position.y);
    physic_body = physics->CreateBody(&body_def);
    // PHYSICS: SHAPE SETUP (CAPSULE: 2 circles and 1 rectangle)
    const glm::vec2 capsule_bb_size_meters = glm::vec2(18, 43) * Constants::Units::meters_per_pixel;
    float radius = glm::min(capsule_bb_size_meters.x, capsule_bb_size_meters.y) / 2.f;
    b2CircleShape c1;
    c1.m_radius = radius;
    float c1_y = capsule_bb_size_meters.y / 2.f - radius;
    c1.m_p.Set(0, c1_y);
    b2CircleShape c2;
    c2.m_radius = radius;
    float c2_y = -capsule_bb_size_meters.y / 2.f + radius;
    c2.m_p.Set(0, c2_y);
    b2PolygonShape box;
    box.SetAsBox(capsule_bb_size_meters.x / 2.f, glm::abs(c1_y - c2_y) / 2.f, b2Vec2(0, 0), 0.f);

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
        sprite->setFlip(true, inverted);
        if (sprite->animation() != RUN && !jumping) {
            sprite->changeAnimation(RUN);
        }
        h_impulse += glm::vec2(-impulse, 0);
    }
    if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) {
        sprite->setFlip(false, inverted);
        if (sprite->animation() != RUN && !jumping) {
            sprite->changeAnimation(RUN);
        }
        h_impulse += glm::vec2(impulse, 0);
    }
    physic_body->ApplyLinearImpulseToCenter(to_box2d(h_impulse), true);

    if (Game::instance().getSpecialKey(GLUT_KEY_UP)) {
        float max_jump = Constants::Regular::max_jump_speed * Constants::Units::meters_per_pixel;
        float vvchange = max_jump - glm::abs(velocity.y);
        float impulse = physic_body->GetMass() * vvchange;
        if (!jumping) {
            sprite->changeAnimation(JUMP_UP);
            physic_body->ApplyLinearImpulseToCenter(b2Vec2(0, -gravity_direction_y * impulse), true);
            jumping = true;
        }
    }
    float min_fall_speed = Constants::Regular::max_jump_speed * Constants::Units::meters_per_pixel / 5.f;
    bool falling = velocity.y * gravity_direction_y - min_fall_speed > 0;
    if (falling && sprite->animation() != FALL) {
        sprite->changeAnimation(FALL);
    }
    if (!falling && sprite->animation() == FALL) {
        sprite->changeAnimation(STAND);
    }
    float max_stand_speed = Constants::Regular::max_movement_speed * Constants::Units::meters_per_pixel / 5.f;
    bool standing = glm::abs(velocity.x) - max_stand_speed < 0;
    if (!falling && !jumping && standing && sprite->animation() != STAND) {
        sprite->changeAnimation(STAND);
    }

    // gravity
    auto gravity = Constants::Physics::gravity * Constants::Units::meters_per_pixel * gravity_direction_y;
    physic_body->ApplyForceToCenter(to_box2d(gravity * physic_body->GetMass()), true);
    sprite->update(deltaTime);
}


void Player::end_overlap(b2Contact* contact) {
    Object::uuid_t id1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    Object::uuid_t id2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
    if (uuid == id1 || uuid == id2) {
        --contacts;
    }
}

void Player::begin_overlap(b2Contact* contact) {
    Object::uuid_t id1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    Object::uuid_t id2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;

    //Game::instance().get_scene().get_object(id2); might be useful with dynamic_cast ;D
    if (uuid == id1 || uuid == id2) {
        ++contacts;
        if (jumping) {
            b2WorldManifold b;
            contact->GetWorldManifold(&b);
            auto normal = glm::vec2(b.normal.x, b.normal.y);
            float gravity_direction_y = (inverted) ? -1 : 1;
            auto dot = glm::dot(normal, glm::vec2(0.f, 1.f)) * -gravity_direction_y;
            jumping = dot < 0.9;
        }
    }
}
