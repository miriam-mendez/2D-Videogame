#include "Text.h"

void Text::init(std::string const& text, int size, glm::vec4 const& color) {
    assert(text_quad.init("fonts/OpenSans-Regular.ttf"));
    text_quad.set_text(text);
    text_quad.set_position(position);
    text_quad.set_font_size(size);
    text_quad.set_font_color(color);
}

void Text::render() {
    text_quad.render();
}

void Text::update(int deltaTime) {
    text_quad.set_position(position);
}
