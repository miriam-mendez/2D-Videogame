#include "TileMap.h"
#include <iostream>
#include <vector>
#include "Constants.h"
#include "utils.h"
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <fstream>
#include <sstream>
using namespace std;


TileMap::TileMap() {}

TileMap::TileMap(std::ifstream& stream, b2World& physics, const glm::vec2& minCoords,
                 ShaderProgram& program) {
    read_tilemap(stream);
    register_physics(physics);
    prepareArrays(minCoords, program);
}

TileMap::~TileMap() {
    if (map != NULL)
        delete map;
}

void TileMap::render() const {
    glEnable(GL_TEXTURE_2D);
    tilesheet.use();
    glBindVertexArray(vao);
    glEnableVertexAttribArray(posLocation);
    glEnableVertexAttribArray(texCoordLocation);
    glDrawArrays(GL_TRIANGLES, 0, 6 * mapSize.x * mapSize.y);
    glDisable(GL_TEXTURE_2D);
}

void TileMap::free() { glDeleteBuffers(1, &vbo); }

void TileMap::read_tilemap(std::ifstream& stream) {
    std::string line;
    std::getline(stream, line);
    while (line.find("[END TILEMAP]") == string::npos) {
        auto instr_end = line.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ_");
        instr_end = (instr_end == string::npos) ? 0 : instr_end;
        auto instr = line.substr(0, instr_end);
        auto args = line.substr(instr_end);
        std::stringstream sstream;
        if (instr == "DIMENSIONS") {
            sstream.str(args);
            sstream >> mapSize.x >> mapSize.y;
        }
        else if (instr == "TILE_SIZE") {
            sstream.str(args);
            sstream >> tileSize;
        }
        else if (instr == "BLOCK_SIZE") {
            sstream.str(args);
            sstream >> blockSize;
        }
        else if (instr == "TILESHEET") {
            sstream.str(args);
            std::string file;
            sstream >> file;
            tilesheet.loadFromFile(file, TEXTURE_PIXEL_FORMAT_RGBA);
            tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
            tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
            tilesheet.setMinFilter(GL_NEAREST);
            tilesheet.setMagFilter(GL_NEAREST);
        }
        else if (instr == "TILESHEET_NUMTILES") {
            sstream.str(args);
            sstream >> tilesheetSize.x >> tilesheetSize.y;
            tileTexSize = glm::vec2(1.f / tilesheetSize.x, 1.f / tilesheetSize.y);
        }
        else if (line.find("[BEGIN TILES]") != string::npos) {
            char tile[1];
            map = new int[mapSize.x * mapSize.y];
            for (int j = 0; j < mapSize.y; j++) {
                for (int i = 0; i < mapSize.x; i++) {
                    stream.read(tile, 1);
                    map[j * mapSize.x + i] = strtoul(tile, nullptr, 21);
                }
                stream.read(tile, 1);
            }
        }
        std::getline(stream, line);
    }
}

void TileMap::prepareArrays(const glm::vec2& minCoords,
                            ShaderProgram& program) {
    int tile, nTiles = 0;
    glm::vec2 posTile, texCoordTile[2], halfTexel;
    vector<float> vertices;

    halfTexel = glm::vec2(0.5f / tilesheet.width(), 0.5f / tilesheet.height());
    for (int j = 0; j < mapSize.y; j++) {
        for (int i = 0; i < mapSize.x; i++) {
            tile = map[j * mapSize.x + i];
            if (tile != 0) {
                // Non-empty tile
                nTiles++;
                posTile =
                    glm::vec2(minCoords.x + i * tileSize, minCoords.y + j * tileSize);
                texCoordTile[0] =
                    glm::vec2(float((tile - 1) % tilesheetSize.x) / tilesheetSize.x,
                              float((tile - 1) / tilesheetSize.x) / tilesheetSize.y);
                texCoordTile[1] = texCoordTile[0] + tileTexSize;
                texCoordTile[0] += halfTexel;
                texCoordTile[1] -= halfTexel;
                // First triangle
                vertices.push_back(posTile.x);
                vertices.push_back(posTile.y);
                vertices.push_back(texCoordTile[0].x);
                vertices.push_back(texCoordTile[0].y);
                vertices.push_back(posTile.x + blockSize);
                vertices.push_back(posTile.y);
                vertices.push_back(texCoordTile[1].x);
                vertices.push_back(texCoordTile[0].y);
                vertices.push_back(posTile.x + blockSize);
                vertices.push_back(posTile.y + blockSize);
                vertices.push_back(texCoordTile[1].x);
                vertices.push_back(texCoordTile[1].y);
                // Second triangle
                vertices.push_back(posTile.x);
                vertices.push_back(posTile.y);
                vertices.push_back(texCoordTile[0].x);
                vertices.push_back(texCoordTile[0].y);
                vertices.push_back(posTile.x + blockSize);
                vertices.push_back(posTile.y + blockSize);
                vertices.push_back(texCoordTile[1].x);
                vertices.push_back(texCoordTile[1].y);
                vertices.push_back(posTile.x);
                vertices.push_back(posTile.y + blockSize);
                vertices.push_back(texCoordTile[0].x);
                vertices.push_back(texCoordTile[1].y);
            }
        }
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 24 * nTiles * sizeof(float), &vertices[0],
                 GL_STATIC_DRAW);
    posLocation =
        program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
    texCoordLocation = program.bindVertexAttribute(
        "texCoord", 2, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void TileMap::register_physics(b2World& physics) {
    const float tile_size_meters = tileSize * Constants::Units::meters_per_pixel;
    b2BodyDef body_def;
    body_def.position.Set(0.f, 0.f);
    physics_body = physics.CreateBody(&body_def);

    b2BodyUserData data;
    data.pointer = 0xFFFF;
    physics_body->GetUserData() = data;

    for (int j = 0; j < mapSize.y; j++) {
        for (int i = 0; i < mapSize.x; i++) {
            int tile = map[j * mapSize.x + i];
            if (tile != 0) {// Non-empty tile
                float x = i * tile_size_meters;
                float y = j * tile_size_meters;
                b2PolygonShape shape;
                // collision shapes are separated by 1 pixel gap (ghost edge avoidance)
                float offset = Constants::Units::meters_per_pixel;
                float hx = (tile_size_meters - offset) / 2.f;
                float hy = hx;
                shape.SetAsBox(hx, hy, b2Vec2(x, y), 0.f);

                b2FixtureDef fixture_def;
                fixture_def.shape = &shape;
                fixture_def.density = 0.0f;
                fixture_def.friction = 1.f;
                fixture_def.filter.categoryBits = (int)Constants::Physics::Category::Wall;
                fixture_def.filter.maskBits = (int)Constants::Physics::Mask::Wall;
                physics_body->CreateFixture(&fixture_def);
            }

        }
    }
}

glm::vec2 TileMap::get_center() const {
    return glm::vec2(mapSize * tileSize) / 2.f;
}