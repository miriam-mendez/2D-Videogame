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
        static const auto gravity = glm::vec2(0, 1000); // pixels

        enum class Category { // what object type is it
            Wall = 0x0001,
            Regular = 0x0002,
            Sensor = 0x0003
        };
        enum class Mask { // what object types does collide
            Wall = (int)Category::Regular,
            Regular = (int)Category::Wall | (int)Category::Regular | (int)Category::Sensor,
            Sensor = (int)Category::Regular | (int)Category::Sensor
        };
    };

    namespace Regular {
        static constexpr int max_movement_speed = 200;
        static constexpr int max_jump_speed = 350;
    };
};