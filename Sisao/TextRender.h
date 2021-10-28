#pragma once
#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"

class TextRender
{

public:
	// Textured quads can only be created inside an OpenGL context
	static TextRender* createTexturedQuad(glm::vec2 geom[2], glm::vec2 texCoords[2], ShaderProgram& program);

	TextRender(glm::vec2 geom[2], glm::vec2 texCoords[2], ShaderProgram& program);

	void render(const Texture& tex) const;
	void free();

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;

};

