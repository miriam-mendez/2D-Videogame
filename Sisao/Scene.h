#pragma once

#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include <box2d/b2_world.h>
#include <unordered_map>
#include "Object.h"

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
    void initShaders();

private:
    TileMap* map = nullptr;
    Player* player1 = nullptr;
    Player* player2 = nullptr;
    std::unordered_map<std::uint32_t, Object*> objects;

    b2World* physics = nullptr;

    ShaderProgram texProgram;
    float currentTime;
};

