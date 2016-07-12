#include "textureshaderclass.h"



TextureShaderClass::TextureShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;

	m_sampleState = 0;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{

}

TextureShaderClass::~TextureShaderClass()
{

}

bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	//initialize vertex and pixel shader
	result = InitializeShader(device,hwnd, L"texture.vs", L"texture.ps");
	if (!result)
	{
		return false;
	}

	return true;
}

void TextureShaderClass::Shutdown()
{
	//shutdown the vertex and pixel shader
	ShutdownShader();

	return;
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;

	//set the shader parameters
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;


	D3D11_SAMPLER_DESC samplerDesc;

	//initialize the pointers to null
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//Compile the vertex shader code
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else {
			MessageBox(hwnd, vsFilename, L"Missing Shader file", MB_OK);
		}

		return false;
	}

	//compile the pixel shader code
	result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//if shader failed to compile  it should have written to the error message
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		//if there was nothing in error message it couldnt find shader
		else {
			MessageBox(hwnd, psFilename, L"Missing shder file", MB_OK);
		}

		return false;
	}

	//create the vertex shader from the buffer
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//Create the pixel shader
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	//create the vertex input layout
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;


	//Get a count of elements in layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);


	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
												vertexShaderBuffer->GetBufferSize(), &m_layout);

	if (FAILED(result))
	{
		return false;
	}

	//release the vertex shader buffer and pixel shader buffer
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;


	//set up descritpion of matrix constant buffer
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;


	//create the constat buffer pointer so we can access the vertex shader constant buffer
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Create a texture sample state description
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the texture sampler state
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	return true;


}

void TextureShaderClass::ShutdownShader()
{
	//Release the sampler state
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	//Release the matrix constant buffer
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	//Release the layout
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	//Release the pixel shader
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	//Release the vertex shader
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;

}


void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize,i;
	ofstream fout;

	//Get a pointer to the error message text buffer
	compileErrors = (char*)errorMessage->GetBufferPointer();

	//Get the length of the buffer
	bufferSize = errorMessage->GetBufferSize();

	//open a file to the shader error file
	fout.open("shader-error.txt");

	//Write out the shader error message
	for (int i = 0;i < bufferSize;i++)
	{
		fout << compileErrors[i];
	}

	//close the file
	fout.close();

	//Release the error message
	errorMessage->Release();
	errorMessage = 0;

	//pop a message on screen to notify the user to check the text file for compile errors
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message", shaderFilename, MB_OK);

	return;

}


bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix,
												XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;


	//Transpose the matrices to prepare them for the shader
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//lock the constant buffer so it can be written to
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	if (FAILED(result))
	{
		return false;
	}

	//Get a	pointer to the data in the constant buffer
	dataPtr = (MatrixBufferType*)mappedSubResource.pData;

	//copy the matrices into the constant buffer
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//Unlock the constant buffer
	deviceContext->Unmap(m_matrixBuffer, 0);

	//Set the position of the constant buffer in the vertex shader
	bufferNumber = 0;

	//finally set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	//set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//set the vertex input layout
	deviceContext->IASetInputLayout(m_layout);

	//set the vertex and pixel shader
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//set the sampler state in the pixel shader
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//render the triangle
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}






