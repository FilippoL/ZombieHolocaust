#include "Wall.h"



Wall::Wall()
{
	position.x = 0.0;
	position.y = 0.0;
	position.z = -175.0;
	XMVECTOR rot;
	rot = XMVectorSet(0.0,1.0,0.0,0.0);
	Rotate(rot, 90);
	Scale(0.075, 0.075, 0.065);
			
	_aabb.SetBoundSize(165, 60);
}

void Wall::update(float dt)
{
	if (position.z < -350)
	{
		position.z = Randomize(555, 800);
		(int(position.z) % 2 == 0) ? position.x = 85 : position.x = -85;
	}

	_aabb.UpdateBoundPosition(position.x, position.y, position.z);

	UpdateWorld();
}


float Wall::Randomize(int Between_this, int and_this)
{
	return Between_this + rand() % and_this;
}


Wall::~Wall()
{
}
