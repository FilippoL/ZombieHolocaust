#pragma once
#include "ModelManager.h"
class Ground :
	public ModelManager
{
public:
	Ground();

	void StartMov(float speed, double dt);

	bool & NeedSwap();
	virtual ~Ground();
	bool isit;

	void SetHeight(float H);
	void SetDistanceFromPlayer(float Distance_from_player);

protected:
	float _Height;
	float _Distance;

};

