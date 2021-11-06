#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"
#include "Constants.h"

void Game::init() {
    bPlay = true;
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    clean_input();
}

bool Game::update(int deltaTime) {
    if (level_change) {
        clean_input();
        delete scene;
        scene = new Scene();
        level_change = false;
        scene->init(current_level);
    }
    scene->update(deltaTime);
    return bPlay;
}

void Game::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!level_change)
        scene->render();
}

void Game::keyPressed(int key) {
    if (key == Constants::Keys::Esc)
        bPlay = false;
    else if (key == Constants::Keys::R) {
        delayed_set_level(current_level);
    }
    else if (key == Constants::Keys::_1) {
        delayed_set_level("levels/level1.txt");
    }
    else if (key == Constants::Keys::_2) {
        delayed_set_level("levels/level2.txt");
    }
    else if (key == Constants::Keys::_3) {
        delayed_set_level("levels/level3.txt");
    }
    else if (key == Constants::Keys::_4) {
        delayed_set_level("levels/level4.txt");
    }
    else if (key == Constants::Keys::_5) {
        delayed_set_level("levels/level5.txt");
    }
    keys[key] = true;
}

void Game::keyReleased(int key) {
    keys[key] = false;
}

void Game::specialKeyPressed(int key) {
    if (key == Constants::Keys::ArrowLeft) {
        delayed_set_level(prev_level);
    }
    else if (key == Constants::Keys::ArrowRight) {
        delayed_set_level(next_level);
    }
    specialKeys[key] = true;
}

void Game::specialKeyReleased(int key) {
    specialKeys[key] = false;
}

void Game::mouseMove(int x, int y) {}

void Game::mousePress(int button) {}

void Game::mouseRelease(int button) {}

bool Game::getKey(int key) const {
    return keys[key];
}

bool Game::getSpecialKey(int key) const {
    return specialKeys[key];
}

void Game::delayed_set_level(std::string const& level) {
    current_level = level;
    level_change = true;
}

void Game::clean_input() {
    for (int i = 0; i < 256; ++i) {
        keys[i] = false;
        specialKeys[i] = false;
    }
}
