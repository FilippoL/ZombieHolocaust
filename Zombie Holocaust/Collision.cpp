#include "Collision.h"



Collision::Collision()
{
}

bool Collision::IsColliding(const Collision& bound)
{
	return ((_max_position.x > bound._min_position.x && bound._max_position.x > _min_position.x) &&
			(_max_position.z > bound._min_position.z && bound._max_position.z > _min_position.z));
}


void Collision::GenerateBound()
{
	_min_position.x = _position.x - W/2;

	_min_position.z = _position.z - H/2;

	_max_position.x = _position.x + W/2;

	_max_position.z = _position.z + H/2;	
}

void Collision::SetBoundSize(float w, float h)
{
	W = w;
	H = h;
}

void Collision::UpdateBoundPosition(float x, float y, float z)
{
	_position.x = x;
	_position.y = y;
	_position.z = z;

	GenerateBound();
}


Collision::~Collision()
{
}
