#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"


void Game::init() {
    bPlay = true;
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

bool Game::update(int deltaTime) {
    if (level_change) {
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
    if (key == 27) // Escape code
        bPlay = false;
    else if (key == 114) { // R key
        delayed_set_level(current_level);
    }
    else if (key == 101) { // E key
        delayed_set_level(prev_level);
    }
    else if (key == 116) { // T key
        delayed_set_level(next_level);
    }
    keys[key] = true;
}

void Game::keyReleased(int key) {
    keys[key] = false;
}

void Game::specialKeyPressed(int key) {
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





