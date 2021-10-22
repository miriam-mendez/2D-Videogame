#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"
#include <string> 


void Game::init() {
    bPlay = true;
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    scene.init("levels/level" + std::to_string(current_level) + ".txt");
}

bool Game::update(int deltaTime) {
    scene.update(deltaTime);

    return bPlay;
}

void Game::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene.render();
}

void Game::keyPressed(int key) {
    if (key == 27) // Escape code
        bPlay = false;
    else if (key == 114) { // R key
        scene.free();
        scene.init("levels/level" + std::to_string(current_level) + ".txt");
    }
    else if (key == 101) { // E key
        scene.free();
        scene.init("levels/level" + std::to_string(--current_level) + ".txt");
    }
    else if (key == 116) { // T key
        scene.free();
        scene.init("levels/level" + std::to_string(++current_level) + ".txt");
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





