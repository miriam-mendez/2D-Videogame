#pragma once
#include "ShaderProgram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Quad {
    Quad(glm::vec2 size, ShaderProgram* program);
public:
    static Quad* init(glm::vec2 size, ShaderProgram* program);
    virtual ~Quad();

    virtual void update(int deltaTime);
    virtual void render() const;

    void set_position(const glm::vec2& pos);
    glm::vec2 get_position() const;
    void set_rotation(float radians);
    float get_rotation() const;
    void set_flip(bool vertical, bool horizontal);
    bool is_horizontal_flipped() const;
    bool is_vertical_flipped() const;
    void set_scale(const glm::vec2& scale);
    glm::vec2 get_scale() const;


    ShaderProgram* expose_shader();

protected:
    Quad() {};

    glm::mat4 model_matrix() const;
    glm::mat4 modelview_matrix() const;

    GLuint vao;
    GLuint vbo;
    GLint posLocation;
    ShaderProgram* shaderProgram;

    glm::vec2 position = glm::vec2(0, 0);
    float rotation = 0.f;
    glm::vec2 scale = glm::vec2(1, 1);
    glm::mat4 flipVH = glm::mat4(1);
    bool flipV = false;
    bool flipH = false;
};