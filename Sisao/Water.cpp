#include "Water.h"
#include "Game.h"

void Water::init(b2World* physics, ShaderProgram& shaderProgram) {
    glm::vec2 ocean_size = glm::vec2(SCREEN_WIDTH * 20, SCREEN_HEIGHT);
    quad = Quad::init(ocean_size, &shaderProgram);
}

void Water::update(int deltaTime) {
    quad->set_position(position);
    quad->set_rotation(rotation);
}

void Water::render() {
    auto time = Game::instance().get_scene().get_current_time();
    auto shader = quad->expose_shader();
    shader->use();
    shader->setUniform1f("time", time);
    shader->setUniform4f("color", 0.2f, 0.27f, 0.37f, 0.6f);
    quad->render();
}
