#pragma once
#include "fmod.hpp"
#include<unordered_map>

using namespace FMOD;

class SoundSystem {

public:
	SoundSystem();
	~SoundSystem();

	void addNewSound(char* name, char* filename, bool loop = false);
	/*void loadAllSound(char* path);*/

	void playSound(char* name);
	void stopSound(char* name);
	/*void resetMusic(char* name);*/

	void update();
	void releaseSound(char* name);

private:
	static bool initialized;
	static System* system; // Pointer to the FMOD instance
	static std::unordered_map<char*, Sound*> sounds; 
	static Channel* channel; 
};


