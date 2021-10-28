#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include "Texture.h"
#include "ShaderProgram.h"
#include "Quad.h"

#define NUM_CHARACTERS (128 - 32)


struct CharMetrics {
    int tx, ty; 		// Pos x, y inside texture in pixels
    int sx, sy; 		// Size of char in pixels
    int ax, ay; 		// Advance for each character in a string sequence
    int bl, bt; 		// Bitmap left and top adjusting
};


// Using Freetype the Text class is capable of building a texture atlas
// for a given font and size


class TextQuad : public Quad {

public:
    TextQuad();
    ~TextQuad();

    bool init(const char* filename);
    void destroy();

    ShaderProgram& getProgram();
    int getSize() const;
    virtual void render();
    void set_font_size(int size);
    void set_font_color(glm::vec4 const& color);
    void set_text(std::string const& text);
private:
    void initShaders();
    bool extractCharSizes(int* maxCharWidth, int* maxCharHeight);
    void createTextureAtlas();

private:
    void set_scale(const glm::vec2& scale) = delete;
    void set_rotation(float radians) = delete;

    GLuint vao;
    GLuint vbo;
    GLint posLocation, texCoordLocation;

    int fontSize, textureSize, maxCharWidth, maxCharHeight;
    int render_font_size = 14;
    glm::vec4 tint = glm::vec4(1, 1, 1, 1);
    std::string text;
    FT_Face face;
    CharMetrics chars[NUM_CHARACTERS];
    Texture textureAtlas;
    ShaderProgram program;

    static bool bLibInit;
    static FT_Library library;

};