#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include "Camera.h"
#include "FlippedPlayer.h"

#define INIT_PLAYER_X_TILES 3
#define INIT_PLAYER_Y_TILES 3

#define MAP_HEIGHT 11

//#define SCREEN_X 0//SCREEN_WIDTH/2 - INIT_PLAYER_X_TILES*32
//#define SCREEN_Y 0//SCREEN_HEIGHT/2 - 32*MAP_HEIGHT/2


Scene::Scene() {
    map = NULL;
    player1 = NULL;
}

Scene::~Scene() {
    if (map != NULL)
        delete map;
    if (player1 != NULL) {
        delete player1;
        delete player2;
    }
}


void Scene::init(std::string level) {
    initShaders();
    currentTime = 0.0f;
    map = TileMap::createTileMap(level, glm::vec2(0, 0), texProgram);
    // spawn player 1
    player1 = new Player();
    player1->init(glm::ivec2(0, 0), texProgram);
    player1->setPosition(map->get_spawn1());
    player1->setTileMap(map);
    // spawn player 2
    player2 = new FlippedPlayer();
    player2->init(glm::ivec2(0, 0), texProgram);
    player2->setPosition(map->get_spawn2());
    player2->setTileMap(map);
    // setup camera
    Camera::get_instance().set_orthogonal(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
}

void Scene::update(int deltaTime) {
    currentTime += deltaTime;
    player1->update(deltaTime);
    player2->update(deltaTime);
}

void Scene::render() {
    auto& cam = Camera::get_instance();
    auto projection = cam.projection_matrix();

    glm::vec2 players_center = (player1->getPosition() + player2->getPosition()) / 2.f;
    glm::vec2 map_center = map->get_center();
    glm::vec2 camera_focus_point = glm::vec2(SCREEN_WIDTH / 2.f - players_center.x, SCREEN_HEIGHT / 2.f - map_center.y);
    cam.view_matrix() = glm::translate(glm::mat4(1.0f), glm::vec3(camera_focus_point, 0.f));
    auto model_view = cam.view_matrix();

    texProgram.use();
    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    texProgram.setUniformMatrix4f("modelview", model_view);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
    map->render();
    player1->render();
    player2->render();
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



