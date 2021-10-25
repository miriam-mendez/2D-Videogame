#include "Parallax.h"
#include <algorithm>
#include "Game.h"

Parallax::~Parallax() {
    for (auto const& layer : layers) {
        delete layer.tex;
        delete layer.sprite;
    }
}

Parallax::Parallax(std::string const& background_path, ShaderProgram* shader) {
    this->shader = shader;
    spritesheet.loadFromFile(background_path, TEXTURE_PIXEL_FORMAT_RGBA);
    auto size = glm::vec2(spritesheet.width(), spritesheet.height());
    sprite = Sprite::init(size, glm::vec2(1, 1), &spritesheet, shader);
    sprite->set_position(position);
    max_scroll = size.x;
}

Parallax* Parallax::init(std::string const& background_path,
                         ShaderProgram* shader) {
    return new Parallax(background_path, shader);
}

void Parallax::add_layer(std::string const& layer_path, float relation) {
    auto tex = new Texture();
    tex->loadFromFile(layer_path, TEXTURE_PIXEL_FORMAT_RGBA);
    auto size = glm::vec2(tex->width(), tex->height());
    Sprite* s = Sprite::init(size, glm::vec2(1, 1), tex, shader);
    auto it = std::lower_bound(layers.begin(), layers.end(), glm::abs(relation),
                               [](layer_data const& a, float const& b) {
                                   return glm::abs(a.relation) < b;
                               });
    it = layers.emplace(it, tex, s, relation);
}

void Parallax::update(int deltaTime) {
    auto& cam = Game::instance().get_scene().get_camera();
    auto cam_x = cam.get_position().x;
    float x0 = position.x;
    float r = (cam_x - x0) / max_scroll;
    if (r < -1.f || r > 1.f) return;

    for (auto const& layer : layers) {
        float layer_max_scroll = max_scroll * glm::abs(layer.relation);
        float layer_x0 = position.x;
        float x_pos = layer_x0 + layer_max_scroll * -r;
        auto pos = glm::vec2(x_pos, position.y);
        layer.sprite->set_position(pos);
    }
}

void Parallax::render() {
    sprite->render();
    for (auto const& layer : layers) {
        layer.sprite->render();
    }
}


