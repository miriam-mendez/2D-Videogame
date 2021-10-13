#include "FlippedPlayer.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>

void FlippedPlayer::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram) {
    Player::init(tileMapPos, shaderProgram); // same code, no need to override
    // mirrored sprite
    auto flip = glm::rotate(glm::mat4(1.0f), glm::radians(180.f), glm::vec3(1.f, 0.f, 0.f));
    sprite->setModifier(flip);
}

void FlippedPlayer::update(int deltaTime) {
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
            posPlayer.y = int(startY + 96 * sin(3.14159f * jumpAngle / 180.f));
            if (jumpAngle > 90) {
                bJumping = !map->collisionMoveUp(posPlayer, glm::ivec2(28, 33), posPlayer.y);
            }
        }
    }
    else {
        posPlayer.y -= FALL_STEP;
        if (map->collisionMoveUp(posPlayer, glm::ivec2(28, 33), posPlayer.y)) {
            if (Game::instance().getSpecialKey(GLUT_KEY_UP)) {
                bJumping = true;
                jumpAngle = 0;
                startY = posPlayer.y;
            }
        }
    }

    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}