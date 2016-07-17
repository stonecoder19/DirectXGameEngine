#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	//m_ColorShader = 0;
	m_Model = 0;
	m_TextureShader = 0;

	m_lightShader = 0;
	m_Light = 0;

}

GraphicsClass::GraphicsClass(const GraphicsClass& graphicsClass)
{

}

GraphicsClass::~GraphicsClass()
{

}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	//Create the Direct3D object
	m_D3D = new D3DClass;

	if (!m_D3D)
	{
		return false;
	}

	//Initialize the Direct3D object
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initalize Direct3D", L"Error", MB_OK);
		return false;
	}

	//Create the camera object
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	//set the intial position of the camera
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	//Create the model object
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	//Initialize the model object
	result = m_Model->Initialize(m_D3D->GetDevice(),m_D3D->GetDeviceContext(),"cube.txt","stone01.tga");
	if (!result)
	{
		MessageBox(hwnd, L"Could not intialize the model object", L"Error", MB_OK);
		return false;
	}

	//create the color shader object
	/*m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
	{
		return false;
	}

	//initalize the color shader object
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initalize the color shader object", L"Error", MB_OK);
		return false;
	}*/




	//create the texture shader object
	/*m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	//Initialize the texture shader object
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initalize the texture shader object", L"Error", MB_OK);
		return false;
	}*/

	m_lightShader = new LightShaderClass;
	if (!m_lightShader)
	{
		return false;
	}

	result = m_lightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initalize the light shader object", L"Error", MB_OK);
		return false;
	}

	//Create the light object
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	//Initalize the light object
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);


	return true;

}

void GraphicsClass::Shutdown()
{


	//Release the light object
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	//Release the light shader object
	if (m_lightShader)
	{
		m_lightShader->Shutdown();
		delete m_lightShader;
		m_lightShader = 0;
	}


	//release the texture object
	/*if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}*/


	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	//release the color shader object
	/*if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}*/

	//release the model object
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	//release the camera object
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}


	return;

}

bool GraphicsClass::Frame()
{
	bool result;


	static float rotation = 0.0f;

	rotation += (float)XM_PI*0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	//rende the graphics scene
	result = Render(rotation);
	if (!result)
	{
		return false;
	}
	return true;
}

bool GraphicsClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;


	//Clear the buffers to begin the scene
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//generate the view matrix based on camera's position
	m_Camera->Render();

	//get the world,view and projection matrices from camera and d3d objects
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	//rotate the world matrix by the rotation value
	worldMatrix = XMMatrixRotationY(rotation);

	//put the model vertex and index buffers on the graphics pipeline to prepare them for drawing
	m_Model->Render(m_D3D->GetDeviceContext());


	result = m_lightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(), m_Light->GetDirection(),m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
						m_Camera->GetPosition(),m_Light->GetSpecularColor(),m_Light->GetSpecularPower());

	if (!result)
	{
		return false;
	}

	//render the model using the color shader
	/*result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}*/

	/*result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result)
	{
		return false;
	}*/

	
	//present the rendered scene to screen
	m_D3D->EndScene();

	return true;
}

