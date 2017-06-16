#pragma once
#include <d3dx11.h>
#include <xnamath.h>

class Collision
{
public:
	Collision();

	bool IsColliding(const Collision & bound);



	void SetBoundSize(float w, float h);

	void UpdateBoundPosition(float x, float y, float z);

	~Collision();

private:

	void GenerateBound();
	XMFLOAT3 _position;
	XMFLOAT3 _max_position;
	XMFLOAT3 _min_position;
	
	int W, H, D;
};

