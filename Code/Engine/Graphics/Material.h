#pragma once
#ifndef EAE6320_MATERIAL_H
#define EAE6320_MATERIAL_H

#include "Effect.h"
#include "ConstBuffer.h"
#include "cTexture.h"

namespace eae6320
{
	namespace Graphics
	{
		class Material
		{
		public:
			Effect m_effect;
			ConstBuffer m_constBuffer;
			cTexture m_texture;
		public:
			void Load(const char* i_matPath);
			void CleanUp();
			void Bind();

		};
	}
}

#endif