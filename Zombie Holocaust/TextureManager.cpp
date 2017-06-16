#include "TextureManager.h"
#include <d3dx11.h>
#include <xnamath.h>
#include <string>

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

ID3D11ShaderResourceView *const* TextureManager::GetTexture()
{
	return &TextureMap;
}

void TextureManager::TextureLoader(const char* TextureName, ID3D11Device * DeviceName)
{
	D3DX11CreateShaderResourceViewFromFile( DeviceName, TextureName, 0, 0, &TextureMap, 0 );
}

void TextureManager::UnloadTexture()
{
	if (TextureMap) TextureMap->Release();			//Release Texture Pointer
	TextureMap = 0;
}

