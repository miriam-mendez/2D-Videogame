#include "Camera.h"
#include "Constants.h"
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include "utils.h"

void Camera::update(int deltaTime) {
    Object::physics_update(deltaTime);
    // calculate following center position
    glm::vec2 follow_target = glm::vec2(0.f, 0.f);
    if (follow_targets.size() > 0) {
        for (auto const& target : follow_targets) {
            follow_target += target.second->get_position();
        }
        follow_target /= follow_targets.size();
    }
    // following center lock behaviour
    follow_target.x = (lock_x) ? position.x : follow_target.x;
    follow_target.y = (lock_y) ? position.y : follow_target.y;
    // camera smooth movement
    auto v = (follow_target - position) * Constants::Units::meters_per_pixel;
    physic_body->SetLinearVelocity(to_box2d(v * follow_speed));

    glm::vec2 current = glm::vec2(view_rect) / 2.f - position;
    view = glm::translate(glm::mat4(1.0f), glm::vec3(current, 0.f));
}

bool Camera::follow(Object* target) {
    if (target) {
        auto result = follow_targets.emplace(target->get_id(), target);
        return result.second;
    }
    return false;
}

void Camera::init(b2World* physics, float follow_speed, bool lock_x, bool lock_y) {
    this->follow_speed = follow_speed;
    this->lock_x = lock_x;
    this->lock_y = lock_y;
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(position.x, position.y);
    physic_body = physics->CreateBody(&body_def);

    const glm::vec2 view_rect_meters = glm::vec2(view_rect) * Constants::Units::meters_per_pixel;
    b2PolygonShape box_shape;
    box_shape.SetAsBox(view_rect_meters.x / 2.f, view_rect_meters.y / 2.f);
    b2FixtureDef fixture_def;
    fixture_def.shape = &box_shape;
    fixture_def.filter.categoryBits = (int)Constants::Physics::Category::Sensor;
    fixture_def.filter.maskBits = (int)Constants::Physics::Mask::Sensor;
    fixture_def.isSensor = true;
    physic_body->CreateFixture(&fixture_def);
}

bool Camera::unfollow(Object* target) {
    if (target) {
        return follow_targets.erase(target->get_id());
    }
    return false;
}

glm::mat4 const& Camera::view_matrix() const {
    return view;
}

glm::mat4 const& Camera::projection_matrix() const {
    return projection;
}


void Camera::set_orthogonal(float left, float right, float bottom, float top) {
    projection = glm::ortho(left, right, bottom, top);
    view_rect = glm::ivec2(right - left, bottom - top);
}