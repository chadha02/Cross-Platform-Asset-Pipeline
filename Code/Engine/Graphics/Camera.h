#pragma once
#pragma once
#ifndef EAE6320_CAMERA_H
#define EAE6320_CAMERA_H
#include "../Math/cVector.h"
#include "../Math/cQuaternion.h"
#include "../Math/cMatrix_transformation.h"

namespace eae6320
{
	class Camera
	{
	private:
		Math::cVector position;
		Math::cQuaternion orientation;
		float FOV;
		float aspectRatio;
		float nearPlaneDistance;
		float farPlaneDistance;

		Math::cMatrix_transformation cameraToScreen;
		Math::cMatrix_transformation worldToCamera;
	public:
		Camera();
		Camera(Camera & rhs);
		Camera(float i_fov, float near, float far, float aspect, Math::cQuaternion i_quat, Math::cVector i_pos);
		void SetAspectRatio(float i_ratio);
		void Move(Math::cVector i_pos);
		void Rotate(Math::cQuaternion i_orient);
		Math::cMatrix_transformation GetWorldToCamera();  
		Math::cMatrix_transformation GetCameraToScreen(); 
		void updateCamera();
	

	};
}

#endif