#include "SoundSystem.h"
#include<assert.h>


SoundSystem::SoundSystem() {
    assert(System_Create(&system) == FMOD_OK);

    int driverCount = 0;
    system->getNumDrivers(&driverCount);
    assert(driverCount != 0);

    // Initialize our Instance with 16 Channels
    system->init(num_channels, FMOD_INIT_NORMAL, nullptr);
}

SoundSystem::~SoundSystem() {
    for (auto const& x : sounds)
        x.second->release();
    system->close();
    system->release();
}

void SoundSystem::addNewSound(std::string const& path, std::string const& name,
                              std::string const& group_name, bool loop) {
    auto it = sounds.find(name);
    if (it == sounds.end()) {
        FMOD::Sound* sound;
        // FMOD_DEFAULT = FMOD_LOOP_OFF, FMOD_2D, FMOD_HARDWARE
        system->createSound(path.c_str(), FMOD_DEFAULT, 0, &sound);
        auto group = get_or_create_group(group_name);
        sound->setSoundGroup(group);
        if (loop) {
            sound->setMode(FMOD_LOOP_NORMAL);
            sound->setLoopCount(-1);
        }
        sounds.emplace(name, sound);
    }
}


void SoundSystem::playSound(std::string const& name, uint32_t channel) {
    FMOD::Channel* chan;
    system->getChannel(channel, &chan);
    auto sound = sounds.find(name);
    assert(sound != sounds.end());
    system->playSound(sound->second, 0, false, &chan);
}

void SoundSystem::set_group_volume(std::string const& group, float volume) {
    auto it = groups.find(group);
    if (it != groups.end()) {
        it->second->setVolume(volume);
    }
}

float SoundSystem::get_group_volume(std::string const& group) {
    auto it = groups.find(group);
    float v = 0.f;
    if (it != groups.end()) {
        it->second->getVolume(&v);
        return v;
    }
    return v;
}

void SoundSystem::stop_group_sounds(std::string const& group) {
    auto it = groups.find(group);
    if (it != groups.end()) {
        it->second->stop();
    }
}

int SoundSystem::get_playing_sounds(std::string const& group) {
    int result = 0;
    auto it = groups.find(group);
    if (it != groups.end())
        it->second->getNumPlaying(&result);
    return result;
}

void SoundSystem::update(int deltaTime) {
    system->update();
}

FMOD::SoundGroup* SoundSystem::get_or_create_group(std::string const& name) {
    FMOD::SoundGroup* group;
    auto it = groups.find(name);
    if (it == groups.end()) {
        system->createSoundGroup(name.c_str(), &group);
        it = groups.emplace(name, group).first;
    }
    return it->second;
}

void SoundSystem::releaseSound(std::string const& name) {
    auto sound = sounds.find(name);
    if (sound != sounds.end()) {
        sound->second->release();
        sounds.erase(sound);
    }
}