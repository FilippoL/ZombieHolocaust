#pragma once
#include <d3dx11.h>
#include <xnamath.h>
#include <string>

class TextureManager
{

public:

	TextureManager();

	void TextureLoader(const char* TextureName, ID3D11Device * DeviceName);

	void UnloadTexture();
	
	~TextureManager();

	//ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView *const* GetTexture();


private: 
	ID3D11ShaderResourceView* TextureMap;
};