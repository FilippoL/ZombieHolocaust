#include "Sprite.h"



Sprite::Sprite()
{
	_matrix = XMMatrixIdentity();
	_texture = new TextureManager;
}

void Sprite::Intialise(ID3D11Device* DeviceName, std::string txture,float x, float y, float z, int W, int H)
{
	//
	////all vertices for 2D quad (we will keep the depth at 0)
	//_vertices.push_back(x - W/2); _vertices.push_back(y - H/2); _vertices.push_back(z); 
	//_vertices.push_back(x - W/2); _vertices.push_back(y + H/2); _vertices.push_back(z); 
	//_vertices.push_back(x + W/2); _vertices.push_back(y + H/2); _vertices.push_back(z); 
	//_vertices.push_back(x + W/2); _vertices.push_back(y - H/2); _vertices.push_back(z); 
 //
	////all texture coordinates for 2D quad
	//_UVs.push_back(0.0f); _UVs.push_back(0.0f); _UVs.push_back(1.0f); _UVs.push_back(0.0f);
	//_UVs.push_back(1.0f); _UVs.push_back(1.0f); _UVs.push_back(0.0f); _UVs.push_back(1.0f);


	//VPOS* vertices = new VPOS[_vertices.size()];

	//_vertsPtr = &_vertices[0];
	//
	//_texCPtr = &_UVs[0];

	//for (int t = 0; t < _vertices.size(); t++)
	//	{
	//		vertices[t]._pos = _vertices[t];
	//		_vertsPtr += 3;
	//	}

	//for (int t = 0; t < _UVs.size(); t++)
	//	{
	//		vertices[t]._tex0 = _UVs[t];
	//		_texCPtr += 2;
	//	}
	//
	_texture->TextureLoader(txture.c_str(), DeviceName);

	D3D11_SAMPLER_DESC textColorMapDesc;
    ZeroMemory( &textColorMapDesc, sizeof( textColorMapDesc ) );
    textColorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    textColorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    textColorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    textColorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    textColorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    textColorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;

    DeviceName->CreateSamplerState( &textColorMapDesc, &textColorMapSampler_ );


	D3D11_BUFFER_DESC sprtVertexDesc;
    ZeroMemory( &sprtVertexDesc, sizeof( sprtVertexDesc ) );
    sprtVertexDesc.Usage = D3D11_USAGE_DYNAMIC;
    sprtVertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    sprtVertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    const int sizeOfSprite = sizeof( VPOS ) * 6;
    const int maxLetters = 24;

    sprtVertexDesc.ByteWidth = sizeOfSprite * maxLetters;

    DeviceName->CreateBuffer( &sprtVertexDesc, 0, &_buff_geometry );


	//D3D11_SUBRESOURCE_DATA resourceData;
	//ZeroMemory(&resourceData, sizeof(resourceData));
	//resourceData.pSysMem = vertices;

	//DeviceName->CreateBuffer(&vertexDesc, &resourceData, &_buff_geometry);


	//delete[] vertices;
}

void Sprite::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	Update();
}


void Sprite::Update()
{

	_matrix =  XMMatrixTranslation(position.x,position.y,position.z);
	_matrix = XMMatrixTranspose(_matrix);
}

void Sprite::Unload()
{
	if (_buff_geometry){	_buff_geometry->Release();}

	_buff_geometry = 0;
	
	_vertices.clear();
	_UVs.clear();
}

int Sprite::GetVerts()
{
	return _vertices.size();
}

ID3D11Buffer * const * Sprite::GetGeometry()
{
	return &_buff_geometry;
}

TextureManager* Sprite::GetTexture()
{
	return _texture;
}

Sprite::~Sprite()
{
}
