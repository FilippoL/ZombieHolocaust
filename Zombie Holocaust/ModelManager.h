#pragma once
#include "TextureManager.h"
#include "Mesh.h"
#include "Collision.h"

#include <d3dx11.h>
#include <xnamath.h>
#include <stdio.h>
#include <string>

class ModelManager
{
public:
	

	ModelManager();
	virtual void Initialise(std::string txture, 
							std::string Path_to_OBJ,  
							ID3D11Device * DeviceName, 
							int number_frames = 1);

	void SetPosition(float x, float y, float z);

	XMFLOAT3 GetPosition();
	
	Mesh * GetMesh();
	TextureManager * GetTexture();
	XMMATRIX * GetMatrix();
	Collision GetBound();

	void Rotate(XMVECTOR whichax, float degree);
	void MoveUpAndDown(float speed, double dt);

	bool & isBehind();

	void UpdateWorld();
	void MoveLeftRight(float speed);
	void Scale(float x, float y, float z);
	void Scale(float scaleammount);

	void UpdateRotationM();

	float GetLife();
	void SetAlive(bool alive);


	void Unload();
	XMVECTOR scaleby;
	XMVECTOR direction;
	bool IsAlive();


	~ModelManager();

protected:
	
	XMFLOAT3 position;
	int nFrame;
	Collision _aabb;

	float	 _life;
	bool	 _isAlive;


protected:
	TextureManager * _texture;
	Mesh * _mesh;
	

	XMVECTOR DefaultUp;
	XMVECTOR DefaultForward;
	XMVECTOR DefaultRight;


	XMMATRIX _world;
	XMMATRIX _scale;
	XMMATRIX _rotation;
	XMMATRIX _translation;

	bool behind;
	

};


