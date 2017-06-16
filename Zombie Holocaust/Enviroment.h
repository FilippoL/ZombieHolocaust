#pragma once
#include "ModelManager.h"
class Enviroment :
	public ModelManager
{
public:
	Enviroment();
	void update(float dt);

	~Enviroment();
	float _rot_speed;
	float _rot;
	XMVECTOR r;
};

