#include "../Misc/pch.h"

#include "Camera.h"

// A #def for converting degrees to radians
#define DEGREES_TO_RADIANS 0.0174532925f

using namespace DirectX;

Camera::Camera()
{
	m_rotation = { 0.0f, 0.0f, 0.0f };
	m_viewMatrix = XMMatrixIdentity();
}

void Camera::ConstructMatrix(const XMFLOAT3 pos)
{

	// Setup the vector that points upwards.
	XMVECTOR up = { 0, 1.0f, 0 };

	// Setup the position of the camera in the world.
	XMVECTOR position = { pos.x, pos.y, pos.z };

	// Setup where the camera is looking by default.
	XMVECTOR lookAt = { 0, 0, 1.0f };

	// Create the rotation matrix from the yaw, pitch, and roll values.
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.	
	lookAt = XMVector3Transform(lookAt, rotationMatrix);
	up = XMVector3Transform(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt += position;

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(position, lookAt, up);

	return;
}

DirectX::XMFLOAT3 Camera::GetRotation() { return m_rotation; }
void Camera::SetRotation(const DirectX::XMFLOAT3 rot) { m_rotation = rot; }

DirectX::XMMATRIX Camera::GetViewMatrix() { return m_viewMatrix; }

DirectX::XMMATRIX Camera::GetWorldMatrix() { return XMMatrixInverse(nullptr, m_viewMatrix); }

void Camera::SetViewMatrix(const DirectX::XMMATRIX viewMatrix) { m_viewMatrix = viewMatrix; }
void Camera::SetWorldMatrix(const DirectX::XMMATRIX worldMatrix) { m_viewMatrix = XMMatrixInverse(nullptr, worldMatrix); }