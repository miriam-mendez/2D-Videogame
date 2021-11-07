#pragma once

#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "Texture.h"
#include <box2d/b2_world.h>
#include <vector>

// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.

class TileMap {
public:

    TileMap(); // Tile maps can only be created inside an OpenGL context

    TileMap(std::ifstream& stream, b2World* physics,
            ShaderProgram& program);
    ~TileMap();

    void render();

    int getTileSize() const { return tileSize; }

    glm::vec2 get_center() const;

private:
    void read_tilemap(std::ifstream& stream);
    void prepareArrays(ShaderProgram& program);
    void register_physics(b2World* physics);

private:
    GLuint vao;
    GLuint vbo;
    GLint posLocation, texCoordLocation;
    ShaderProgram shader;

    glm::vec2 position = glm::vec2(0.f, 0.f);
    glm::ivec2 mapSize = glm::ivec2(0, 0);
    glm::ivec2 tilesheetSize = glm::ivec2(0, 0);
    int tileSize, blockSize;
    Texture tilesheet;
    glm::vec2 tileTexSize;
    std::vector<int> map;
};

