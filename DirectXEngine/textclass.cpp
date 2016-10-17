#include "textclass.h"





TextClass::TextClass()
{
	m_Font = 0;
	m_FontShader = 0;

	m_sentence1 = 0;
	m_sentence2 = 0;
}

TextClass::TextClass(const TextClass& other)
{


}

TextClass::~TextClass()
{

}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth,
							int  screenHeight, XMMATRIX baseViewMatrix)
{

	bool result;

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_baseViewMatrix = baseViewMatrix;


	//create font object
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	result = m_Font->Initialize(device, "data.txt", L"font.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object", L"Error", MB_OK);
		return false;
	}

	//create font shader object
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	//initialize font shader object
	result = m_FontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object", L"Error", MB_OK);
		return false;
	}

	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result)
	{
		return false;
	}

	result = UpdateSentence(m_sentence1, "Hello", 100, 100, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	result = InitializeSentence(&m_sentence2, 16, device);
	if (!result)
	{
		return false;
	}

	result = UpdateSentence(m_sentence2, "Line 2 Yeah", 100, 200, 1.0f, 0.0f, 1.0f,deviceContext);
	if (!result)
	{
		return false;
	}

	
	return true;

}


void TextClass::Shutdown()
{

	ReleaseSentence(&m_sentence1);

	ReleaseSentence(&m_sentence2);

	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	return;
}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{

	bool result;

	//Draw the first sentence
	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);

	if (!result)
	{
		return false;
	}

	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}


bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{

	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	(*sentence)->maxLength = maxLength;

	(*sentence)->vertexCount = 6 * maxLength;

	(*sentence)->indexCount = (*sentence)->vertexCount;

	vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	//initialize vertex array to zeros
	memset(vertices, 0, (sizeof(VertexType)*(*sentence)->vertexCount));

	//initialize index aray
	for (i = 0; i < (*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}


	//set up buffer description
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.StructureByteStride = 0;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//create vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//index buffer description
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
	
	return true;

}

bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red,
						float green, float blue, ID3D11DeviceContext* deviceContext)
{

	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;


	//store the color
	sentence->blue = blue;
	sentence->green = green;
	sentence->red = red;

	//get number of letters
	numLetters = strlen(text);

	//buffer overflow
	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	//initialize arrays to zero
	memset(vertices, 0, sizeof(VertexType)*sentence->vertexCount);

	//claculate x and y pixel position
	drawX = ((float)m_screenWidth / 2) + positionX;
	drawY = ((float)m_screenHeight / 2 * -1) - positionY;


	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	//lock vertex buffer so it can be written to
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	verticesPtr = (VertexType*)mappedResource.pData;

	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType)* sentence->vertexCount));

	//unlock vertex buffer
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	delete[] vertices;
	vertices = 0;

	return true;


}

void TextClass::ReleaseSentence(SentenceType** sentence)
{

	if ((*sentence))
	{

		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		delete *sentence;
		*sentence = 0;
	}

	return;
}


bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix,
	XMMATRIX orthoMatrix)
{

	unsigned int stride, offset;
	XMFLOAT4 pixelColor;
	bool result;

	stride = sizeof(VertexType);
	offset = 0;

	//set vertex buffer
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	//set index buffer
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//aet primitve topology
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pixelColor = XMFLOAT4(sentence->red, sentence->blue, sentence->green, 1.0f);

	result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix,
		m_Font->GetTexture(), pixelColor);

	if (!result)
	{
		false;
	}

	return true;

}







