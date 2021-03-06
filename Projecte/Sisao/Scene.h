#pragma once

#include <glm/glm.hpp>
#include <box2d/b2_world.h>
#include <map>
#include <fstream>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Object.h"
#include "PhysicsListener.h"
#include "Camera.h"
#include "Water.h"
#include "Parallax.h"

class Scene {

public:
    Scene();
    ~Scene();
    void init(std::string level);
    void update(int deltaTime);
    void render();

    Object* get_object(Object::uuid_t id);
    Camera& get_camera();
    float get_current_time() const { return currentTime; }

    int captured_flags = 0;
    int required_flags = 1;
    const int debug_key_delay = 400;
    int debug_key_delay_timer = 0;
    bool god_mode = false;
    bool no_barrier_mode = false;

private:
    const glm::vec2 scene_center = glm::vec2(0, 0);
    void read_level(std::ifstream& stream);
    void read_general_settings(std::ifstream& stream);
    void read_objects(std::ifstream& stream);
    void setup_shader(ShaderProgram& shader,
                      std::string const& vs, std::string const& fs);

    ShaderProgram texProgram;
    ShaderProgram waterProgram;
    ShaderProgram textProgram;
    float currentTime;

    Parallax* background = nullptr;
    TileMap* map = nullptr;
    Camera camera;
    // map because its sorted (rendering) and provides an easy way to get
    // objets by its arbitrary id
    std::map<Object::uuid_t, Object*> objects;
    b2World* physics = nullptr;
    PhysicsListener<Object> physics_listener;
};

