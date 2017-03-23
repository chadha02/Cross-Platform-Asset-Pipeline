#pragma once
namespace eae6320
{
	namespace Graphics
	{
		struct sMaterial
		{
			struct
			{
				float r, g, b, a;
			} g_color;
			sMaterial()
			{
				g_color.r = 1.0f;
				g_color.g = 1.0f;
				g_color.b = 1.0f;
				g_color.a = 1.0f;
			}
		};// s_constantMaterialBufferData;
	}
}