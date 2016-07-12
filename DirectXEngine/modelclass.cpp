#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	m_Texture = 0;

}

ModelClass::~ModelClass()
{

}

ModelClass::ModelClass(const ModelClass&)
{

}

bool ModelClass::Initialize(ID3D11Device* device,ID3D11DeviceContext* deviceContext,char* textureFileName)
{
	bool result;

	//initialize the vertex and index buffers
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	//load the texture for this model
	result = LoadTexture(device, deviceContext, textureFileName);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	//release the model texture
	ReleaseTexture();


	ShutdownBuffers();

	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	//put the vertex and index buffers on the graphics pipeline
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//set the number of vertices in the vertex array
	m_vertexCount = 3;

	//set the number of indices in the index array
	m_indexCount = 3;

	//Create the vertex array
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	//load the vertex array with data
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  //Bottom Left
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	//vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f); //Top middle
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
	//vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
	//vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	//Load the index array with data
	indices[0] = 0; //bottom left
	indices[1] = 1; //top middle
	indices[2] = 2; //bottom right

	//set up description of static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Give the subresource structure a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Now create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}


	//set up descritption of static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;	
	indexBufferDesc.StructureByteStride = 0;


	//Give the subresource structure a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//release the index and vertex arrays since we are done with the buffer
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;



}

void ModelClass::ShutdownBuffers()
{
	//Release index buffer
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	//Release vertex buffer
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	//Set the vertex buffer to active in the input assembler so it can be rendered
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assemberler so it can be rendered
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	//create the texture object
	m_Texture = new TextureClass;

	if (!m_Texture)
	{
		return false;
	}

	//initialize the texture object
	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	//release the texture object
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
	return;
}

