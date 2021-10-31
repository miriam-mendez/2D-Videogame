#include "Water.h"
#include "Game.h"
#include "Utils.h"
#include <box2d/b2_fixture.h>
#include <box2d/b2_edge_shape.h>
#include "Constants.h"
#include "Box.h"

void Water::init(b2World* physics, ShaderProgram& shaderProgram) {
    glm::vec2 ocean_size = glm::vec2(SCREEN_WIDTH * 10, SCREEN_HEIGHT);
    float ocean_length_meters = 0.5f * ocean_size.x * Constants::Units::meters_per_pixel;
    float leeway = 16 * Constants::Units::meters_per_pixel;
    quad = Quad::init(ocean_size, &shaderProgram);

    b2BodyDef body_def;
    body_def.type = b2_staticBody;
    body_def.position.Set(position.x, position.y);
    physic_body = physics->CreateBody(&body_def);

    b2EdgeShape edge_shape1;
    b2Vec2 v00(position.x - ocean_length_meters, leeway);
    b2Vec2 v01(position.x + ocean_length_meters, leeway);
    edge_shape1.SetTwoSided(v00, v01);

    b2EdgeShape edge_shape2;
    b2Vec2 v11(position.x - ocean_length_meters, -leeway);
    b2Vec2 v10(position.x + ocean_length_meters, -leeway);
    edge_shape2.SetTwoSided(v10, v11);

    b2EdgeShape edge_shape3;
    b2Vec2 v20(position.x - ocean_length_meters, 0);
    b2Vec2 v21(position.x + ocean_length_meters, 0);
    edge_shape3.SetTwoSided(v20, v21);

    b2FixtureDef fixture_def1;
    fixture_def1.shape = &edge_shape1;
    fixture_def1.friction = 0.1f;
    fixture_def1.filter.categoryBits = (int)Constants::Physics::Category::AboveWater;
    fixture_def1.filter.maskBits = (int)Constants::Physics::Mask::Regular;
    physic_body->CreateFixture(&fixture_def1);

    b2FixtureDef fixture_def2;
    fixture_def2.shape = &edge_shape2;
    fixture_def2.friction = 0.1f;
    fixture_def2.filter.categoryBits = (int)Constants::Physics::Category::BelowWater;
    fixture_def2.filter.maskBits = (int)Constants::Physics::Mask::Regular;
    physic_body->CreateFixture(&fixture_def2);

    b2FixtureDef fixture_def3;
    fixture_def3.shape = &edge_shape3;
    fixture_def3.filter.categoryBits = (int)Constants::Physics::Category::Wall;
    fixture_def3.filter.maskBits = (int)Constants::Physics::Mask::Regular;
    fixture_def3.isSensor = true;
    physic_body->CreateFixture(&fixture_def3);

    b2BodyUserData data;
    data.pointer = get_id();
    physic_body->GetUserData() = data;

    const glm::ivec2 sprite_size_pixels = glm::ivec2(133.375f,84);
    spritesheet.loadFromFile("images/VFX/water.png", TEXTURE_PIXEL_FORMAT_RGBA);
    quad2 = Sprite::init(sprite_size_pixels, glm::vec2(1 / 8.f, 1), &spritesheet, &shaderProgram);
    auto splash  = static_cast<Sprite*>(quad2);
    float h_frames = 8.f;
    splash->setAnimationSpeed(SPLASH, 24);
    splash->setAnimationLoop(SPLASH, false);
    for (int i = 0; i < h_frames; ++i) {
        splash->addKeyframe(SPLASH, glm::vec2(i / h_frames, 0.f));
    }
    splash->changeAnimation(0);
    
}

void Water::update(int deltaTime) {
    auto splash = static_cast<Sprite*>(quad2);
    quad->set_position(position);
    quad->set_rotation(rotation);

    if (in_range) {
       // b2Contact::GetManifold()
        splash->set_position(position);
    }
    splash->update(deltaTime);
}

void Water::begin_overlap(b2Contact* contact) {
    Object::uuid_t id1 = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    Object::uuid_t id2 = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
    Object::uuid_t self_id = get_id();

    if (valid_contact(id1, id2)) {
        in_range = true;
    }
}

bool Water::valid_contact(Object::uuid_t id1, Object::uuid_t id2) {
    Object::uuid_t self_id = get_id();
    if (self_id == id1 || self_id == id2) {
        Object* a = Game::instance().get_scene().get_object(id1);
        Object* b = Game::instance().get_scene().get_object(id2);
        Box* x = dynamic_cast<Box*>(a);
        Box* y = dynamic_cast<Box*>(b);
        return (x != nullptr || y != nullptr);
    }
    return false;
}

void Water::render() {
    auto time = Game::instance().get_scene().get_current_time();
    auto shader = quad->expose_shader();
    shader->use();
    shader->setUniform1f("time", time);
    shader->setUniform4f("color", 0.2f, 0.27f, 0.37f, 0.6f);
    quad->render();
}
