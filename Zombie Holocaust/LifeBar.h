#pragma once
#include "ModelManager.h"
class LifeBar :
	public ModelManager
{
public:
	LifeBar();
	void Restore();
	void update();
	void Damaged();
	virtual ~LifeBar();

	float life;
};

