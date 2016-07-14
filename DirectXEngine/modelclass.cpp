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

bool ModelClass::Initialize(ID3D11Device* device,ID3D11DeviceContext* deviceContext,char* modelFilename,char* textureFileName)
{
	bool result;

	//Load in the model data
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}



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

	ReleaseModel();

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
	int i;


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

	//Load the vertex array and index array with data
	for (i = 0;i < m_vertexCount;i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

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


bool ModelClass::LoadModel(char* filename)
{
	ifstream fin;
	char input;
	int i;


	//open the model file
	fin.open(filename);

	//if it could not open the file then exit
	if (fin.fail())
	{
		return false;
	}

	//Read up to the value of the vertex count
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//Read in the vertex count
	fin >> m_vertexCount;

	//set number of indices to the same as vertex count
	m_indexCount = m_vertexCount;

	//create the model using the vertex count
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	//Read up to beginning of the data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	//read in vertex data
	for (int i = 0;i < m_vertexCount;i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;

	}

	//close the model file
	fin.close();

	return true;
}

void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}



