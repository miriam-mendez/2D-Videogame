#pragma once
#include <glm/gtc/matrix_transform.hpp>

class Camera {
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-200, 0, 0.f));
    glm::mat4 projection = glm::mat4(1.0f);

public:
    static Camera& get_instance() {
        static Camera c;
        return c;
    };
    glm::mat4& view_matrix();
    glm::mat4& projection_matrix();

    void set_orthogonal(float left, float right, float bottom, float top);
};

