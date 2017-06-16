#pragma once
#include "ModelManager.h"
#include "Collision.h"

class Wall :
	public ModelManager
{
public:
	Wall();

	
	void update(float dt);
	float Randomize(int Between_this, int and_this);
	virtual ~Wall();

};

