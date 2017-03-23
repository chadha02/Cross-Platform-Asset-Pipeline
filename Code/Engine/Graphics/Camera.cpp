#include "Camera.h"
#include "../Math/Functions.h"
#include "../Time/Time.h"

eae6320::Camera::Camera()
{

}
eae6320::Camera::Camera(Camera  &rhs):
	FOV(rhs.FOV) , nearPlaneDistance(rhs.nearPlaneDistance),farPlaneDistance(rhs.nearPlaneDistance),
	orientation(rhs.orientation), position(rhs.position)
{
	
}

eae6320::Camera::Camera(float i_fov, float near, float far, float aspect, Math::cQuaternion i_quat, Math::cVector i_pos)
{
	FOV = Math::ConvertDegreesToRadians(i_fov);
	nearPlaneDistance = near;
	farPlaneDistance = far;
	aspectRatio = aspect;
	orientation = i_quat;
	position = i_pos;
	worldToCamera = Math::cMatrix_transformation::CreateWorldToCameraTransform(orientation, position);
	cameraToScreen = Math::cMatrix_transformation::CreateCameraToProjectedTransform(FOV, aspectRatio, nearPlaneDistance, farPlaneDistance);
}

void eae6320::Camera::Move(Math::cVector i_pos)
{
	position += i_pos *Time::GetElapsedSecondCount_duringPreviousFrame();;
	updateCamera();
}

void eae6320::Camera::Rotate(Math::cQuaternion i_quat)
{
	orientation = orientation * i_quat;
}
void eae6320::Camera::SetAspectRatio(float i_ratio)
{
	aspectRatio = i_ratio;
}
//eae6320::Math::cMatrix_transformation eae6320::Camera::WorldToCamera()
//{
//	 eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform(orientation, position);
//	
//}
//eae6320::Math::cMatrix_transformation eae6320::Camera::CameraToScreen()
//{
//	 eae6320::Math::cMatrix_transformation::CreateCameraToProjectedTransform(FOV, aspectRatio, nearPlaneDistance, farPlaneDistance);
//}

eae6320::Math::cMatrix_transformation eae6320::Camera::GetCameraToScreen()
{
	return cameraToScreen;
}

eae6320::Math::cMatrix_transformation eae6320::Camera::GetWorldToCamera()
{
	return worldToCamera;
}

void eae6320::Camera::updateCamera()
{
	cameraToScreen = Math::cMatrix_transformation::CreateCameraToProjectedTransform(FOV, aspectRatio, nearPlaneDistance, farPlaneDistance);
	worldToCamera = Math::cMatrix_transformation::CreateWorldToCameraTransform(orientation, position);
}