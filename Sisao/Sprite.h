#pragma once


#include <vector>
#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"
#include "AnimKeyframes.h"
#include "Quad.h"


class Sprite : public Quad {
    Sprite(const glm::vec2& quadSize, const glm::vec2& sizeInSpritesheet, Texture* spritesheet, ShaderProgram* program);
public:
    static Sprite* init(const glm::vec2& quadSize, const glm::vec2& sizeInSpritesheet, Texture* spritesheet, ShaderProgram* program);

    virtual void update(int deltaTime);
    virtual void render() const;

    void setNumberAnimations(int nAnimations);
    void setAnimationSpeed(int animId, int keyframesPerSec);
    void setAnimationLoop(int animId, bool loop);
    void addKeyframe(int animId, const glm::vec2& frame);
    void changeAnimation(int animId);
    int animation() const;

protected:
    Sprite() {};

    Texture* texture;
    GLint texCoordLocation;

    glm::vec2 quad_size;

    int currentAnimation = 0;
    int currentKeyframe = 0;
    float timeAnimation = 0;
    glm::vec2 texCoordDispl;
    vector<AnimKeyframes> animations;

    ShaderProgram* expose_shader() = delete;

};

