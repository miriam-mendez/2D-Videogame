#include "Lever.h"
#include "Utils.h"
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include "Constants.h"
#include "Game.h"
#include "Activable.h"
#include "Player.h"
#include <GL/glew.h>
#include <GL/glut.h>

void Lever::init(b2World* physics, ShaderProgram& shaderProgram, int orientation,
                 bool active, std::vector<Object::uuid_t> const& linked) {
    links = linked;
    const glm::ivec2 sprite_size_pixels = glm::ivec2(32, 32);
    spritesheet.loadFromFile("images/blocks.png", TEXTURE_PIXEL_FORMAT_RGBA);
    sprite = Sprite::init(sprite_size_pixels, glm::vec2(1 / 6.f, 1), &spritesheet, &shaderProgram);
    sprite->setAnimationSpeed(ACTIVATE, 8);
    sprite->setAnimationLoop(ACTIVATE, false);
    sprite->addKeyframe(ACTIVATE, glm::vec2(1.f / 6.f, 0.f));
    sprite->addKeyframe(ACTIVATE, glm::vec2(2.f / 6.f, 0.f));
    sprite->addKeyframe(ACTIVATE, glm::vec2(3.f / 6.f, 0.f));

    sprite->setAnimationSpeed(DEACTIVATE, 8);
    sprite->setAnimationLoop(DEACTIVATE, false);
    sprite->addKeyframe(DEACTIVATE, glm::vec2(3.f / 6.f, 0.f));
    sprite->addKeyframe(DEACTIVATE, glm::vec2(2.f / 6.f, 0.f));
    sprite->addKeyframe(DEACTIVATE, glm::vec2(1.f / 6.f, 0.f));

    auto default_state = (active) ? ACTIVATE : DEACTIVATE;
    sprite->changeAnimation(default_state, 2);

    sprite->setPosition(position);

    b2BodyDef body_def;
    body_def.type = b2_staticBody;
    body_def.position.Set(position.x, position.y);
    physic_body = physics->CreateBody(&body_def);

    // radius not divided by 2 intentionally
    float radius_meters = sprite_size_pixels.y * Constants::Units::meters_per_pixel;
    b2CircleShape c1;
    c1.m_radius = radius_meters;
    c1.m_p.Set(0, 0);
    b2FixtureDef fixture_def;
    fixture_def.shape = &c1;
    fixture_def.filter.categoryBits = (int)Constants::Physics::Category::Sensor;
    fixture_def.filter.maskBits = (int)Constants::Physics::Mask::Sensor;
    fixture_def.isSensor = true;
    physic_body->CreateFixture(&fixture_def);

    b2BodyUserData data;
    data.pointer = get_id();
    physic_body->GetUserData() = data;
}

void Lever::update(int deltaTime) {
    if (requires_sync_with_linked) {
        requires_sync_with_linked = false;
        send_to_linked(sprite->animation() == ACTIVATE);
    }
    current_delay -= deltaTime;
    if (in_range && current_delay < 0 && Game::instance().getSpecialKey(GLUT_KEY_DOWN)) {
        current_delay = switch_delay;
        if (sprite->animation() == ACTIVATE) {
            sprite->changeAnimation(DEACTIVATE);
            send_to_linked(false);
        }
        else {
            sprite->changeAnimation(ACTIVATE);
            send_to_linked(true);
        }
    }
    sprite->update(deltaTime);
}

void Lever::begin_overlap(b2Contact* contact) {
    Object::uuid_t id1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    Object::uuid_t id2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
    if (valid_contact(id1, id2)) in_range = true;
}

void Lever::end_overlap(b2Contact* contact) {
    Object::uuid_t id1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    Object::uuid_t id2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
    if (valid_contact(id1, id2)) in_range = false;
}

bool Lever::valid_contact(Object::uuid_t id1, Object::uuid_t id2) {
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

void Lever::send_to_linked(bool activated) {
    for (auto const id : links) {
        Object* obj = Game::instance().get_scene().get_object(id);
        Activable* x = dynamic_cast<Activable*>(obj);
        if (x) {
            x->set(activated);
        }
    }
}
