#include "colorshaderclass.h"


ColorShaderClass::ColorShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_matrixBuffer = 0;
	m_inputLayout = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{

}

ColorShaderClass::~ColorShaderClass()
{

}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	//initialize the vertex and pixel shaders
	result = InitializeShader(device, hwnd, L"color.vs", L"color.ps");
	if (!result)
	{
		return false;
	}

	return true;
}

void ColorShaderClass::Shutdown()
{
	//shutdown the vertex and pixel shader as well as the related objects
	ShutdownShader();

	return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatix)
{
	bool result;

	//Set the shader paramaters that it will use for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatix);
	if (!result)
	{
		return false;
	}

	//render prepared buffers with the shader
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	//initalize pointers this function will use to null
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if FAILED(result)
	{
		// if the shader failed to compile  it should have written something to the error message
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else {
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//compile pixel shader code
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer,&errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else {
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}


	//create vertex shader from buffer
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}


	//create pixel shader from buffer
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	//create the vertex input layout description

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;


	//get a count of the elelments
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//create vertex input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
												vertexShaderBuffer->GetBufferSize(), &m_inputLayout);
	if (FAILED(result))
	{
		return false;
	}

	//release vertex shader buffer and pixel shader buffer
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	//set up the description of the dynamic matrix constant buffer
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//create the constant buffer pointer so we can access the vertex shader constant buffer
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;


}

void ColorShaderClass::ShutdownShader()
{
	//release the matrix buffer
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	//release the layout
	if (m_inputLayout)
	{
		m_inputLayout->Release();
		m_inputLayout = 0;
	}

	//release the pixel shader
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	//release the vertex shader
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	//get a 
	compileErrors = (char*)errorMessage->GetBufferPointer();

	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for (i = 0;i < bufferSize;i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L"Error Compiling shader. Check shader-error.txt for more information", shaderFileName, MB_OK);

	return;
}



bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//transpose matrices to prepare them for buffers
	viewMatrix = XMMatrixTranspose(viewMatrix);
	worldMatrix = XMMatrixTranspose(worldMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//lock constant buffer so it can be written to
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//get a pointer to the data in the constant buffer
	dataPtr = (MatrixBufferType*)mappedResource.pData;


	//copy the matrices into the contant buffer
	dataPtr->projection = projectionMatrix;
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;

	//unlock the constant buffer
	deviceContext->Unmap(m_matrixBuffer, 0);

	//set the position of the constant buffer in the vertex shader
	bufferNumber = 0;

	//set the constant buffer in the vertex shader with the update values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//set the vertex input layout
	deviceContext->IASetInputLayout(m_inputLayout);

	//set vertex and pixel shaders
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//render the triangle
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}



