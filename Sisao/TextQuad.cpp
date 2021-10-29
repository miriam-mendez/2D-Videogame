#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include "TextQuad.h"
#include "Game.h"

#define ATLAS_FONT_SIZE 64


using namespace std;


bool TextQuad::bLibInit = false;
FT_Library TextQuad::library;


TextQuad::TextQuad() {}

TextQuad::~TextQuad() {
    destroy();
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vao);
}


bool TextQuad::init(std::string const& filename, ShaderProgram* program) {
    this->program = program;
    FT_Error error;

    if (!bLibInit) {
        error = FT_Init_FreeType(&TextQuad::library);
        if (error)
            return false;
        bLibInit = true;
    }
    error = FT_New_Face(TextQuad::library, filename.c_str(), 0, &face);
    if (error)
        return false;
    FT_Set_Pixel_Sizes(face, ATLAS_FONT_SIZE, ATLAS_FONT_SIZE);

    if (!extractCharSizes(&maxCharWidth, &maxCharHeight))
        return false;
    fontSize = maxCharHeight;
    textureSize = 512;
    if (floor(float(textureSize) / maxCharWidth) * floor(float(textureSize) / maxCharHeight) < (128 - 32))
        textureSize = 1024;
    if (floor(float(textureSize) / maxCharWidth) * floor(float(textureSize) / maxCharHeight) < (128 - 32))
        return false;
    createTextureAtlas();

    glm::vec2 geom[2] = { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f) };
    glm::vec2 texCoords[2] = { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f) };

    float vertices[24] = { geom[0].x, geom[0].y, texCoords[0].x, texCoords[0].y,
                                                geom[1].x, geom[0].y, texCoords[1].x, texCoords[0].y,
                                                geom[1].x, geom[1].y, texCoords[1].x, texCoords[1].y,
                                                geom[0].x, geom[0].y, texCoords[0].x, texCoords[0].y,
                                                geom[1].x, geom[1].y, texCoords[1].x, texCoords[1].y,
                                                geom[0].x, geom[1].y, texCoords[0].x, texCoords[1].y };


    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_STATIC_DRAW);
    posLocation = program->bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
    texCoordLocation = program->bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    return true;
}

void TextQuad::destroy() {
    FT_Done_Face(face);
}


int TextQuad::getSize() const {
    return fontSize;
}


void TextQuad::render() {
    int vp[4];
    glm::vec2 minTexCoord, maxTexCoord, pos = position;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    program->use();
    glGetIntegerv(GL_VIEWPORT, vp);
    auto projection = Game::instance().get_scene().get_camera().projection_matrix();
    program->setUniformMatrix4f("projection", projection);
    program->setUniform4f("color", tint.r, tint.g, tint.b, tint.a);

    for (unsigned int i = 0; i < text.length(); i++) {
        glm::mat4 modelview = glm::mat4(1.0f);
        modelview = glm::translate(modelview, glm::vec3(pos.x + (float(render_font_size) / fontSize) * chars[text[i] - 32].bl * scale.x, pos.y - (float(render_font_size) / fontSize) * chars[text[i] - 32].bt * scale.y, 0.f));
        auto rotation_mat = glm::rotate(glm::mat4(1), rotation, glm::vec3(0.f, 0.f, 1.f));
        rotation_mat = rotation_mat * flipVH;
        modelview = glm::scale(modelview * rotation_mat, (float(render_font_size) / fontSize) * glm::vec3(chars[text[i] - 32].sx, chars[text[i] - 32].sy, 0.f) * glm::vec3(scale, 0.f));
        auto view = Game::instance().get_scene().get_camera().view_matrix();
        program->setUniformMatrix4f("modelview", view * modelview);
        minTexCoord = glm::vec2(float(chars[text[i] - 32].tx) / textureSize, float(chars[text[i] - 32].ty) / textureSize);
        maxTexCoord = glm::vec2(float(chars[text[i] - 32].tx + chars[text[i] - 32].sx) / textureSize, float(chars[text[i] - 32].ty + chars[text[i] - 32].sy) / textureSize);
        program->setUniform2f("minTexCoord", minTexCoord.s, minTexCoord.t);
        program->setUniform2f("maxTexCoord", maxTexCoord.s, maxTexCoord.t);

        glEnable(GL_TEXTURE_2D);
        textureAtlas.use();
        glBindVertexArray(vao);
        glEnableVertexAttribArray(posLocation);
        glEnableVertexAttribArray(texCoordLocation);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisable(GL_TEXTURE_2D);

        pos.x += (float(render_font_size) / fontSize) * chars[text[i] - 32].ax;
    }

    glDisable(GL_BLEND);
}

void TextQuad::set_font_size(int size) {
    render_font_size = size;
}

void TextQuad::set_font_color(glm::vec4 const& color) {
    tint = color;
}

void TextQuad::set_text(std::string const& text) {
    this->text = text;
}


bool TextQuad::extractCharSizes(int* maxCharWidth, int* maxCharHeight) {
    unsigned char c;

    *maxCharWidth = 0;
    *maxCharHeight = 0;
    for (c = 32; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            return false;
        *maxCharWidth = glm::max(*maxCharWidth, (int)face->glyph->bitmap.width);
        *maxCharHeight = glm::max(*maxCharHeight, (int)face->glyph->bitmap.rows);
    }

    return true;
}

void TextQuad::createTextureAtlas() {
    unsigned char c;
    int x = 0, y = 0;

    textureAtlas.createEmptyTexture(textureSize, textureSize);
    for (c = 32; c < 128; c++) {
        FT_Load_Char(face, c, FT_LOAD_RENDER);
        chars[c - 32].tx = x;
        chars[c - 32].ty = y;
        chars[c - 32].sx = face->glyph->bitmap.width;
        chars[c - 32].sy = face->glyph->bitmap.rows;
        chars[c - 32].ax = face->glyph->advance.x >> 6;
        chars[c - 32].ay = face->glyph->advance.y >> 6;
        chars[c - 32].bl = face->glyph->bitmap_left;
        chars[c - 32].bt = face->glyph->bitmap_top;
        textureAtlas.loadSubtextureFromGlyphBuffer(face->glyph->bitmap.buffer, x, y, face->glyph->bitmap.width, face->glyph->bitmap.rows);
        x += maxCharWidth;
        if ((x + maxCharWidth) >= textureSize) {
            x = 0;
            y += maxCharHeight;
        }
    }
    textureAtlas.generateMipmap();
    textureAtlas.setWrapS(GL_CLAMP_TO_EDGE);
    textureAtlas.setWrapT(GL_CLAMP_TO_EDGE);
}
