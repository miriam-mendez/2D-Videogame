#pragma once
#include <glm/glm.hpp>

namespace Constants {
    namespace Units {
        static constexpr float pixels_per_meter = 32.0f;
        static constexpr float meters_per_pixel = 1.0f / pixels_per_meter;
    };

    namespace Physics {
        static constexpr float timestep = 1.0f / 60.0f;
        static constexpr int velocity_iters = 8;
        static constexpr int position_iters = 3;
        static const auto gravity = glm::vec2(0, 30);
    };

    namespace Player {
        static constexpr int movement_speed = 10;
        static constexpr int jump_speed = 10;
    };
};