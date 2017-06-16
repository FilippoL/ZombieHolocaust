#pragma once
#include "ModelManager.h"


class Car :
	public ModelManager
{
public:
	Car();

	Collision _point_col[2];

	void Update();

	bool Point_collision(Collision coll);


	virtual ~Car();
};

