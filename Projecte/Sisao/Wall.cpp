#include "Wall.h"
#include "Utils.h"
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include "Constants.h"
#include "Sprite.h"

void Wall::init(b2World* physics, ShaderProgram& shaderProgram, bool inverted) {
    this->inverted = inverted;
    const glm::ivec2 sprite_size_pixels = glm::ivec2(32, 32);
    spritesheet.loadFromFile("images/blocks.png", TEXTURE_PIXEL_FORMAT_RGBA);
    quad = Sprite::init(sprite_size_pixels, glm::vec2(1 / 6.f, 1), &spritesheet, &shaderProgram);
    auto sprite = static_cast<Sprite*>(quad);
    sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
    sprite->changeAnimation(0);
    sprite->set_position(position);
    sprite->set_flip(false, inverted);

    b2BodyDef body_def;
    body_def.type = b2_staticBody;
    body_def.position.Set(position.x, position.y);
    physic_body = physics->CreateBody(&body_def);

    const glm::vec2 sprite_size_meters = glm::vec2(sprite_size_pixels) * Constants::Units::meters_per_pixel;
    b2PolygonShape box_shape;
    box_shape.SetAsBox(sprite_size_meters.x / 2.f, sprite_size_meters.y / 2.f);
    b2FixtureDef fixture_def;
    fixture_def.shape = &box_shape;
    fixture_def.friction = 0.4f;
    fixture_def.filter.categoryBits = (int)Constants::Physics::Category::Wall;
    fixture_def.filter.maskBits = (int)Constants::Physics::Mask::Wall;
    physic_body->CreateFixture(&fixture_def);

    b2BodyUserData data;
    data.pointer = get_id();
    physic_body->GetUserData() = data;
}

void Wall::update(int deltaTime) {}

void Wall::render() {
    if (physic_body->IsEnabled()) {
        auto sprite = static_cast<Sprite*>(quad);
        sprite->render();
    }
}

void Wall::set(bool active) {
    physic_body->SetEnabled(active);
}
