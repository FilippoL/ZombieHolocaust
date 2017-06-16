#pragma once
#include <string>
#include "Singleton.h"
#include <vector>

#include <fmod.hpp>

class AudioManager
{
public:
	friend class Singleton<AudioManager>;

public:
	bool Init();
	bool LoadMusic(const std::string & filename, int TAG);
	bool LoadSFX(const std::string& filename, int TAG);

	void Unload();
	void ShutDown();
	void Update();

public:
	FMOD::Sound *GetMusic(int index) { return m_backg_music[index]; };
	FMOD::Sound *GetSFX(int index) { return m_flashsound[index]; };
	FMOD::System *GetSystem() { return m_system; };


private:
	AudioManager();
	AudioManager(const AudioManager&);
	AudioManager& operator=(const AudioManager&);
	~AudioManager();

private:
	FMOD::System* m_system;
	std::vector<FMOD::Sound*> m_backg_music;
	std::vector<FMOD::Sound*> m_flashsound;

};

typedef Singleton<AudioManager> Audio;