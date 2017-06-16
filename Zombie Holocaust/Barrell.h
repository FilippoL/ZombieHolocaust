#pragma once
#include "ModelManager.h"
class Barrell :
	public ModelManager
{
public:
	Barrell();

	void FixRot();

	void Roll(float dt);

	void Respawn();

	float Randomize(int Between_this, int and_this);

	bool Die();

	virtual ~Barrell();

	bool _dead;

	int r_n;

	float _speed;

	XMVECTOR r1;
	float r;
	float rotation_speed;
};

