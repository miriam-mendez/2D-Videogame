#pragma once
#include "Sprite.h"
#include "TileMap.h"

/*

#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4


enum PlayerAnims {
    STAND_LEFT_UP,
    STAND_RIGHT_UP,
    MOVE_LEFT_UP,
    MOVE_RIGHT_UP,
    // STAND_LEFT_DOWN,
    // STAND_RIGHT_DOWN,
    // MOVE_LEFT_DOWN,
    // MOVE_RIGHT_DOWN
};


class Player {

public:
    virtual void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
    virtual void update(int deltaTime);
    void render();

    void setTileMap(TileMap* tileMap);
    void setPosition(const glm::vec2& pos1);
    glm::vec2 getPosition() const;

protected:
    bool bJumping;
    glm::ivec2 tileMapDispl, posPlayer;
    int jumpAngle, startY;
    Texture spritesheet;
    Sprite* sprite;
    TileMap* map;

};


*/