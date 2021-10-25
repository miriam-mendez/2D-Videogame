#include "SoundSystem.h"
#include<assert.h>
#include "dirent.h"
#include<string>

bool SoundSystem::initialized = false;
System* SoundSystem::system; // Pointer to the FMOD instance
std::unordered_map<char*, Sound*> SoundSystem::sounds;
Channel* SoundSystem::channel = 0;

SoundSystem::SoundSystem() {
    if (!initialized) {
        assert(System_Create(&system) == FMOD_OK);

        int driverCount = 0;
        system->getNumDrivers(&driverCount);
        assert(driverCount != 0);

        // Initialize our Instance with 16 Channels
        system->init(16, FMOD_INIT_NORMAL, nullptr);

        // Call loadAll SFX
        addNewSound("music-loop", "media/music-loop.wav", true);
        addNewSound("piano-loop", "media/piano-loop.wav", true);
        addNewSound("box-water", "media/SFX/box-water.wav", false);
        addNewSound("cactus", "media/SFX/cactus2.wav",false);
        addNewSound("click", "media/SFX/click.wav",false);
        addNewSound("flag", "media/SFX/flag.ogg", false);
        addNewSound("footsteps", "media/SFX/footsteps_player.ogg", true);
        addNewSound("jump", "media/SFX/jump.ogg", false);
        initialized = true;
    }
}

SoundSystem::~SoundSystem() {
    for (auto const& x : sounds) 
        x.second->release();
    system->close();
    system->release();
}

void SoundSystem::addNewSound(char* name, char* filename, bool loop) {
    Sound *sound;
    sounds.emplace(name, sound);
    auto sound2 = sounds.find(name);
    // FMOD_DEFAULT = FMOD_LOOP_OFF, FMOD_2D, FMOD_HARDWARE
    system->createSound(filename, FMOD_DEFAULT, 0, &sound2->second);
    if (loop) {
        sound2->second->setMode(FMOD_LOOP_NORMAL);
        sound2->second->setLoopCount(-1);
    }
}

//void SoundSystem::loadAllSounds(char* path) { // to test
//    struct dirent* entry;
//    DIR* dir = opendir(path);
//    if (dir == NULL)  return;
//
//    while ((entry = readdir(dir)) != NULL) {
//        char* name;
//        std::string filePath = entry->d_name;
//        std::size_t dotPos = filePath.rfind('.');
//        std::size_t sepPos = filePath.rfind('/');
//
//        name = &filePath.substr(sepPos + 1, filePath.size() -  dotPos)[0];
//        addNewSound(name , entry->d_name, false);
//    }
//    closedir(dir);
//}

void SoundSystem::playSound(char*name) {
    auto sound = sounds.find(name);
    system->playSound((*sound).second,0,false, &channel);
}

void SoundSystem::stopSound(char* name) {
    auto sound = sounds.find(name);
    system->playSound((*sound).second, 0, true, &channel);
}

//void SoundSystem::resetMusic(char* name) {
//    auto sound = sounds.find(name);
//    sound->second->release();
//
//    char result[100];
//    strcpy(result, "media/" );
//    strcat(result, sound->first);
//
//    addNewSound(sound->first, result, true);
//    system->playSound((*sound).second, 0, false, &channel);
//}

void SoundSystem::update() {
    system->update();
}

void SoundSystem::releaseSound(char* name) {
    auto sound = sounds.find(name);
    sound-> second ->release();
}