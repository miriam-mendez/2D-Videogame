#pragma once
#include "Object.h"
#include "TextQuad.h"

class Text :
    public Object {
    TextQuad text_quad;
public:
    Text(Object::uuid_t uuid) : Object(uuid) {}

    virtual void init(std::string const& text, int size, glm::vec4 const& color,
                      ShaderProgram& shader);
    void render() override;
    void update(int deltaTime) override;
};

