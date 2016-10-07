#pragma once
#ifndef _FONTCLASS_H_
#define __FONTCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
using namespace std;
using namespace DirectX;

#include "textureclass.h"

class FontClass
{
private:

	struct FontType
	{
		float left, right;
		int size;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	FontClass();
	FontClass(const FontClass&);
	~FontClass();

	bool Initialize(ID3D11Device*,ID3D11DeviceContext*,char*,wchar_t*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	void BuildVertexArray(void*, char*, float, float);

private:
	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*,wchar_t*);
	void ReleaseTexture();

private:
	FontType* m_Font;
	TextureClass *m_Texture;

};

#endif // !_FONTCLASS_H_
