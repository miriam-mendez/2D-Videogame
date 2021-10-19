#pragma once

#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include <box2d/b2_world.h>
#include <unordered_map>
#include "Object.h"
#include "PhysicsListener.h"
#include <fstream>
#include "Camera.h"


class Scene {
    TileMap map;
    std::unordered_map<Object::uuid_t, Object*> objects;
    b2World physics = b2World(b2Vec2(0.0f, 0.0f));
    PhysicsListener<Object> physics_listener;

    ShaderProgram texProgram;
    float currentTime;
    Camera camera = Camera(Object::undefined_uuid);

    void read_level(std::ifstream& stream);
    void read_objects(std::ifstream& stream);
    void initShaders();
public:
    Scene();
    ~Scene();

    void init(std::string level);
    void update(int deltaTime);
    void render();

    Object* get_object(Object::uuid_t id);
    Camera& get_camera();
};

