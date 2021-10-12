#include "Camera.h"


glm::mat4& Camera::view_matrix() {
    return view;
}

glm::mat4& Camera::projection_matrix() {
    return projection;
}


void Camera::set_orthogonal(float left, float right, float bottom, float top) {
    projection = glm::ortho(left, right, bottom, top);
}