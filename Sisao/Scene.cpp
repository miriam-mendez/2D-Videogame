#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include "Camera.h"
#include "FlippedPlayer.h"
#include "Player.h"
#include "Box.h"
#include "Constants.h"
#include <fstream>
#include <sstream>


Scene::Scene() {}

Scene::~Scene() {
    for (auto const& x : objects) {
        delete x.second;
    }
    objects.clear();
}


void Scene::init(std::string level) {
    initShaders();
    currentTime = 0.0f;

    physics.SetContactListener(&physics_listener);
    Camera::get_instance().set_orthogonal(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);

    std::ifstream stream;
    stream.open(level.c_str());
    read_level(stream);
}

void Scene::update(int deltaTime) {
    currentTime += deltaTime;

    for (auto const& x : objects) {
        x.second->update(deltaTime);
    }

    physics.Step(Constants::Physics::timestep, Constants::Physics::velocity_iters, Constants::Physics::position_iters);
}

void Scene::render() {
    auto& cam = Camera::get_instance();
    auto projection = cam.projection_matrix();

    glm::vec2 players_center = glm::vec2(200, 0);//(player1->getPosition() + player2->getPosition()) / 2.f;
    glm::vec2 map_center = map.get_center();
    glm::vec2 camera_focus_point = glm::vec2(SCREEN_WIDTH / 2.f - players_center.x, SCREEN_HEIGHT / 2.f - map_center.y);
    cam.view_matrix() = glm::translate(glm::mat4(1.0f), glm::vec3(camera_focus_point, 0.f));
    auto model_view = cam.view_matrix();

    texProgram.use();
    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    texProgram.setUniformMatrix4f("modelview", model_view);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
    map.render();

    for (auto const& x : objects) {
        x.second->render();
    }
}

void Scene::read_level(std::ifstream& stream) {
    std::string line;
    while (std::getline(stream, line)) {
        if (line.find("[BEGIN TILEMAP]") != string::npos) {
            map = TileMap(stream, physics, glm::vec2(0, 0), texProgram);
        }
        else if (line.find("[BEGIN OBJECTS]") != string::npos) {
            read_objects(stream);
        }
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
            bool invert;
            sstream.str(args);
            sstream >> pos.x >> pos.y >> invert;
            auto id = generate_object_uuid();
            Player* player = invert ? new FlippedPlayer(id) : new Player(id);
            player->init(physics, texProgram);
            player->set_position(glm::vec2(pos));
            objects.emplace(id, player);
        }
        else if (instr == "BOX") {
            auto id = generate_object_uuid();
            auto box = new Box(id);
            box->init(physics, texProgram);
            sstream.str(args);
            glm::ivec2 pos;
            sstream >> pos.x >> pos.y;
            box->set_position(glm::vec2(pos));
            objects.emplace(id, box);
        }
        std::getline(stream, line);
    }
}

void Scene::initShaders() {
    Shader vShader, fShader;

    vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
    if (!vShader.isCompiled()) {
        cout << "Vertex Shader Error" << endl;
        cout << "" << vShader.log() << endl << endl;
    }
    fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
    if (!fShader.isCompiled()) {
        cout << "Fragment Shader Error" << endl;
        cout << "" << fShader.log() << endl << endl;
    }
    texProgram.init();
    texProgram.addShader(vShader);
    texProgram.addShader(fShader);
    texProgram.link();
    if (!texProgram.isLinked()) {
        cout << "Shader Linking Error" << endl;
        cout << "" << texProgram.log() << endl << endl;
    }
    texProgram.bindFragmentOutput("outColor");
    vShader.free();
    fShader.free();
}

Object::uuid_t Scene::generate_object_uuid() {
    static Object::uuid_t current = 0;
    return current++;
}



