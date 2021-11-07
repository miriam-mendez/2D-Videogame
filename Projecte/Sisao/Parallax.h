#pragma once
#include "Object.h"
#include <vector>
#include "Sprite.h"

class Parallax :
    public Object {
    struct layer_data {
        layer_data(Texture* t, Sprite* s, float r)
            : tex(t), sprite(s), relation(r) {}
        Texture* tex;
        Sprite* sprite = nullptr;
        float relation = 1.f;
    };
    float max_scroll = 512.f;
    std::vector<layer_data> layers;
    ShaderProgram* shader = nullptr;
public:
    virtual ~Parallax();

    static Parallax* init(std::string const& background_path,
                          ShaderProgram* shader);
    void add_layer(std::string const& layer_path, float relation);

    virtual void update(int deltaTime);
    virtual void render();
protected:
    Parallax(std::string const& background_path,
             ShaderProgram* shader);
};

