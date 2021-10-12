#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include "Camera.h"

#define INIT_PLAYER_X_TILES 3
#define INIT_PLAYER_Y_TILES 3

#define MAP_HEIGHT 11

//#define SCREEN_X 0//SCREEN_WIDTH/2 - INIT_PLAYER_X_TILES*32
//#define SCREEN_Y 0//SCREEN_HEIGHT/2 - 32*MAP_HEIGHT/2


Scene::Scene() {
    map = NULL;
    player = NULL;
}

Scene::~Scene() {
    if (map != NULL)
        delete map;
    if (player != NULL)
        delete player;
}


void Scene::init(std::string level) {
    initShaders();
    map = TileMap::createTileMap(level, glm::vec2(0, 0), texProgram);
    player = new Player();
    player->init(glm::ivec2(0, 0), texProgram);
    player->setPosition(map->get_spawn1());
    //player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
    player->setTileMap(map);
    Camera::get_instance().set_orthogonal(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
    currentTime = 0.0f;
}

void Scene::update(int deltaTime) {
    currentTime += deltaTime;
    player->update(deltaTime);
}

void Scene::render() {
    auto& cam = Camera::get_instance();

    auto projection = cam.projection_matrix();

    glm::vec2 pos = player->getPosition();
    cam.view_matrix() = glm::translate(glm::mat4(1.0f), glm::vec3(-pos.x + SCREEN_WIDTH / 2, 0.f, 0.f));
    auto model_view = cam.view_matrix();

    texProgram.use();
    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    texProgram.setUniformMatrix4f("modelview", model_view);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
    map->render();
    player->render();
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



