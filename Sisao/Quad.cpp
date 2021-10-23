#include "Quad.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Sprite.h"
#include "Camera.h"
#include "Game.h"

Quad::Quad(glm::vec2 size, ShaderProgram* program) : shaderProgram(program) {
    const glm::vec2 max = size / 2.f;
    const glm::vec2 min = -max;
    float vertices[24] = { min.x, min.y, 1.f, 1.f,
                                                max.x, min.y, 1.f, 1.f,
                                                max.x, max.y, 1.f, 1.f,
                                                min.x, min.y, 1.f, 1.f,
                                                max.x, max.y, 1.f, 1.f,
                                                min.x, max.y, 1.f, 1.f };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_STATIC_DRAW);
    posLocation = program->bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
}

Quad* Quad::init(glm::vec2 size, ShaderProgram* program) {
    return new Quad(size, program);
}

Quad::~Quad() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vao);
}

void Quad::update(int deltaTime) {}

void Quad::render() const {
    shaderProgram->use();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    auto& model = model_matrix();
    auto view = Game::instance().get_scene().get_camera().view_matrix();
    auto projection = Game::instance().get_scene().get_camera().projection_matrix();
    shaderProgram->setUniformMatrix4f("model", model);
    shaderProgram->setUniformMatrix4f("view", view);
    shaderProgram->setUniformMatrix4f("projection", projection);
    shaderProgram->setUniformMatrix4f("modelviewprojection", projection * view * model);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(posLocation);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_BLEND);
}


void Quad::setPosition(const glm::vec2& pos) {
    position = pos;
}

void Quad::setRotation(float radians) {
    rotation = radians;
}

void Quad::setFlip(bool vertical, bool horizontal) {
    const float radians = glm::radians(180.f);
    flipVH = glm::rotate(glm::mat4(1.0f), horizontal * radians, glm::vec3(1.f, 0.f, 0.f));
    flipVH = glm::rotate(flipVH, vertical * radians, glm::vec3(0.f, 1.f, 0.f));
}

void Quad::setScale(const glm::vec2& axis_scale) {
    scale = axis_scale;
}

ShaderProgram* Quad::expose_shader() {
    return shaderProgram;
}

glm::mat4 Quad::model_matrix() const {
    // compute translation
    glm::mat4 translation_mat = glm::translate(glm::mat4(1), glm::vec3(position.x, position.y, 0.f));
    // compute rotation
    auto rotation_mat = glm::rotate(glm::mat4(1), rotation, glm::vec3(0.f, 0.f, 1.f));
    rotation_mat = rotation_mat * flipVH;
    // compute scale
    auto model = glm::scale(translation_mat * rotation_mat, glm::vec3(scale, 1.f));
    return model;
}

glm::mat4 Quad::modelview_matrix() const {
    const auto& view = Game::instance().get_scene().get_camera().view_matrix();
    return view * model_matrix();
}
