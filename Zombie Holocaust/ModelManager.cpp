#include "ModelManager.h"


#include <d3dx11.h>
#include <xnamath.h>
#include <stdio.h>
#include <string>



ModelManager::ModelManager()
{
	_texture = new TextureManager;
	_mesh = new Mesh;

	_world = XMMatrixIdentity();
	_rotation = XMMatrixIdentity();
	_translation =  XMMatrixIdentity();
	_scale = XMMatrixIdentity();

	scaleby = XMVectorSet(1,1,1,1);

	DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DefaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
}

void ModelManager::Initialise(std::string txture, std::string Path_to_OBJ, ID3D11Device * DeviceName,  int number_frames)
{
	nFrame = number_frames;
	_mesh->LoadMesh(Path_to_OBJ, DeviceName, number_frames);
	_texture->TextureLoader(txture.c_str(),DeviceName);
}

bool ModelManager::IsAlive()
{

	return _isAlive;
}
void ModelManager::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	UpdateWorld();
}

XMFLOAT3 ModelManager::GetPosition()
{
	return position;
}

Mesh * ModelManager::GetMesh()
{
	return _mesh;
}

TextureManager * ModelManager::GetTexture()
{
	return _texture;
}

void ModelManager::Rotate(XMVECTOR whichax, float degrees)
{
	//_rotation = XMMatrixIdentity();

	_rotation = XMMatrixRotationAxis(whichax, XMConvertToRadians(degrees));
	UpdateWorld();
}

bool &ModelManager::isBehind()
{
	return behind;
}

void ModelManager::UpdateWorld()
{	
	_scale = XMMatrixScalingFromVector(scaleby);
	
	_translation = XMMatrixTranslation(position.x,position.y,position.z);
	
	_world =  _rotation * _scale * _translation;

	_world = XMMatrixTranspose(_world);
}

void ModelManager::MoveUpAndDown(float speed, double dt)
{
	position.z += speed * dt;
	UpdateWorld();
}

void ModelManager::MoveLeftRight(float speed)
{
	position.x += speed;
	UpdateWorld();
}

void ModelManager::Scale(float x, float y, float z)
{
	scaleby = XMVectorSet(x,y,z, 1);
	UpdateWorld();


}

void ModelManager::Scale(float scaleammount)
{
	scaleby = XMVectorSet(scaleammount,scaleammount,scaleammount, 1);
	UpdateWorld();

}


void ModelManager::UpdateRotationM()
{
	direction = XMVector3Normalize(direction);
	
	XMVECTOR directionInvertX = XMVectorSet(-1.0f*XMVectorGetX(direction), XMVectorGetY(direction), XMVectorGetZ(direction), 0.0f); 
		
	XMVECTOR xaxis = XMVector3Cross(DefaultUp, directionInvertX);

	xaxis = XMVector3Normalize(xaxis);

	_rotation = XMMatrixSet
	(	XMVectorGetX(xaxis), XMVectorGetX(DefaultUp), XMVectorGetX(directionInvertX), 0.0f,
		XMVectorGetY(xaxis), XMVectorGetY(DefaultUp), XMVectorGetY(directionInvertX), 0.0f,
		XMVectorGetZ(xaxis), XMVectorGetZ(DefaultUp), XMVectorGetZ(directionInvertX), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);


	UpdateWorld();
}

float ModelManager::GetLife()
{
	return _life;
}

void ModelManager::SetAlive(bool alive)
{
	_isAlive = alive;
}

XMMATRIX* ModelManager::GetMatrix()
{
	return &_world;
}

Collision ModelManager::GetBound()
{
	return _aabb;
}

void ModelManager::Unload()
{
	_texture->UnloadTexture();
	_mesh->Unload(); 

	delete _mesh;
	delete _texture;
}

ModelManager::~ModelManager()
{

}
