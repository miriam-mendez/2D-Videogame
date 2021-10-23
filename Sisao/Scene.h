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
#include "Quad.h"


class Scene {

public:
    Scene();
    ~Scene();
    void init(std::string level);
    void update(int deltaTime);
    void render();

    Object* get_object(Object::uuid_t id);
    Camera& get_camera();

private:
    void read_level(std::ifstream& stream);
    void read_general_settings(std::ifstream& stream);
    void read_objects(std::ifstream& stream);
    void setup_shader(ShaderProgram& shader,
                      std::string const& vs, std::string const& fs);

private:
    ShaderProgram texProgram;
    ShaderProgram waterProgram;
    float currentTime;

    TileMap* map = nullptr;
    Quad* water = nullptr;
    Camera camera;
    std::unordered_map<Object::uuid_t, Object*> objects;
    b2World* physics = nullptr;
    PhysicsListener<Object> physics_listener;
};

