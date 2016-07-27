#include "bitmapclass.h"

BitmapClass::BitmapClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;


}


BitmapClass::BitmapClass(const BitmapClass& other)
{

}

BitmapClass::~BitmapClass()
{

}

bool BitmapClass::Inititalize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, char* textureFilename, 
			int bitmapWidth, int bitmapHeight)
{
	bool result;
	
	//store screen size
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	//store size in pixels
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	//initalize previous position
	m_previousPosX = -1;
	m_previousPosY = -1;

	//intialize index and vertex buffers
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	//load texture for this model
	result = LoadTexture(device, deviceContext,textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::Shutdown()
{
	//release model texture
	ReleaseTexture();

	//shutdown vertex and index buffers
	ShutdownBuffers();

	return;
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positonY)
{
	bool result;

	//rebuild dynamic vertex buffer for rendering
	result = UpdateBuffers(deviceContext, positionX, positonY);
	if (!result)
	{
		return false;
	}

	RenderBuffers(deviceContext);

	return true;
}

int BitmapClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	//set number of vertices
	m_vertexCount = 6;

	//set number of indicies
	m_indexCount = m_vertexCount;

	//create vertex array
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//create index array
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	//initlialize vertex array to zeros first
	memset(vertices, 0, sizeof(VertexType)* m_vertexCount);

	//load the index array with data
	for (i = 0;i < m_indexCount;i++)
	{
		indices[i] = i;
	}

	//set up descritpion of static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//give subresource a 
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData,&m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//set up description of index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//create index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//release the arrays now that the vertex and index buffers have been created
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;

}

void BitmapClass::ShutdownBuffers()
{
	//release index buffer
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	//release the vertex buffer
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	return;
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{

	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;


	//if the position hasnt changed dont update buffers
	if((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	//if has changed then update previous position
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	left = (float)((m_screenWidth / 2)*-1) + (float)positionX;

	right = left + (float)m_bitmapWidth;

	top = (float)(m_screenHeight / 2)-(float)positionY;

	bottom = top - (float)m_bitmapHeight;

	//create vertex array
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//load vertex array with data

	vertices[0].position = XMFLOAT3(left, top, 0);
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	//second triangle
	vertices[3].position = XMFLOAT3(left, top, 0.0f); //top left
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	//lock vertex buffer so it can be written to
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//get a pointer to the data in the vertex buffer
	verticesPtr = (VertexType*)mappedResource.pData;

	//cop data into vertex buffer
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType)* m_vertexCount));


	//unlock the vertex buffer
	deviceContext->Unmap(m_vertexBuffer, 0);

	//release the vertex array as it no longer needed
	delete[] vertices;
	vertices = 0;

	return true;

}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//set vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	//set vertex buffer to active in the input
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//set index buffer to active in the input assembeler
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool BitmapClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext,char* filename)
{
	bool result;

	//Create texture object
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	//Initialize the texture object
	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::ReleaseTexture()
{
	//release texture object
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;

	}

	return;
}
