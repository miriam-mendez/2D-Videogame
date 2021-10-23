#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include "Object.h"

class Camera : public Object {
protected:
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-200, 0, 0.f));
    glm::mat4 projection = glm::mat4(1.0f);
    glm::ivec2 view_rect;
    std::unordered_map<Object::uuid_t, Object*> follow_targets;

    float follow_speed = 1.f;
    bool lock_x = false;
    bool lock_y = false;

public:
    Camera() : Object() {};

    virtual void init(b2World* physics, float follow_speed, bool lock_x, bool lock_y);

    virtual void update(int deltaTime);

    bool follow(Object* target);
    bool unfollow(Object* target);

    glm::mat4 view_matrix() const;
    glm::mat4 projection_matrix() const;

    void set_orthogonal(float left, float right, float bottom, float top);
};

