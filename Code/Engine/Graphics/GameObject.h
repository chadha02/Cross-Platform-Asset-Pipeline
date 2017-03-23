#pragma once
#include "Mesh.h"
#include "../Math/cVector.h"
#include "../Math/cQuaternion.h"
#include "../Math/cMatrix_transformation.h"
//#include "Effect.h"
#include "Material.h"

namespace eae6320 
{
	namespace Graphics
	{
		struct GameObject
		{
			Mesh* mesh_pntr;
			float x, y;
			eae6320::Math::cVector pos_vect;
			eae6320::Math::cQuaternion orient_quat;
			Material* material_pntr;
		};
	}
}
