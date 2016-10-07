#include "fontclass.h"

FontClass::FontClass()
{
	m_Font = 0;
	m_Texture = 0;
}

FontClass::FontClass(const FontClass& other)
{

}

FontClass::~FontClass()
{

}

bool FontClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deivceContext,char* fontFilename, wchar_t* textureFilename)
{
	bool result;

	result = LoadFontData(fontFilename);
	if (!result)
	{
		return false;
	}

	result = LoadTexture(device,textureFilename);
	if (!result)
	{
		return false;
	}
	
	return true;
}


void FontClass::Shutdown()
{
	ReleaseTexture();

	ReleaseFontData();

	return;
}


bool FontClass::LoadFontData(char* filename)
{
	ifstream fin;
	int i;
	char temp;

	m_Font = new FontType[95];
	if (!m_Font)
	{
		return false;
	}

	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	for (i = 0; i < 95; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	fin.close();

	return true;
}

void FontClass::ReleaseFontData()
{
	if (m_Font)
	{
		delete[] m_Font;
		m_Font = 0;
	}
}

bool FontClass::LoadTexture(ID3D11Device* device, wchar_t* filename)
{
	bool result;

	m_Texture = new TextureClass();
	if (!m_Texture)
	{
		return false;
	}

	result = m_Texture->Initialize(device,filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void FontClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;	
}

void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{

	VertexType* vertexPtr;
	int numLetters, index, i, letter;

	vertexPtr = (VertexType*)vertices;

	numLetters = (int)strlen(sentence);

	index = 0;

	for (i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			//top left
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			//botton right
			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			//bottom left
			vertexPtr[index].position = XMFLOAT3(drawX, (drawY - 16), 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
			index++;

			//top left
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
			index++;


			//top right
			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), drawY, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			//bottom right
			vertexPtr[index].position = XMFLOAT3((drawX+m_Font[letter].size), (drawY - 16), 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;


			drawX = drawX + m_Font[letter].size + 1.0f;
		
		}
	}

	return;
}

