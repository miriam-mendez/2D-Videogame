#pragma once
#include <glm/glm.hpp>
#include <GL/glut.h>

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
            Sensor = 0x0004,
            Camera = 0x0008,
            AboveWater = 0x0010,
            BelowWater = 0x0020
        };
        enum class Mask { // what object types does collide
            Wall = (int)Category::Regular,
            Regular = (int)Category::Wall | (int)Category::Regular | (int)Category::Sensor,
            Sensor = (int)Category::Regular | (int)Category::Sensor | (int)Category::Camera
        };
    };

    namespace Regular {
        static constexpr int max_movement_speed = 180;
        static constexpr int max_jump_speed = 330;
    };

    namespace Keys {
        static constexpr int R = 114;
        static constexpr int E = 101;
        static constexpr int T = 116;
        static constexpr int W = 119;
        static constexpr int A = 97;
        static constexpr int S = 115;
        static constexpr int D = 100;
        static constexpr int G = 103;
        static constexpr int B = 98;
        static constexpr int _1 = 49;
        static constexpr int _2 = 50;
        static constexpr int _3 = 51;
        static constexpr int _4 = 52;
        static constexpr int _5 = 53;
        static constexpr int Space = 32;
        static constexpr int Esc = 27;
        static constexpr int ArrowUp = GLUT_KEY_UP;
        static constexpr int ArrowDown = GLUT_KEY_DOWN;
        static constexpr int ArrowLeft = GLUT_KEY_LEFT;
        static constexpr int ArrowRight = GLUT_KEY_RIGHT;
    };
};