#include "Planck.h"
#include "Utils.h"
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_revolute_joint.h>
#include "Constants.h"
#include "Sprite.h"

void Planck::init(b2World* physics, ShaderProgram& shaderProgram, bool inverted) {

    // PLANCK BASE
    const glm::ivec2 sprite_size_pixels = glm::ivec2(32, 32);
    spritesheet.loadFromFile("images/new.png", TEXTURE_PIXEL_FORMAT_RGBA);
    quad = Sprite::init(sprite_size_pixels, glm::vec2(1 / 4.f, 1.f), &spritesheet, &shaderProgram);
    auto sprite = static_cast<Sprite*>(quad);
    sprite->addKeyframe(0, glm::vec2(0.75f, 0.f));
    sprite->changeAnimation(0);
    sprite->set_position(position);
    sprite->set_flip(false, inverted);

    b2BodyDef bodyBase;
    bodyBase.type = b2_staticBody;
    bodyBase.position.Set(position.x, position.y);
    auto m_bodyA = physics->CreateBody(&bodyBase);

    const glm::vec2 sprite_size_meters = glm::vec2(sprite_size_pixels) * Constants::Units::meters_per_pixel;
    b2PolygonShape polygA;

    b2Vec2 vertices[3]; //set each vertex of polygon in an array
    vertices[0].Set(sprite_size_meters.x / 2.f, sprite_size_meters.y / 2.f);
    vertices[1].Set(0.f, 0.f);
    vertices[2].Set(sprite_size_meters.x, 0.f);

    polygA.Set(vertices, 3); //pass array to the shape

    b2FixtureDef fixture_defA;
    fixture_defA.shape = &polygA;
    fixture_defA.filter.categoryBits = (int)Constants::Physics::Category::Sensor;
    fixture_defA.filter.maskBits = (int)Constants::Physics::Mask::Sensor;
    fixture_defA.isSensor = true;
    m_bodyA->CreateFixture(&fixture_defA);

    // PLANCK BEAM
    const glm::ivec2 sprite_size_pixels = glm::ivec2(96, 8);
    spritesheet.loadFromFile("images/bar.png", TEXTURE_PIXEL_FORMAT_RGBA);
    quad = Sprite::init(sprite_size_pixels, glm::vec2(1.f, 1.f), &spritesheet, &shaderProgram);
    auto sprite = static_cast<Sprite*>(quad);
    sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
    sprite->changeAnimation(0);
    sprite->set_position(position);

    b2BodyDef bodyBeam;
    bodyBeam.type = b2_dynamicBody;
    bodyBeam.position.Set(position.x, position.y);
    auto m_bodyB = physics->CreateBody(&bodyBeam);

    const glm::vec2 sprite_size_meters = glm::vec2(sprite_size_pixels) * Constants::Units::meters_per_pixel;
    b2PolygonShape polygB;
    polygB.SetAsBox(sprite_size_meters.x / 2, sprite_size_meters.y / 2);

    b2FixtureDef fixture_defB;
    fixture_defB.shape = &polygB;
    fixture_defB.filter.categoryBits = (int)Constants::Physics::Category::Sensor;
    fixture_defB.filter.maskBits = (int)Constants::Physics::Mask::Sensor;
    fixture_defB.isSensor = true;
    m_bodyB->CreateFixture(&fixture_defB);

	
    // JOINT
	b2RevoluteJointDef revoluteJointDef;
    revoluteJointDef.bodyA = m_bodyA;
    revoluteJointDef.bodyB =m_bodyB;
    revoluteJointDef.collideConnected = false;
    revoluteJointDef.localAnchorA.Set(sprite_size_meters.x/2, sprite_size_meters.y/2);//the top of the base
    revoluteJointDef.localAnchorB.Set(sprite_size_meters.x/2, sprite_size_meters.y/2);//center of the beam
    auto m_joint = (b2RevoluteJoint*)physics->CreateJoint(&revoluteJointDef);
}

void Planck::update(int deltaTime) {

}