#include "Box.h"
#include "utils.h"
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include "Constants.h"

void Box::init(b2World* physics, ShaderProgram& shaderProgram) {
    const glm::ivec2 sprite_size_pixels = glm::ivec2(32, 32);
    spritesheet.loadFromFile("images/tilesblock.png", TEXTURE_PIXEL_FORMAT_RGBA);
    sprite = Sprite::createSprite(sprite_size_pixels, glm::vec2(1 / 5.f, 1), &spritesheet, &shaderProgram);
    sprite->setPosition(position);


    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(position.x, position.y);
    physical_body = physics->CreateBody(&body_def);

    const glm::vec2 sprite_size_meters = glm::vec2(sprite_size_pixels) * Constants::Units::meters_per_pixel;
    b2PolygonShape box_shape;
    box_shape.SetAsBox(sprite_size_meters.x / 2.f, sprite_size_meters.y / 2.f);
    b2FixtureDef fixture_def;
    fixture_def.shape = &box_shape;
    fixture_def.density = 1.0f;
    fixture_def.friction = 0.3f;
    physical_body->CreateFixture(&fixture_def);
}
