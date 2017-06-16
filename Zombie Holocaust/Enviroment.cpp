#include "Enviroment.h"



Enviroment::Enviroment()
{
	position.x = 0;
	position.y = -7.5;
	position.z = 0;

	r = XMVectorSet(0.0,1.0,0.0,0.0);
	_rot_speed = 1.85;
	Scale(10);

}

void Enviroment::update(float dt)
{
	_rot += _rot_speed * dt;
	Rotate(r, _rot );
}


Enviroment::~Enviroment()
{
}
