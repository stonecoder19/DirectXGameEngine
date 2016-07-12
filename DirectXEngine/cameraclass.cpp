#include "cameraclass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass&)
{

}

CameraClass::~CameraClass()
{

}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);	
}

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	//set up the vector that points upwards
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//Load it into a XMVECTOR structure
	upVector = XMLoadFloat3(&up);

	//set up the position of the camera in the world
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	//Load it into a XMVECTOR structure
	positionVector = XMLoadFloat3(&position);

	//setup where the camera is looking	by default
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	//load it into  a XMVECTOR structre
	lookAtVector = XMLoadFloat3(&lookAt);

	//set the yaw pitch and roll in radians
	pitch = m_rotationX *  0.0174532925f;
	yaw = m_rotationY *  0.0174532925f;
	roll = m_rotationZ *  0.0174532925f;

	//create the rotation matrix from the yaw,pitch and roll
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);

	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//finally create the view matrix
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;

}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}