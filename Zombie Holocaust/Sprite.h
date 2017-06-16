#pragma once
#include "TextureManager.h"
#include "Collision.h"
#include <vector>

class Sprite
{
public:
	Sprite();

	void Intialise(ID3D11Device * DeviceName, std::string txture, 
					float x, float y, float z, 
					int W, int H);

	void SetPosition(float x, float y, float z);


	void Update();

	void Unload();

	int GetVerts();

	ID3D11Buffer * const * GetGeometry();
	ID3D11SamplerState* textColorMapSampler_;

	TextureManager * GetTexture();

	XMFLOAT3 position;


	TextureManager * _texture;

	float* _vertsPtr;
	float* _texCPtr;
	float* _normalPtr;

	std::vector<float>  _vertices;
	std::vector<float>  _UVs;

	XMMATRIX* GetMatrix() { return &_matrix; }

	~Sprite();

protected:
	ID3D11Buffer* _buff_geometry;

	XMMATRIX _matrix;

};

struct VPOS
{
    float _pos;
    float _tex0;
};