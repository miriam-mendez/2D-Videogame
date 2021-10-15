#pragma once

#include <glm/glm.hpp>
#include <box2d/box2d.h>


b2Vec2 to_box2d(glm::vec2 const& vec);
glm::vec2 to_glm(b2Vec2 const& vec);