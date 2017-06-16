#include "Ground.h"



Ground::Ground()
{
	position.x = 0.0;
	position.y = 0.0;
	position.z = 0.0;

	Scale(1.25,1.0,1.0);
}

void Ground::StartMov(float speed, double dt)
{
	MoveUpAndDown(speed, dt);
	
	if (position.z < (-(_Height/2) - _Distance))
	{
		NeedSwap() = true;
		position.z = (_Height + _Height/2) - _Distance;
	}

}

bool & Ground::NeedSwap()
{

	return isit;
}

Ground::~Ground()
{
}

void Ground::SetHeight(float H)
{
	_Height = H;
}

void Ground::SetDistanceFromPlayer(float Distance_from_player)
{
	_Distance = Distance_from_player;
}
