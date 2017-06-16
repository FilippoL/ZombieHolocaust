#pragma once
#include<d3dx11.h>
#include<xnamath.h>
#include<stdio.h>
#include<vector>

class Mesh
{
public:
	Mesh();
	 // VertexPos* vertices

	std::vector<float*> _vertsPtr;
	std::vector<float*> _texCPtr;
	std::vector<float*> _normalPtr;
	

	void LoadMesh(std::string FileName,  ID3D11Device* DeviceName, int NumbOfFrames = 1);
	void Unload();
	~Mesh();
	int GetVerts(int whatframe = 0);

	std::string whole_name;

	ID3D11Buffer* const* GetGeometry(int whatframe = 0);

	std::vector<int> _Totvertices;

	std::vector<ID3D11Buffer*> _buff_geometry;

};


struct VertexPos
{
    XMFLOAT3 _pos;
    XMFLOAT2 _tex0;
    XMFLOAT3 _norm;
};

