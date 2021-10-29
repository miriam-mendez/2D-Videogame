#include "Box.h"
#include "Utils.h"
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include "Constants.h"
#include "Sprite.h"
#include "Game.h"
#include "Water.h"

void Box::init(b2World* physics, ShaderProgram& shaderProgram, bool inverted) {
    this->inverted = inverted;
    const glm::ivec2 sprite_size_pixels = glm::ivec2(32, 32);
    spritesheet.loadFromFile("images/blocks.png", TEXTURE_PIXEL_FORMAT_RGBA);
    quad = Sprite::init(sprite_size_pixels, glm::vec2(1 / 6.f, 1), &spritesheet, &shaderProgram);
    auto sprite = static_cast<Sprite*>(quad);
    sprite->addKeyframe(0, glm::vec2(5.f / 6.f, 0.f));
    sprite->changeAnimation(0);
    sprite->set_position(position);
    sprite->set_flip(false, inverted);

    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(position.x, position.y);
    physic_body = physics->CreateBody(&body_def);

    const glm::vec2 sprite_size_meters = glm::vec2(sprite_size_pixels) * Constants::Units::meters_per_pixel;
    b2PolygonShape box_shape;
    box_shape.SetAsBox(sprite_size_meters.x / 2.f, sprite_size_meters.y / 2.f);
    b2FixtureDef fixture_def;
    fixture_def.shape = &box_shape;
    fixture_def.density = 3.0f;
    fixture_def.friction = 0.4f;
    auto water_mask = inverted ? (int)Constants::Physics::Category::BelowWater : (int)Constants::Physics::Category::AboveWater;
    fixture_def.filter.categoryBits = (int)Constants::Physics::Category::Regular;
    fixture_def.filter.maskBits = (int)Constants::Physics::Mask::Regular | water_mask;
    physic_body->CreateFixture(&fixture_def);

    b2BodyUserData data;
    data.pointer = get_id();
    physic_body->GetUserData() = data;

    auto& sounds = Game::instance().get_sound_system();
    sounds.addNewSound("sounds/SFX/box-water.wav", "box-water", "box-water", false);
    sounds.set_group_volume("box-water", 0.25f);
}

void Box::update(int deltaTime) {
    if (in_water && physic_body->GetMass() < 99) {
        b2MassData md;
        md.center = b2Vec2(0, 0);
        md.I = physic_body->GetInertia();;
        md.mass = 99;
        physic_body->SetMassData(&md);
        auto& sounds = Game::instance().get_sound_system();
        sounds.playSound("box-water");
    }

    physics_update(deltaTime);

    // gravity
    auto gravity = Constants::Physics::gravity * Constants::Units::meters_per_pixel;
    gravity = inverted ? -gravity : (gravity);
    physic_body->ApplyForceToCenter(to_box2d(gravity * physic_body->GetMass()), true);
}


void Box::begin_overlap(b2Contact* contact) {
    Object::uuid_t id1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    Object::uuid_t id2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
    Object::uuid_t self_id = get_id();
    if (self_id == id1 || self_id == id2) {
        auto obj1 = Game::instance().get_scene().get_object(id1);
        auto obj2 = Game::instance().get_scene().get_object(id2);

        auto w0 = dynamic_cast<Water*>(obj1);
        auto w1 = dynamic_cast<Water*>(obj2);

        if (w0 != nullptr || w1 != nullptr) {
            in_water = true;
        }
    }
}