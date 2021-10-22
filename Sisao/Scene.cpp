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


Scene::Scene() {}

Scene::~Scene() {
    for (auto const& x : objects) {
        delete x.second;
    }
    objects.clear();
    delete water;
}

void Scene::free() {
    for (auto const& x : objects) {
        camera.unfollow(x.second);
        delete x.second;
    }
    objects.clear();
    delete water;
    delete physics;
}


void Scene::init(std::string level) {
    initShaders();
    currentTime = 0.0f;

    physics = new b2World(b2Vec2(0.0f, 0.0f));
    physics->SetContactListener(&physics_listener);

    std::ifstream stream;
    stream.open(level.c_str());
    read_level(stream);
    stream.close();

    camera.init(physics, 1.f, false, true);
    camera.set_orthogonal(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
    camera.set_position(map.get_center());

    glm::vec2 ocean_size = glm::vec2(SCREEN_WIDTH * 99, SCREEN_HEIGHT);
    glm::vec2 ocean_pos = map.get_center();
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
    auto projection = camera.projection_matrix();
    auto model_view = camera.view_matrix();
    texProgram.use();
    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    texProgram.setUniformMatrix4f("modelview", model_view);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
    map.render();

    for (auto const& x : objects) {
        x.second->render();
    }
    waterProgram.use();
    model_view = glm::translate(glm::mat4(1.0f), glm::vec3(0, SCREEN_HEIGHT / 2.f, 0));
    waterProgram.setUniform1f("time", currentTime);
    waterProgram.setUniformMatrix4f("projection", projection);
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
        std::getline(stream, line);
    }
}

void Scene::initShaders() {
    {
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




    Shader vShader, fShader;

    vShader.initFromFile(VERTEX_SHADER, "shaders/water.vert");
    if (!vShader.isCompiled()) {
        cout << "Vertex Shader Error" << endl;
        cout << "" << vShader.log() << endl << endl;
    }
    fShader.initFromFile(FRAGMENT_SHADER, "shaders/water.frag");
    if (!fShader.isCompiled()) {
        cout << "Fragment Shader Error" << endl;
        cout << "" << fShader.log() << endl << endl;
    }
    waterProgram.init();
    waterProgram.addShader(vShader);
    waterProgram.addShader(fShader);
    waterProgram.link();
    if (!waterProgram.isLinked()) {
        cout << "Shader Linking Error" << endl;
        cout << "" << waterProgram.log() << endl << endl;
    }
    waterProgram.bindFragmentOutput("outColor");
    vShader.free();
    fShader.free();
}




