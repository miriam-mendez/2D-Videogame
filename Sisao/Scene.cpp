#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include "Camera.h"
#include "Player.h"
#include "Box.h"
#include "Constants.h"
#include <fstream>
#include <sstream>
#include "Cactus.h"
#include "Wall.h"

Scene::Scene() {}

Scene::~Scene() {
    delete map;
    for (auto const& x : objects) {
        camera.unfollow(x.second);
        delete x.second;
    }
    objects.clear();
    delete water;
    physics_listener.release();
    delete physics;
}



void Scene::init(std::string level) {
    setup_shader(texProgram, "shaders/texture.vert", "shaders/texture.frag");
    setup_shader(waterProgram, "shaders/water.vert", "shaders/water.frag");
    currentTime = 0.0f;

    physics = new b2World(b2Vec2(0.0f, 0.0f));
    physics->SetContactListener(&physics_listener);

    std::ifstream stream;
    stream.open(level.c_str());
    read_level(stream);
    stream.close();

    camera.init(physics, 1.f, false, true);
    camera.set_orthogonal(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
    camera.set_position(map->get_center());

    glm::vec2 ocean_size = glm::vec2(SCREEN_WIDTH * 99, SCREEN_HEIGHT);
    glm::vec2 ocean_pos = map->get_center();
    ocean_pos.y += ocean_size.y / 2;
    water = Quad::init(ocean_size, &waterProgram);
    water->setPosition(ocean_pos);
}

void Scene::update(int deltaTime) {
    currentTime += deltaTime;
    for (auto const& x : objects) {
        x.second->update(deltaTime);
    }
    camera.update(deltaTime);
    physics->Step(Constants::Physics::timestep, Constants::Physics::velocity_iters, Constants::Physics::position_iters);
}

void Scene::render() {
    map->render();
    for (auto const& x : objects) {
        x.second->render();
    }
    // required to inject extra uniforms
    water->expose_shader()->use();
    waterProgram.setUniform1f("time", currentTime);
    waterProgram.setUniform4f("color", 0.23f, 0.7f, 1.0f, 0.5f);
    water->render();
}

Object* Scene::get_object(Object::uuid_t id) {
    auto search = objects.find(id);
    return (search != objects.end()) ? search->second : nullptr;
}

Camera& Scene::get_camera() {
    return camera;
}

void Scene::read_level(std::ifstream& stream) {
    std::string line;
    while (std::getline(stream, line)) {
        if (line.find("[BEGIN GENERAL]") != string::npos) {
            read_general_settings(stream);
        }
        else if (line.find("[BEGIN TILEMAP]") != string::npos) {
            delete map;
            map = new TileMap(stream, physics, glm::vec2(0, 0), texProgram);
        }
        else if (line.find("[BEGIN OBJECTS]") != string::npos) {
            read_objects(stream);
        }
    }
}

void Scene::read_general_settings(std::ifstream& stream) {
    std::string line;
    std::getline(stream, line);
    while (line.find("[END GENERAL]") == string::npos) {
        auto instr_end = line.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ_");
        instr_end = (instr_end == string::npos) ? 0 : instr_end;
        auto instr = line.substr(0, instr_end);
        auto args = line.substr(instr_end);
        std::stringstream sstream;
        if (instr == "NEXT_LEVEL") {
            std::string level;
            sstream.str(args);
            sstream >> level;
            Game::instance().next_level = level;
        }
        else if (instr == "PREV_LEVEL") {
            std::string level;
            sstream.str(args);
            sstream >> level;
            Game::instance().prev_level = level;
        }
        else if (instr == "MUSIC") {
        }
        else if (instr == "BACKGROUND") {
        }
        std::getline(stream, line);
    }
}

void Scene::read_objects(std::ifstream& stream) {
    std::string line;
    std::getline(stream, line);
    while (line.find("[END OBJECTS]") == string::npos) {
        auto instr_end = line.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ_");
        instr_end = (instr_end == string::npos) ? 0 : instr_end;
        auto instr = line.substr(0, instr_end);
        auto args = line.substr(instr_end);
        std::stringstream sstream;
        if (instr == "PLAYER") {
            glm::ivec2 pos;
            bool inverted;
            Object::uuid_t id;
            sstream.str(args);
            sstream >> id >> pos.x >> pos.y >> inverted;
            Player* player = new Player(id);
            player->init(physics, texProgram, inverted);
            player->set_position(glm::vec2(pos));
            auto r = objects.emplace(id, player);
            camera.follow(player);
            assert(r.second);
        }
        else if (instr == "BOX") {
            glm::ivec2 pos;
            bool inverted;
            Object::uuid_t id;
            sstream.str(args);
            sstream >> id >> pos.x >> pos.y >> inverted;
            auto box = new Box(id);
            box->init(physics, texProgram, inverted);
            box->set_position(glm::vec2(pos));
            auto r = objects.emplace(id, box);
            assert(r.second);
        }
        else if (instr == "WALL") {
            glm::ivec2 pos;
            bool inverted;
            Object::uuid_t id;
            sstream.str(args);
            sstream >> id >> pos.x >> pos.y >> inverted;
            auto wall = new Wall(id);
            wall->init(physics, texProgram, inverted);
            wall->set_position(glm::vec2(pos));
            auto r = objects.emplace(id, wall);
            assert(r.second);
        }
        else if (instr == "CACTUS") {
            glm::ivec2 pos;
            Object::uuid_t id;
            int orientation;
            sstream.str(args);
            sstream >> id >> pos.x >> pos.y >> orientation;
            auto box = new Cactus(id);
            box->init(physics, texProgram, orientation);
            box->set_position(glm::vec2(pos));
            auto r = objects.emplace(id, box);
            assert(r.second);
        }
        std::getline(stream, line);
    }
}

void Scene::setup_shader(ShaderProgram& shader, std::string const& vs, std::string const& fs) {
    Shader vShader, fShader;
    vShader.initFromFile(VERTEX_SHADER, vs);
    assert(vShader.isCompiled()); // Vertex Shader Error see vShader.log()
    fShader.initFromFile(FRAGMENT_SHADER, fs);
    assert(fShader.isCompiled()); // Fragment Shader Error see fShader.log()
    shader.init();
    shader.addShader(vShader);
    shader.addShader(fShader);
    shader.link();
    assert(shader.isLinked()); // Shader Linking Error see shader.log()
    shader.bindFragmentOutput("outColor");
    vShader.release();
    fShader.release();
}




