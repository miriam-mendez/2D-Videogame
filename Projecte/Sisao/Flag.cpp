#include "Flag.h"
#include "Utils.h"
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include "Constants.h"
#include "Game.h"
#include "Player.h"

void Flag::init(b2World* physics, ShaderProgram& shaderProgram, bool inverted,
                std::string const& level) {
    this->inverted = inverted;
    this->level = level;
    const glm::ivec2 sprite_size_pixels = glm::ivec2(32, 64);
    spritesheet.loadFromFile("images/flag.png", TEXTURE_PIXEL_FORMAT_RGBA);
    quad = Sprite::init(sprite_size_pixels, glm::vec2(1 / 14.f, 1), &spritesheet, &shaderProgram);
    auto sprite = static_cast<Sprite*>(quad);
    sprite->setAnimationSpeed(FALL, 32);
    sprite->setAnimationLoop(FALL, false);
    for (int i = 0; i < 14; ++i) {
        sprite->addKeyframe(FALL, glm::vec2(i / 14.f, 0.f));
    }

    sprite->setAnimationSpeed(RISE, 32);
    sprite->setAnimationLoop(RISE, false);
    for (int i = 13; i >= 0; --i) {
        sprite->addKeyframe(RISE, glm::vec2(i / 14.f, 0.f));
    }

    sprite->changeAnimation(FALL, 13);
    sprite->set_position(position);
    sprite->set_flip(false, inverted);

    auto& sounds = Game::instance().get_sound_system();
    sounds.addNewSound("sounds/SFX/congrats.wav", "flag", "flag", false);
    sounds.addNewSound("sounds/SFX/congrats_part.wav", "flag_part", "flag_part", false);

    b2BodyDef body_def;
    body_def.type = b2_staticBody;
    body_def.position.Set(position.x, position.y);
    physic_body = physics->CreateBody(&body_def);

    const glm::vec2 sprite_size_meters = glm::vec2(sprite_size_pixels) * Constants::Units::meters_per_pixel;
    b2PolygonShape box_shape;
    auto offset = b2Vec2(-sprite_size_meters.x / 2.f, 0);
    box_shape.SetAsBox(0.7f * sprite_size_meters.x, sprite_size_meters.y, offset, 0.f);
    b2FixtureDef fixture_def;
    fixture_def.shape = &box_shape;
    fixture_def.filter.categoryBits = (int)Constants::Physics::Category::Sensor;
    fixture_def.filter.maskBits = (int)Constants::Physics::Mask::Sensor;
    fixture_def.isSensor = true;
    physic_body->CreateFixture(&fixture_def);

    b2BodyUserData data;
    data.pointer = get_id();
    physic_body->GetUserData() = data;
}

void Flag::update(int deltaTime) {
    auto sprite = static_cast<Sprite*>(quad);
    auto& sounds = Game::instance().get_sound_system();
    remaining_time = in_range ? remaining_time - deltaTime : timer;
    part_sound_played = in_range ? part_sound_played : false;
    if (in_range && sprite->animation() != RISE) {
        sprite->changeAnimation(RISE);
    }
    else if (!in_range && sprite->animation() != FALL) {
        sprite->changeAnimation(FALL);
    }
    if (remaining_time <= 0) {
        if (!part_sound_played) {
            sounds.playSound("flag_part");
            part_sound_played = true;
        }
        auto& scene = Game::instance().get_scene();
        if (scene.captured_flags >= scene.required_flags) {
            sounds.playSound("flag");
            Game::instance().delayed_set_level(level);
        }
    }
    quad->update(deltaTime);
}

void Flag::begin_overlap(b2Contact* contact) {
    Object::uuid_t id1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    Object::uuid_t id2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
    if (valid_contact(id1, id2) && !in_range) {
        ++Game::instance().get_scene().captured_flags;
        in_range = true;
    }
}

void Flag::end_overlap(b2Contact* contact) {
    Object::uuid_t id1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    Object::uuid_t id2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
    if (valid_contact(id1, id2) && in_range) {
        --Game::instance().get_scene().captured_flags;
        in_range = false;
    }
}

bool Flag::valid_contact(Object::uuid_t id1, Object::uuid_t id2) {
    Object::uuid_t self_id = get_id();
    if (self_id == id1 || self_id == id2) {
        Object* a = Game::instance().get_scene().get_object(id1);
        Object* b = Game::instance().get_scene().get_object(id2);
        Player* x = dynamic_cast<Player*>(a);
        Player* y = dynamic_cast<Player*>(b);
        return (x != nullptr || y != nullptr);
    }
    return false;
}
