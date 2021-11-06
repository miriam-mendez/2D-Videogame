#pragma once
#include "fmod.hpp"
#include<unordered_map>
#include <string>

class SoundSystem {

public:
    SoundSystem();
    ~SoundSystem();

    void addNewSound(std::string const& path, std::string const& sound,
                     std::string const& group = "default", bool loop = false);
    void playSound(std::string const& sound, uint32_t channel = 0);

    void set_group_volume(std::string const& group, float volume);
    float get_group_volume(std::string const& group);
    void stop_group_sounds(std::string const& group);

    int get_playing_sounds(std::string const& group);

    void update(int deltaTime);
    void releaseSound(std::string const& name);

private:
    FMOD::SoundGroup* get_or_create_group(std::string const& name);

    uint32_t num_channels = 16;
    FMOD::System* system = nullptr;

    std::unordered_map<std::string, FMOD::SoundGroup*> groups;
    std::unordered_map<std::string, FMOD::Sound*> sounds;
};


