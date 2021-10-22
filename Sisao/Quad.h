#pragma once
#include "ShaderProgram.h"


class Quad {
public:
    Quad(float width, float height, ShaderProgram& program);
    ~Quad();

    virtual void update(int deltaTime);
    virtual void render() const;

    void setPosition(const glm::vec2& pos);
    void setRotation(float radians);
    void setFlip(bool vertical, bool horizontal);
    void setScale(const glm::vec2& scale);

private:
    GLuint vao;
    GLuint vbo;
    GLint posLocation;
    ShaderProgram& shaderProgram;

    glm::vec2 position = glm::vec2(0, 0);
    float rotation = 0.f;
    glm::vec2 scale = glm::vec2(1, 1);
    glm::mat4 flipVH = glm::mat4(1);
    bool flipV = false;
    bool flipH = false;
};