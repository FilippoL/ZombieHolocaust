#pragma once
#include <string>
#include "Singleton.h"

#include <fmod.hpp>
class AudioComponent
{
public:
	AudioComponent();

	void SetMusic(int which);
	void SetSFX(int which);

	void Play();
	void Pause();
	void Stop();
	void Resume();

	void SetPan(float pan);
	~AudioComponent();


private:
	float m_volume;

	FMOD::Sound* m_audioData;
	FMOD::Channel* m_channel;
	FMOD::ChannelGroup* m_channelgroup;

};

