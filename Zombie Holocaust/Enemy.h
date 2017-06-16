#pragma once
#include "ModelManager.h"
#include "Animation.h"
#include "Collision.h"
#include <vector>

class Enemy :
	public ModelManager
{
public:
	Enemy();
	void SetDifficulty(int dif);
	
	enum WhatAnimation {RUNNING , ATTACK , DEAD };

	void Spawn(float spawn_ref_point, float central_spawning, bool if_player_didnt_kill = false);
	
	void GoToward(XMFLOAT3 target);

	bool &IsOnCar();
	
	Animation  _animation;

	void SetLife(float life);

	void update(float dt);

	void SetScaleFactor(float scale);

	void OneTimeAnimation();

	void SetAnimationVelocity(float anim_v);

	void Reset();

	float Randomize(int Between_this, int and_this);

	void SetDyingIndex(int FIRST, int LAST);

	void SetAttackingIndex(int FIRST, int LAST);

	void SetRunningIndex(int FIRST, int LAST);



	
	virtual ~Enemy();
	
	int GetFrame();


private:

	bool OnCar;

	int _running_first;
	int _running_last;


	int _attacking_first;
	int _attacking_last;


	int _dying_first;
	int _dying_last;


	float counter;

	float anim_velocity;
	float _scale;
	float _originalpos;

	float _speed;

	float accelleration;

	XMVECTOR _rotY;

	WhatAnimation _anim_state_index;


};

