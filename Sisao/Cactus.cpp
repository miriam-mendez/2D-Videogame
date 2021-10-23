#include "Cactus.h"
#include "Utils.h"
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include "Constants.h"
#include "Game.h"
#include "Player.h"



void Cactus::update(int deltaTime) {}

void Cactus::init(b2World* physics, ShaderProgram& shaderProgram, int orientation) {
    const glm::ivec2 sprite_size_pixels = glm::ivec2(32, 32);
    spritesheet.loadFromFile("images/tilesblock2.png", TEXTURE_PIXEL_FORMAT_RGBA);
    sprite = Sprite::init(sprite_size_pixels, glm::vec2(1 / 8.f, 1.f), &spritesheet, &shaderProgram);
    sprite->addKeyframe(0, glm::vec2(6.f / 8.f, 0.f));
    sprite->changeAnimation(0);
    sprite->setPosition(position);
    auto rotation = glm::radians(90.f) * (orientation % 4);
    sprite->setRotation(rotation);

    b2BodyDef body_def;
    body_def.type = b2_staticBody;
    body_def.position.Set(position.x, position.y);
    physic_body = physics->CreateBody(&body_def);

    float radius_meters = 0.5f * sprite_size_pixels.y * Constants::Units::meters_per_pixel;
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


void Cactus::begin_overlap(b2Contact* contact) {
    Object::uuid_t id1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    Object::uuid_t id2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
    Object::uuid_t self_id = get_id();

    if (self_id == id1 || self_id == id2) {
        Object* a = Game::instance().get_scene().get_object(id1);
        Object* b = Game::instance().get_scene().get_object(id2);

        Player* x = dynamic_cast<Player*>(a);
        Player* y = dynamic_cast<Player*>(b);

        if (x != nullptr || y != nullptr) {
            Game::instance().delayed_set_level(Game::instance().get_current_level());
        }
    }
}