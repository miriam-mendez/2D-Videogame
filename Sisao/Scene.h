#pragma once

#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include <box2d/b2_world.h>
#include <unordered_map>
#include "Object.h"
#include "PhysicsListener.h"
#include <fstream>

// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene {

public:
    Scene();
    ~Scene();

    void init(std::string level);
    void update(int deltaTime);
    void render();

private:
    Object::uuid_t generate_object_uuid();
    void read_level(std::ifstream& stream);
    void read_objects(std::ifstream& stream);
    void initShaders();

private:
    TileMap map;
    std::unordered_map<std::uint32_t, Object*> objects;
    b2World physics = b2World(b2Vec2(0.0f, 0.0f));
    PhysicsListener<Object> physics_listener;

    ShaderProgram texProgram;
    float currentTime;
};

