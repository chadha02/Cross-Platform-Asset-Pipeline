#pragma once
#include "cSprite.h"
#include "Material.h"

namespace eae6320
{
	namespace Graphics
	{
		struct GameObject2D
		{
			cSprite* sprite;
			Material* material_pntr;
		};
	}
}