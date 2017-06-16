#include "Animation.h"
#include <string>
#include <sstream>



Animation::Animation()
{
	time_elapsed = 0.00000f;
	_animation_index = 0;
	a_speed = 1;
	_first = 0;
	_last = 100;
}


Animation::~Animation()
{

}
void Animation::SetVelocity(float v)
{
	a_speed = v;
}

void Animation::SetFrameIndex(float ind)
{
	_animation_index = ind;
}

void Animation::LoopBetween(int first, int last)
{
	_first = first;
	_last = last;
}

void Animation::update(float dt)
{
	time_elapsed += dt * a_speed;

	if (time_elapsed > 0.007f)
	{
		_animation_index += 1;
	
		if (_animation_index > _last-1)
		{
			_animation_index = _first;
		}

		time_elapsed = 0.0000f;
	}
}

int Animation::GetFrameIndex()
{
	return _animation_index;
}

bool Animation::IsLastFrame()
{
	if (_animation_index > _last-2) { return true; }
	else {
		return false;	}
}

