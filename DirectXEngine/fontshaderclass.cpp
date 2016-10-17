#include "fontshaderclass.h"


FontShaderClass::FontShaderClass()
{

}

FontShaderClass::FontShaderClass(const FontShaderClass& other)
{

}

FontShaderClass::~FontShaderClass()
{

}

bool FontShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, L"font.vs", L"font.ps");

	if (!result)
	{
		return false;
	}

	return true;
}

void FontShaderClass::Shutdown()
{
	ShutdownShader();

	return;
}

bool FontShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor)

{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, pixelColor);
	if (!result)
	{
		return false;
	}

	//render the shader with prepared buffers
	RenderShader(deviceContext, indexCount);

	return true;
}

bool FontShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC constantBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC pixelBufferDesc;


	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//compile vertex shader code
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "FontVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 
						&vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage,hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}
	
	//compile pixel shader code
	result = D3DCompileFromFile(psFilename, NULL, NULL, "FontPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing shader file", MB_OK);
		}
		return false;
	}

	//create vertex shader
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), 
		NULL, &m_vertexShader);

	if (FAILED(result))
	{
		return false;
	}

	//create pixel shader
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), 
		NULL, &m_pixelShader);

	if (FAILED(result))
	{
		return false;
	}

	//create vertex input layout

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

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);

	if (FAILED(result))
	{
		return false;
	}

	//release vertex shader buffer and pixel shader buffer

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(ConstantBufferType);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&constantBufferDesc, NULL, &m_constantBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//create sample state
	samplerDesc.Filter = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
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


	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&pixelBufferDesc, NULL, &m_pixelBuffer);
	if (FAILED(result))
	{
		return false;
	}


	return true;
}


void FontShaderClass::ShutdownShader()
{

	if (m_pixelBuffer)
	{
		m_pixelBuffer->Release();
		m_pixelBuffer = 0;
	}

	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	if (m_constantBuffer)
	{
		m_constantBuffer->Release();
		m_constantBuffer = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void FontShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* filename)
{

	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	compileErrors = (char*)errorMessage->GetBufferPointer();

	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", filename, MB_OK);

	return;
}

bool FontShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor)
{


	HRESULT result;
	ConstantBufferType* dataPtr;
	PixelBufferType* dataPtr2;
	unsigned int bufferNumber;
	D3D11_MAPPED_SUBRESOURCE mappedResource;


	result = deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (ConstantBufferType*)mappedResource.pData;

	XMMatrixTranspose(worldMatrix);
	XMMatrixTranspose(viewMatrix);
	XMMatrixTranspose(projectionMatrix);

	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	bufferNumber = 0;

	deviceContext->Unmap(m_constantBuffer, 0);

	deviceContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);

	deviceContext->PSSetShaderResources(0, 1, &texture);


	result = deviceContext->Map(m_pixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr2 = (PixelBufferType*)mappedResource.pData;

	dataPtr2->pixelColor = pixelColor;

	deviceContext->Unmap(m_pixelBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pixelBuffer);


	return true;
}


void FontShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//set input layout

	deviceContext->IASetInputLayout(m_layout);


	//set vertex and pixel shader
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//set sampler for texture
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//draw triangles
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}






