#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include "Camera.h"
#include "Player.h"
#include "Box.h"
#include "Constants.h"
#include <sstream>
#include "Cactus.h"
#include "Wall.h"
#include "Lever.h"
#include "Flag.h"
#include "Text.h"

Scene::Scene() {}

Scene::~Scene() {
    auto& sounds = Game::instance().get_sound_system();
    sounds.releaseSound("background");
    delete background;
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
    setup_shader(textProgram, "shaders/text.vert", "shaders/text.frag");
    currentTime = 0.0f;

    physics = new b2World(b2Vec2(0.0f, 0.0f));
    physics->SetContactListener(&physics_listener);

    std::ifstream stream;
    stream.open(level.c_str());
    assert(stream.good());
    read_level(stream);
    stream.close();

    camera.init(physics, 1.f, false, true);
    camera.set_orthogonal(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
    camera.set_position(scene_center);
}

void Scene::update(int deltaTime) {
    currentTime += deltaTime;
    for (auto const& x : objects) {
        x.second->update(deltaTime);
    }
    camera.update(deltaTime);
    if (background) background->update(deltaTime);
    if (water) water->update(deltaTime);
    physics->Step(Constants::Physics::timestep, Constants::Physics::velocity_iters, Constants::Physics::position_iters);
}

void Scene::render() {
    if (background) background->render();
    if (map) map->render();
    if (water) water->render();
    for (auto const& x : objects) {
        x.second->render();
    }
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
            map = new TileMap(stream, physics, texProgram);
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
            if (level != "")
                Game::instance().next_level = level;
        }
        else if (instr == "PREV_LEVEL") {
            std::string level;
            sstream.str(args);
            sstream >> level;
            if (level != "")
                Game::instance().prev_level = level;
        }
        else if (instr == "REQUIRED_FLAGS") {
            int flags;
            sstream.str(args);
            sstream >> flags;
            required_flags = flags;
        }
        if (instr == "WATER") {
            bool w;
            sstream.str(args);
            sstream >> w;
            delete water;
            if (w) {
                water = new Water();
                water->init(physics, waterProgram);
                water->set_position(scene_center);
            }
        }
        else if (instr == "MUSIC") {
            std::string path;
            int v;
            sstream.str(args);
            sstream >> v >> path;
            if (path != "") {
                auto& sounds = Game::instance().get_sound_system();
                sounds.addNewSound(path, "background", "background", true);
                float volume = v / 100.f;
                sounds.set_group_volume("background", volume);
                sounds.playSound("background");
            }
        }
        else if (instr == "BACKGROUND_BASE") {
            std::string path;
            glm::ivec2 pos;
            sstream.str(args);
            sstream >> pos.x >> pos.y >> path;
            delete background;
            background = Parallax::init(path, &texProgram);
            background->set_position(scene_center);
        }
        else if (instr == "BACKGROUND_PARALLAX") {
            assert(background != nullptr);
            float speed;
            std::string path;
            sstream.str(args);
            sstream >> speed >> path;
            background->add_layer(path, speed);
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
        else if (instr == "TEXT") {
            Object::uuid_t id;
            int size;
            glm::vec2 pos;
            glm::vec4 color;
            std::string text_with_spaces;
            sstream.str(args);
            sstream >> id >> pos.x >> pos.y >> size >> color.r >> color.g >> color.b >> color.a;
            std::getline(sstream, text_with_spaces);
            auto t = new Text(id);
            t->init(text_with_spaces, size, color);
            t->set_position(pos);
            auto r = objects.emplace(id, t);
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
        else if (instr == "FLAG") {
            glm::ivec2 pos;
            bool inverted;
            Object::uuid_t id;
            std::string level;
            sstream.str(args);
            sstream >> id >> pos.x >> pos.y >> inverted >> level;
            auto flag = new Flag(id);
            flag->init(physics, texProgram, inverted, level);
            flag->set_position(glm::vec2(pos));
            auto r = objects.emplace(id, flag);
            assert(r.second);
        }
        else if (instr == "LEVER") {
            glm::ivec2 pos;
            int orientation;
            std::vector<Object::uuid_t> linked_ids;
            Object::uuid_t id;
            bool default_state;
            sstream.str(args);
            sstream >> id >> pos.x >> pos.y >> orientation >> default_state;
            Object::uuid_t tmp;
            while (sstream >> tmp) {
                linked_ids.push_back(tmp);
            }
            auto lever = new Lever(id);
            lever->init(physics, texProgram, orientation, default_state, linked_ids);
            lever->set_position(glm::vec2(pos));
            auto r = objects.emplace(id, lever);
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




