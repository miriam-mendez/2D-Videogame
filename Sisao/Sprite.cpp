#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Sprite.h"
#include "Camera.h"
#include "Game.h"

Sprite* Sprite::createSprite(const glm::vec2& quadSize, const glm::vec2& sizeInSpritesheet, Texture* spritesheet, ShaderProgram* program) {
    Sprite* quad = new Sprite(quadSize, sizeInSpritesheet, spritesheet, program);

    return quad;
}


Sprite::Sprite(const glm::vec2& quadSize, const glm::vec2& sizeInSpritesheet, Texture* spritesheet, ShaderProgram* program) {
    float vertices[24] = { 0.f, 0.f, 0.f, 0.f,
                                                quadSize.x, 0.f, sizeInSpritesheet.x, 0.f,
                                                quadSize.x, quadSize.y, sizeInSpritesheet.x, sizeInSpritesheet.y,
                                                0.f, 0.f, 0.f, 0.f,
                                                quadSize.x, quadSize.y, sizeInSpritesheet.x, sizeInSpritesheet.y,
                                                0.f, quadSize.y, 0.f, sizeInSpritesheet.y };

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

void Sprite::update(int deltaTime) {
    if (currentAnimation >= 0) {
        timeAnimation += deltaTime;
        while (timeAnimation > animations[currentAnimation].millisecsPerKeyframe) {
            timeAnimation -= animations[currentAnimation].millisecsPerKeyframe;
            currentKeyframe = (currentKeyframe + 1) % animations[currentAnimation].keyframeDispl.size();
        }
        texCoordDispl = animations[currentAnimation].keyframeDispl[currentKeyframe];
    }
}

void Sprite::render() const {
    // compute translation
    const auto& cameraview = Game::instance().get_scene().get_camera().view_matrix();
    glm::mat4 modelview = glm::translate(cameraview, glm::vec3(position.x, position.y, 0.f));
    // compute rotation
    auto rotation_mat = glm::rotate(glm::mat4(1), rotation, glm::vec3(0.f, 0.f, 1.f));
    rotation_mat = rotation_mat * flipVH;
    const auto center_mat = glm::translate(glm::mat4(1.0f), glm::vec3(quad_size / 2.f, 0.f));
    rotation_mat = center_mat * rotation_mat * glm::inverse(center_mat);
    // compute scale
    const auto scale_mat = glm::scale(glm::mat4(1.f), glm::vec3(scale, 1.f));
    modelview = modelview * rotation_mat * scale_mat;

    shaderProgram->setUniformMatrix4f("modelview", modelview);
    shaderProgram->setUniform2f("texCoordDispl", texCoordDispl.x, texCoordDispl.y);
    glEnable(GL_TEXTURE_2D);
    texture->use();
    glBindVertexArray(vao);
    glEnableVertexAttribArray(posLocation);
    glEnableVertexAttribArray(texCoordLocation);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_TEXTURE_2D);
}

void Sprite::free() {
    glDeleteBuffers(1, &vbo);
}

void Sprite::setNumberAnimations(int nAnimations) {
    animations.clear();
    animations.resize(nAnimations);
}

void Sprite::setAnimationSpeed(int animId, int keyframesPerSec) {
    if (animId < int(animations.size()))
        animations[animId].millisecsPerKeyframe = 1000.f / keyframesPerSec;
}

void Sprite::addKeyframe(int animId, const glm::vec2& displacement) {
    if (animId < int(animations.size()))
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

int Sprite::animation() const {
    return currentAnimation;
}

void Sprite::setPosition(const glm::vec2& pos) {
    position = pos;
}

void Sprite::setRotation(float radians) {
    rotation = radians;
}

void Sprite::setFlip(bool vertical, bool horizontal) {
    const float radians = glm::radians(180.f);
    flipVH = glm::rotate(glm::mat4(1.0f), horizontal * radians, glm::vec3(1.f, 0.f, 0.f));
    flipVH = glm::rotate(flipVH, vertical * radians, glm::vec3(0.f, 1.f, 0.f));
}

void Sprite::setScale(const glm::vec2& axis_scale) {
    scale = axis_scale;
}

