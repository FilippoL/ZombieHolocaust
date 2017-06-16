#include "AudioManager.h"
#include <iostream>



AudioManager::AudioManager()
{
	m_system = nullptr;
	m_backg_music.resize(10);
	m_flashsound.resize(10);

}


bool AudioManager::Init()
{
	System_Create(&m_system);
	m_system->init(100, FMOD_INIT_NORMAL, NULL);
	if (!m_system)
	{
		std::cout << "Audio not intialised!" << std::endl;
		return false;
	}

	return true;
}

bool AudioManager::LoadMusic(const std::string & filename, int TAG)
{

	if (!m_system->createStream(filename.c_str(), FMOD_LOOP_NORMAL, 0, &m_backg_music[TAG]))
	{
		std::cout << "Music not loaded!" << std::endl;
		//return false;
	}

	return true;
}

bool AudioManager::LoadSFX(const std::string & filename, int TAG)
{
	

	if (!m_system->createSound(filename.c_str(), FMOD_DEFAULT, 0, &m_flashsound[TAG]))
		{
			std::cout << "SFX not loaded!" << std::endl;
			return false;

		}

	return true;
}

void AudioManager::Unload()
{
	for (size_t i = 0; i < m_backg_music.size(); i++)
	{
		m_backg_music[i]->release();
	}

	for (size_t i = 0; i < m_flashsound.size(); i++)
	{
		m_flashsound[i]->release();
	}
	m_backg_music.clear();
	m_flashsound.clear();
}

void AudioManager::ShutDown()
{
	m_system->release();
}

void AudioManager::Update()
{

	m_system->update();
}

AudioManager::~AudioManager()
{
}
