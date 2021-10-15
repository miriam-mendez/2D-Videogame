#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE

#include <glm/glm.hpp>

#include "ShaderProgram.h"
#include "Texture.h"
#include <box2d/b2_world.h>

// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.

class TileMap {
public:

    TileMap(); // Tile maps can only be created inside an OpenGL context

    TileMap(std::ifstream& stream, b2World& physics, const glm::vec2& minCoords,
            ShaderProgram& program);
    ~TileMap();

    void render() const;
    void free();

    int getTileSize() const { return tileSize; }

    bool collisionMoveLeft(const glm::ivec2& pos, const glm::ivec2& size) const;
    bool collisionMoveRight(const glm::ivec2& pos, const glm::ivec2& size) const;
    bool collisionMoveDown(const glm::ivec2& pos, const glm::ivec2& size,
                           int& posY) const;
    bool collisionMoveUp(const glm::ivec2& pos, const glm::ivec2& size,
                         int& posY) const;

    glm::vec2 get_spawn1() const;
    glm::vec2 get_spawn2() const;

    glm::vec2 get_center() const;

private:
    void read_tilemap(std::ifstream& stream);
    void prepareArrays(const glm::vec2& minCoords, ShaderProgram& program);
    void register_physics(b2World& physics);

private:
    GLuint vao;
    GLuint vbo;
    GLint posLocation, texCoordLocation;
    glm::ivec2 mapSize, tilesheetSize;
    int tileSize, blockSize;
    Texture tilesheet;
    glm::vec2 tileTexSize;
    int* map;
    glm::vec2 player1_spawn, player2_spawn;
    glm::vec2 map_center;

    b2Body* physics_body = nullptr; // freed automatically
};

#endif // _TILE_MAP_INCLUDE
