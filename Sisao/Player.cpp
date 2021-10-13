#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"



void Player::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram) {
    bJumping = false;
    spritesheet.loadFromFile("images/yoshi.png", TEXTURE_PIXEL_FORMAT_RGBA);
    sprite = Sprite::createSprite(glm::ivec2(28, 33), glm::vec2(1 / 12.f, 1 / 2.f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(4);

    sprite->setAnimationSpeed(STAND_LEFT_UP, 8);
    sprite->addKeyframe(STAND_LEFT_UP, glm::vec2(0.f, 0.5f));

    sprite->setAnimationSpeed(STAND_RIGHT_UP, 8);
    sprite->addKeyframe(STAND_RIGHT_UP, glm::vec2(0.f, 0.f));

    sprite->setAnimationSpeed(MOVE_LEFT_UP, 8);
    sprite->addKeyframe(MOVE_LEFT_UP, glm::vec2(0.f, 0.5f));
    sprite->addKeyframe(MOVE_LEFT_UP, glm::vec2(2 / 12.f, 0.5f));
    sprite->addKeyframe(MOVE_LEFT_UP, glm::vec2(3 / 12.f, 0.5f));

    sprite->setAnimationSpeed(MOVE_RIGHT_UP, 8);
    sprite->addKeyframe(MOVE_RIGHT_UP, glm::vec2(0.f, 0.f));
    sprite->addKeyframe(MOVE_RIGHT_UP, glm::vec2(2 / 12.f, 0.f));
    sprite->addKeyframe(MOVE_RIGHT_UP, glm::vec2(3 / 12.f, 0.f));

    sprite->changeAnimation(0);
    tileMapDispl = tileMapPos;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::update(int deltaTime) {
    sprite->update(deltaTime);
    if (Game::instance().getSpecialKey(GLUT_KEY_LEFT)) {
        if (sprite->animation() != MOVE_LEFT_UP)
            sprite->changeAnimation(MOVE_LEFT_UP);
        posPlayer.x -= 2;
        if (map->collisionMoveLeft(posPlayer, glm::ivec2(28, 33))) {
            posPlayer.x += 2;
            sprite->changeAnimation(STAND_LEFT_UP);
        }
    }
    else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) {
        if (sprite->animation() != MOVE_RIGHT_UP)
            sprite->changeAnimation(MOVE_RIGHT_UP);
        posPlayer.x += 2;
        if (map->collisionMoveRight(posPlayer, glm::ivec2(28, 33))) {
            posPlayer.x -= 2;
            sprite->changeAnimation(STAND_RIGHT_UP);
        }
    }
    else {
        if (sprite->animation() == MOVE_LEFT_UP)
            sprite->changeAnimation(STAND_LEFT_UP);
        else if (sprite->animation() == MOVE_RIGHT_UP)
            sprite->changeAnimation(STAND_RIGHT_UP);
    }

    if (bJumping) {
        jumpAngle += JUMP_ANGLE_STEP;
        if (jumpAngle == 180) {
            bJumping = false;
            posPlayer.y = startY;
        }
        else {
            posPlayer.y = int(startY - 96 * sin(3.14159f * jumpAngle / 180.f));
            if (jumpAngle > 90) {
                bJumping = !map->collisionMoveDown(posPlayer, glm::ivec2(28, 33), posPlayer.y);
            }
        }
    }
    else {
        posPlayer.y += FALL_STEP;
        if (map->collisionMoveDown(posPlayer, glm::ivec2(28, 33), posPlayer.y)) {
            if (Game::instance().getSpecialKey(GLUT_KEY_UP)) {
                bJumping = true;
                jumpAngle = 0;
                startY = posPlayer.y;
            }
        }
    }

    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::render() {
    sprite->render();
}

void Player::setTileMap(TileMap* tileMap) {
    map = tileMap;
}

void Player::setPosition(const glm::vec2& pos1) {
    posPlayer = pos1;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

glm::vec2 Player::getPosition() const {
    return posPlayer;
}




