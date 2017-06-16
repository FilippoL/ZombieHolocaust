#include "Car.h"



Car::Car()
{
	position.x = 0.0;
	position.y = 0.0;
	position.z = 0.0;

	_aabb.SetBoundSize(50, 5);
	_point_col[0].SetBoundSize(15,75);
	_point_col[1].SetBoundSize(15,75);

	Scale(1.5);

	_life = 10;
	
	_isAlive = true;

}


void Car::Update()
{
	_aabb.UpdateBoundPosition(position.x, position.y, position.z + 40);
	_point_col[0].UpdateBoundPosition(position.x + 20, position.y, position.z);
	_point_col[1].UpdateBoundPosition(position.x - 20, position.y, position.z);
}

bool Car::Point_collision(Collision coll)
{
		if (_point_col[0].IsColliding(coll) || _point_col[1].IsColliding(coll) )
	{
		return true;
	}

		else { return false; }
	

}


Car::~Car()
{
}
