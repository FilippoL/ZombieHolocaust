#include "LifeBar.h"



LifeBar::LifeBar()
{
	Scale(life, 3.5, 1);

	_isAlive = true;
	life = 75;
}

void LifeBar::Restore()
{
	_isAlive = true;
	life = 100;
}


void LifeBar::update()
{
	Scale(life, 3.5, 1);

	if (life < 0)
	{
		_isAlive = false;
	}
}
void LifeBar::Damaged()
{
	life = life - 2.75;
}


LifeBar::~LifeBar()
{
}
