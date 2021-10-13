#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"


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
    TileMap* map;
    Player* player1;
    Player* player2;
    ShaderProgram texProgram;
    float currentTime;
};


#endif // _SCENE_INCLUDE

