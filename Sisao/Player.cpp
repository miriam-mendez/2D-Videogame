#include "Player.h"
#include "Utils.h"
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include "Game.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include "Constants.h"
#include "Water.h"

void Player::init(b2World* physics, ShaderProgram& shaderProgram, bool inverse) {
    inverted = inverse;
    const glm::ivec2 sprite_size_pixels = glm::ivec2(31, 43);

    spritesheet.loadFromFile("images/player.png", TEXTURE_PIXEL_FORMAT_RGBA);
    float h_frames = 22.f;
    quad = Sprite::init(sprite_size_pixels, glm::vec2(1 / h_frames, 1.f), &spritesheet, &shaderProgram);

    auto sprite = static_cast<Sprite*>(quad);
    sprite->setAnimationSpeed(JUMP_UP, 8);
    sprite->addKeyframe(JUMP_UP, glm::vec2(0.f, 0.f));

    sprite->setAnimationSpeed(FALL, 6);
    sprite->setAnimationLoop(FALL, false);
    sprite->addKeyframe(FALL, glm::vec2(1 / h_frames, 0.f));
    sprite->addKeyframe(FALL, glm::vec2(2 / h_frames, 0.f));

    sprite->setAnimationSpeed(FALL_TO_STAND, 7);
    sprite->setAnimationLoop(FALL_TO_STAND, false);
    sprite->setAnimationNext(FALL_TO_STAND, STAND);
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

    sprite->setAnimationSpeed(RUN_TO_STAND, 11);
    sprite->setAnimationLoop(RUN_TO_STAND, false);
    sprite->setAnimationNext(RUN_TO_STAND, STAND);
    sprite->addKeyframe(RUN_TO_STAND, glm::vec2(20 / h_frames, 0.f));
    sprite->addKeyframe(RUN_TO_STAND, glm::vec2(21 / h_frames, 0.f));

    sprite->changeAnimation(STAND);
    sprite->set_position(position);
    sprite->set_flip(false, inverted);

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
    fixture_box.friction = 0.f;
    fixture_box.filter.categoryBits = (int)Constants::Physics::Category::Regular;
    fixture_box.filter.maskBits = (int)Constants::Physics::Mask::Regular;
    physic_body->CreateFixture(&fixture_box);
    physic_body->SetFixedRotation(true);

    b2BodyUserData data;
    data.pointer = get_id();
    physic_body->GetUserData() = data;
}

void Player::update(int deltaTime) {
    auto sprite = static_cast<Sprite*>(quad);
    physics_update(deltaTime);
    b2Vec2 velocity = physic_body->GetLinearVelocity();
    float gravity_direction_y = (inverted) ? -1.f : 1.f;
    float min_fall_speed = Constants::Regular::max_jump_speed * Constants::Units::meters_per_pixel / 5.f;
    falling = velocity.y * gravity_direction_y - min_fall_speed > 0;
    float max_stand_speed = Constants::Regular::max_movement_speed * Constants::Units::meters_per_pixel / 5.f;
    standing = glm::abs(velocity.x) - max_stand_speed < 0;
    float max_xspeed_meters = Constants::Regular::max_movement_speed * Constants::Units::meters_per_pixel;
    float velocity_left = (velocity.x < 0.f) ? -velocity.x : 0.f;
    float velocity_right = (velocity.x > 0.f) ? velocity.x : 0.f;
    float impulse_right = physic_body->GetMass() * (max_xspeed_meters - velocity_right);
    float impulse_left = physic_body->GetMass() * (max_xspeed_meters - velocity_left);
    auto h_impulse = glm::vec2(0, 0);

    if (Game::instance().getSpecialKey(GLUT_KEY_LEFT)) {
        sprite->set_flip(true, inverted);
        if (sprite->animation() != RUN && !jumping && !falling && !standing) {
            sprite->changeAnimation(RUN);
        }
        h_impulse += glm::vec2(-impulse_left, 0);
    }
    if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) {
        sprite->set_flip(false, inverted);
        if (sprite->animation() != RUN && !jumping && !falling && !standing) {
            sprite->changeAnimation(RUN);
        }
        h_impulse += glm::vec2(impulse_right, 0);
    }
    physic_body->ApplyLinearImpulseToCenter(to_box2d(h_impulse), true);

    if (Game::instance().getSpecialKey(GLUT_KEY_UP)) {
        float max_jump = Constants::Regular::max_jump_speed * Constants::Units::meters_per_pixel;
        float vvchange = max_jump - glm::abs(velocity.y);
        float impulse = physic_body->GetMass() * vvchange;
        if (!jumping && !falling) {
            sprite->changeAnimation(JUMP_UP);
            physic_body->ApplyLinearImpulseToCenter(b2Vec2(0, -gravity_direction_y * impulse), true);
            jumping = true;
        }
    }
    if (falling && sprite->animation() != FALL) {
        sprite->changeAnimation(FALL);
    }
    if (!falling && standing && sprite->animation() != STAND && sprite->animation() != FALL_TO_STAND && sprite->animation() != RUN_TO_STAND) {
        if (sprite->animation() == FALL) {
            sprite->changeAnimation(FALL_TO_STAND);
            jumping = false;
        }
        else if (!jumping && sprite->animation() == RUN) {
            sprite->changeAnimation(RUN_TO_STAND);
        }
        else if (!jumping) {
            sprite->changeAnimation(STAND);
        }
    }
    if (in_water) {
        Game::instance().delayed_set_level(Game::instance().get_current_level());
    }

    // gravity
    auto gravity = Constants::Physics::gravity * Constants::Units::meters_per_pixel * gravity_direction_y;
    physic_body->ApplyForceToCenter(to_box2d(gravity * physic_body->GetMass()), true);
    sprite->update(deltaTime);
}


void Player::begin_overlap(b2Contact* contact) {
    Object::uuid_t id1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    Object::uuid_t id2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
    Object::uuid_t self_id = get_id();

    int category = contact->GetFixtureA()->GetFilterData().categoryBits;
    category = category | contact->GetFixtureB()->GetFilterData().categoryBits;
    bool is_sensor = (category & (int)Constants::Physics::Category::Sensor) != 0;

    if (self_id == id1 || self_id == id2) {
        b2WorldManifold b;
        contact->GetWorldManifold(&b);
        auto normal = glm::vec2(b.normal.x, b.normal.y);
        float gravity_direction_y = (inverted) ? -1.f : 1.f;
        auto dot = glm::dot(normal, glm::vec2(0.f, 1.f)) * -gravity_direction_y;

        if (!is_sensor)
            jumping = jumping && dot < 0.9;

        auto obj1 = Game::instance().get_scene().get_object(id1);
        auto obj2 = Game::instance().get_scene().get_object(id2);
        auto w0 = dynamic_cast<Water*>(obj1);
        auto w1 = dynamic_cast<Water*>(obj2);
        if (w0 != nullptr || w1 != nullptr) {
            in_water = true;
        }
    }
}
