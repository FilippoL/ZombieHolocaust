#pragma once
#include "Mesh.h"
#include "TextureManager.h"

#include <list>
#include <string>
#include <sstream>
#include <vector>


class Animation
{
public:
	Animation();
	~Animation();

	void SetVelocity(float velocity);

	void SetFrameIndex(float ind);

	void LoopBetween(int first, int last);

	void update(float dt);

	int _first, _last;

	int GetFrameIndex();

	bool IsLastFrame();

	float a_speed;

	float _animation_index;
	
	double time_elapsed;

};
