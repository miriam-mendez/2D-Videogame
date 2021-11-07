#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Sprite.h"
#include "Camera.h"
#include "Game.h"

Sprite::Sprite(const glm::vec2& quadSize, const glm::vec2& sizeInSpritesheet, Texture* spritesheet, ShaderProgram* program) {
    const glm::vec2 max = quadSize / 2.f;
    const glm::vec2 min = -max;
    float vertices[24] = { min.x, min.y, 0.f, 0.f,
                                                max.x, min.y, sizeInSpritesheet.x, 0.f,
                                                max.x, max.y, sizeInSpritesheet.x, sizeInSpritesheet.y,
                                                min.x, min.y, 0.f, 0.f,
                                                max.x, max.y, sizeInSpritesheet.x, sizeInSpritesheet.y,
                                                min.x, max.y, 0.f, sizeInSpritesheet.y };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_STATIC_DRAW);
    posLocation = program->bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
    texCoordLocation = program->bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    texture = spritesheet;
    shaderProgram = program;
    currentAnimation = -1;
    position = glm::vec2(0.f);
    quad_size = quadSize;
}

Sprite* Sprite::init(const glm::vec2& quadSize, const glm::vec2& sizeInSpritesheet, Texture* spritesheet, ShaderProgram* program) {
    return new Sprite(quadSize, sizeInSpritesheet, spritesheet, program);
}

void Sprite::update(int deltaTime) {
    if (currentAnimation >= 0) {
        timeAnimation += deltaTime;
        while (timeAnimation > animations[currentAnimation].millisecsPerKeyframe) {
            timeAnimation -= animations[currentAnimation].millisecsPerKeyframe;
            auto last = currentKeyframe;
            currentKeyframe = (currentKeyframe + 1) % animations[currentAnimation].keyframeDispl.size();;
            bool loops = animations[currentAnimation].loop;
            if (last > currentKeyframe && !loops) {
                int chained = animations[currentAnimation].next;
                if (chained != -1) {
                    changeAnimation(chained);
                }
                else {
                    currentKeyframe = last;
                }
            }
        }
        texCoordDispl = animations[currentAnimation].keyframeDispl[currentKeyframe];
    }
}

void Sprite::render() const {
    shaderProgram->use();
    auto& modelview = modelview_matrix();
    auto projection = Game::instance().get_scene().get_camera().projection_matrix();
    shaderProgram->setUniformMatrix4f("modelviewprojection", projection * modelview);
    shaderProgram->setUniform2f("texCoordDispl", texCoordDispl.x, texCoordDispl.y);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    texture->use();
    glBindVertexArray(vao);
    glEnableVertexAttribArray(posLocation);
    glEnableVertexAttribArray(texCoordLocation);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}


void Sprite::setAnimationSpeed(int animId, int keyframesPerSec) {
    auto r = glm::max(animId + 1, (int)animations.size());
    animations.resize(r);
    animations[animId].millisecsPerKeyframe = 1000.f / keyframesPerSec;
}

void Sprite::setAnimationLoop(int animId, bool loop) {
    auto r = glm::max(animId + 1, (int)animations.size());
    animations.resize(r);
    animations[animId].loop = loop;
}

void Sprite::setAnimationNext(int animId, int next_animId) {
    auto r = glm::max(animId + 1, (int)animations.size());
    animations.resize(r);
    animations[animId].next = next_animId;
}

void Sprite::addKeyframe(int animId, const glm::vec2& displacement) {
    auto r = glm::max(animId + 1, (int)animations.size());
    animations.resize(r);
    animations[animId].keyframeDispl.push_back(displacement);
}

void Sprite::changeAnimation(int animId) {
    if (animId < int(animations.size())) {
        currentAnimation = animId;
        currentKeyframe = 0;
        timeAnimation = 0.f;
        texCoordDispl = animations[animId].keyframeDispl[0];
    }
}

void Sprite::changeAnimation(int animId, int frame_start) {
    if (animId < int(animations.size())) {
        currentAnimation = animId;
        currentKeyframe = frame_start;
        timeAnimation = 0.f;
        texCoordDispl = animations[animId].keyframeDispl[frame_start];
    }
}

int Sprite::animation() const {
    return currentAnimation;
}

