#include "Enemy.h"

Enemy::Enemy()
{
	position = XMFLOAT3(0.0,0.0,350.0);
	_aabb.SetBoundSize(15, 10);

	_animation.SetFrameIndex(0);
	counter = 50;
	_life = 10;
	_isAlive = true;
	_scale = 1;
	
	_anim_state_index = RUNNING;
}

void Enemy::SetDifficulty(int dif)
{
	if (dif == 1)
	{
		counter = 50;
		_speed = 2.5;
	}
	else if (dif == 2)
	{
		counter = 35;
		_speed = 3.0;

	}
	else
	{
		counter = 25;
		_speed = 4.0;

	}

}

void Enemy::Spawn(float spawn_ref_point, float central_spawning, bool if_player_didnt_kill)
{
	(if_player_didnt_kill) ? accelleration += _speed*2 : accelleration += _speed;
	
	counter -= 3.5;

	float cap;
	if (central_spawning < -75 || central_spawning > 75) //because the position is based on previous zombie location
	{
		cap = central_spawning * -1; //if it goes off road, make the respawn value negative (so on the other side of the road)
	}
	else
	{
		cap = central_spawning;
	}

	position = XMFLOAT3(cap + Randomize(-150, 150),0.0, (spawn_ref_point ) + Randomize(750 + abs(counter) , 1500 + abs(counter)));
	//also base the z on the player z


	_originalpos = position.z;
}


void Enemy::GoToward(XMFLOAT3 target)
{
	if (_anim_state_index == RUNNING )
	{
		_animation.LoopBetween(_running_first,_running_last);

		if (position.x > target.x)
		{
			direction = XMVectorSet((target.x + 20) - position.x, 0.0f, target.z - position.z, 0.0f);
		}
		else
		{
			direction = XMVectorSet((target.x - 20) - position.x, 0.0f, target.z - position.z, 0.0f);
		}

			
		(counter < 15) ? counter = 15 : counter = counter; //never make it impossible

		(position.z < target.z - 100) ? accelleration -= 0.5 : accelleration = accelleration;
		(position.z < target.z - 350) ? accelleration += 1 : accelleration = accelleration;
	
		accelleration = ((_originalpos - target.z) -  (position.z - target.z))/counter; //the more far they come from, the more speed they will have
	
		if (OnCar)
		{
			_animation.SetFrameIndex(_attacking_first);
			_animation.LoopBetween(_attacking_first, _attacking_last);
			_anim_state_index = ATTACK;
		}

		if (!_isAlive)
		{
			_animation.SetFrameIndex(_dying_first);
			_animation.LoopBetween(_dying_first, _dying_last);
			_anim_state_index = DEAD;
		}
	}

	if (_anim_state_index == ATTACK)
	{

		if (position.x > target.x)
		{
			direction = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);//look left
			position.x  = target.x + 35;
		}
		else
		{
			direction = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);//look right
			position.x = target.x - 35;
		}

		if (position.z > (target.z + 20)) { position.z = target.z + 25; }
		else if (position.z < (target.z - 20)){	position.z = target.z - 25;	}
		else if (position.z > (target.z - 20) && position.z < (target.z - 10)) { position.z = target.z - 15; }
		else if (position.z < (target.z + 20) && position.z > (target.z + 10)) { position.z = target.z + 15; }
		else if (position.z > (target.z - 40) && position.z < (target.z - 35)) { position.z = target.z - 30; }
		else if (position.z < (target.z + 40) && position.z > (target.z + 35)) { position.z = target.z + 30; }
		else { position.z = target.z; }

		
		if (!_isAlive)
		{
			_animation.SetFrameIndex(_dying_first);
			_animation.LoopBetween(_dying_first, _dying_last);
			_anim_state_index = DEAD;
			OnCar = false;
		}

	}

	if (_anim_state_index == DEAD)
	{
		if (_isAlive == true)
		{
			_animation.SetFrameIndex(_running_first);
			_animation.LoopBetween(_running_first , _running_last);
			_anim_state_index = RUNNING;
		}

	}

	UpdateRotationM();

}

bool &Enemy::IsOnCar()
{
	return OnCar;
}

void Enemy::SetLife(float life)
{
	_life = life;

}

void Enemy::update(float dt)
{	
	if (_anim_state_index == RUNNING)
	{
		Scale(_scale);

		position.x += dt *( XMVectorGetX(direction) * accelleration) ;
		position.z += dt *( XMVectorGetZ(direction) * accelleration) ;
	

		_aabb.UpdateBoundPosition(position.x, position.y, position.z);
		_animation.SetVelocity((anim_velocity * (accelleration * 15)) * dt);
	}

	if (_anim_state_index == ATTACK)
	{

		_aabb.UpdateBoundPosition(position.x, position.y, position.z);
		_animation.SetVelocity((anim_velocity  *  150) * dt);
		Scale(_scale);
	}


	if (_anim_state_index == DEAD)
	{
		_aabb.UpdateBoundPosition(position.x, position.y, position.z);
		_animation.SetVelocity((anim_velocity  *150) * dt);
		Scale(_scale);
	}

	
	_animation.update(dt);
}

void Enemy::SetScaleFactor(float scale)
{
	_scale = scale;

}

void Enemy::OneTimeAnimation()
{
	if ( _animation.IsLastFrame())
	{
		anim_velocity = 0.0;
	}

}

void Enemy::SetAnimationVelocity(float anim_v)
{
	anim_velocity = anim_v;// 0.15;
}

void Enemy::Reset()
{
	counter = 75;
	_life = 10;
	accelleration = 0;
	_isAlive = true;
	OnCar = false;
	position.z = -500;
	_anim_state_index = RUNNING;

}


int Enemy::GetFrame()
{
	return _animation.GetFrameIndex();
	
}

float Enemy::Randomize(int Between_this, int and_this) 
{
	return Between_this + rand() % and_this;
}

void Enemy::SetDyingIndex(int FIRST, int LAST)
{
	_dying_first = FIRST;
	_dying_last = LAST;

}

void Enemy::SetAttackingIndex(int FIRST, int LAST)
{
	_attacking_first = FIRST;
	_attacking_last = LAST;

}

void Enemy::SetRunningIndex(int FIRST, int LAST)
{
	_running_first = FIRST;
	_running_last = LAST;

}

Enemy::~Enemy()
{

}
