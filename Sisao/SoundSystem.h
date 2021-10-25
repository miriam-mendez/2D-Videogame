#pragma once
#include "fmod.hpp"
#include<unordered_map>
#include <string>

class SoundSystem {

public:
    SoundSystem();
    ~SoundSystem();

    void addNewSound(std::string const& path, std::string const& name,
                     std::string const& group = "default", bool loop = false);
    void playSound(std::string const& name, uint32_t channel = 0);

    void update();
    void releaseSound(std::string const& name);

private:
    FMOD::SoundGroup* get_or_create_group(std::string const& name);

    uint32_t num_channels = 16;
    FMOD::System* system = nullptr; // Pointer to the FMOD instance

    std::unordered_map<std::string, FMOD::SoundGroup*> groups;
    std::unordered_map<std::string, FMOD::Sound*> sounds;
};


