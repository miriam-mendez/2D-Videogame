#include "Quad.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Sprite.h"
#include "Camera.h"
#include "Game.h"

Quad::Quad(float width, float height, ShaderProgram& program) : shaderProgram(program) {
    /*float vertices[24] = {0.f, 0.f, 0.f, 0.f,
        width, 0.f, 1.f, 0.f,
        width, height, 1.f, 1.f,
        0.f, 0.f, 0.f, 0.f,
        width, height, 1.f, 1.f,
        0.f, height, 0.f, 1.f };*/

    float vertices[24] = { 0.f, 0.f, 0.f, 0.f,
        width, 0.f, 1.f, 0.f,
        width, height, 1.f, 1.f,
        0.f, 0.f, 0.f, 0.f,
        width, height, 1.f, 1.f,
        0.f, height, 0.f, 1.f };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_STATIC_DRAW);
    posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
}

Quad::~Quad() {
    glDeleteBuffers(1, &vbo);
}

void Quad::update(int deltaTime) {}

void Quad::render() const {
    glBindVertexArray(vao);
    glEnableVertexAttribArray(posLocation);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Quad::setPosition(const glm::vec2& pos) {}

void Quad::setRotation(float radians) {}

void Quad::setFlip(bool vertical, bool horizontal) {}

void Quad::setScale(const glm::vec2& scale) {}
