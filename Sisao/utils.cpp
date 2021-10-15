#include "utils.h"

b2Vec2 to_box2d(glm::vec2 const& vec) {
    return b2Vec2(vec.x, vec.y);
}

b2Vec2 to_box2d(glm::ivec2 const& vec) {
    return b2Vec2(vec.x, vec.y);
}

glm::vec2 to_glm(b2Vec2 const& vec) {
    return glm::vec2(vec.x, vec.y);
}
