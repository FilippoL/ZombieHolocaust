
#include "Mesh.h"
#include "objLoader.h"
#include <string>


Mesh::Mesh()
{


}


void Mesh::LoadMesh(std::string FilePath,  ID3D11Device* DeviceName, int NumbOfFrames)
{
	for (int i = 0; i <= NumbOfFrames; i++)
	{
		
		(NumbOfFrames > 1) ? whole_name = FilePath + std::to_string(i+1) + ".obj" : whole_name = FilePath +".obj";
	
		
		ObjModel objModel;
		
		objModel.LoadOBJ(whole_name.c_str());
	
		_buff_geometry.push_back(0);

		_Totvertices.push_back(objModel.GetTotalVerts());

		VertexPos* vertices = new VertexPos[_Totvertices[i]];

		_vertsPtr.push_back(objModel.GetVertices());
		_texCPtr.push_back(objModel.GetTexCoords());
		_normalPtr.push_back(objModel.GetNormals());

		for (int t = 0; t < _Totvertices[i]; t++)
		{
			vertices[t]._pos = XMFLOAT3(*(_vertsPtr[i] + 0), *(_vertsPtr[i] + 1), *(_vertsPtr[i] + 2));
			_vertsPtr[i] += 3;

			vertices[t]._tex0 = XMFLOAT2(*(_texCPtr[i] + 0), *(_texCPtr[i] + 1));
			_texCPtr[i] += 2;

			vertices[t]._norm = XMFLOAT3(*(_normalPtr[i] + 0), *(_normalPtr[i] + 1), *(_normalPtr[i] + 2));
			_normalPtr[i] += 3;
		}

		D3D11_BUFFER_DESC vertexDesc;

		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = sizeof(VertexPos) * _Totvertices[i];

		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = vertices;

		DeviceName->CreateBuffer(&vertexDesc, &resourceData, &_buff_geometry[i]);


		delete[] vertices;
		objModel.Release();
	}
}


void Mesh::Unload()
{
	
	for (size_t i = 0; i < _buff_geometry.size(); i++)
	{
		if (_buff_geometry[i]){	_buff_geometry[i]->Release();}

		_buff_geometry[i] = 0;
	}

	_vertsPtr.clear();
	_texCPtr.clear();
	_normalPtr.clear();
	_Totvertices.clear();
	_buff_geometry.clear();
}

Mesh::~Mesh()
{
}

int Mesh::GetVerts(int whatframe)
{
	return _Totvertices[whatframe];
}

ID3D11Buffer * const * Mesh::GetGeometry(int whatframe)
{
	return &_buff_geometry[whatframe];
}
