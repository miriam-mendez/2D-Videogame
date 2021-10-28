#pragma once

#include "Scene.h"
#include "SoundSystem.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 448

// Game is a singleton (a class with a single instance) that represents our
// whole application

class Game {
    friend Scene;
public:
    Game() {}

    static Game& instance() {
        static Game G;

        return G;
    }

    void init();
    bool update(int deltaTime);
    void render();

    Scene& get_scene() { return *scene; };
    SoundSystem& get_sound_system() { return soundSystem; }

    // Input callback methods
    void keyPressed(int key);
    void keyReleased(int key);
    void specialKeyPressed(int key);
    void specialKeyReleased(int key);
    void mouseMove(int x, int y);
    void mousePress(int button);
    void mouseRelease(int button);

    bool getKey(int key) const;
    bool getSpecialKey(int key) const;

    void delayed_set_level(std::string const& level);
    std::string get_current_level() { return current_level; }
    std::string get_next_level() { return next_level; }
    std::string get_prev_level() { return prev_level; }

private:
    bool bPlay;                       // Continue to play game?
    Scene* scene = nullptr;                      // Scene to render
    bool keys[256], specialKeys[256]; // Store key states so that
                                      // we can have access at any time

    bool level_change = true;
    std::string current_level = "levels/default.txt";
    std::string prev_level = current_level;
    std::string next_level = current_level;
    SoundSystem soundSystem;
};

