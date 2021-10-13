#pragma once
#include "Player.h"

class FlippedPlayer : public Player {
    void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram) override;
    void update(int deltaTime) override;
};

