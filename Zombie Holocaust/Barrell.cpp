#include "Barrell.h"
#include <time.h>


Barrell::Barrell()
{
	r1 = XMVectorSet(0.0, 0.0, 1.0, 0.0);
	position.z = -150;
	position.x = -155;

	_aabb.SetBoundSize(35,45);
	rotation_speed = -3.5;

	
}

void Barrell::Roll(float dt)
{
	
	_aabb.UpdateBoundPosition(position.x,position.y,position.z);


	r += rotation_speed * dt;
	Rotate(r1, r);

	MoveLeftRight(_speed * dt);

}

void Barrell::Respawn()
{
	
	if (Randomize(1,2) == 1)
	{
		position = XMFLOAT3(-150.0, 0.0, Randomize(200, 500));
		_speed = 1;
		rotation_speed = -3.5;

	}

	else
	{
		position = XMFLOAT3(150.0, 0.0, Randomize(200, 500));
		_speed = -1;
		rotation_speed = 3.5;

	}
	position.y = 12.5;


}

float Barrell::Randomize(int Between_this, int and_this) 
{
	return Between_this + rand() % and_this;
}

bool Barrell::Die()
{


	return true;
}


Barrell::~Barrell()
{
}
