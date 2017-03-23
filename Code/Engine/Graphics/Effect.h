#pragma once
#ifndef EAE6320_EFFECT_H
#define EAE6320_EFFECT_H

#include "../Platform/Platform.h"
#include "Context.h"
#include "sVertex.h"
#include "cRenderState.h"

namespace eae6320
{
	namespace Graphics
	{
		class Effect
		{
		private:
			cRenderState s_renderState;
#if defined( EAE6320_PLATFORM_D3D ) 
			ID3D11VertexShader* s_vertexShader;
			ID3D11PixelShader* s_fragmentShader;
			ID3D11InputLayout* s_vertexLayout;
#elif defined( EAE6320_PLATFORM_GL )
			GLuint s_programId;
#endif
		public:
			Effect();
			bool LoadVShader(const char* vertexShaderpath);
			bool LoadFShader (const char* fragShaderPath);
			bool LoadShader(const char* effectPath);
			void CleanUp();
			void Bind();
		};
	}
}
#endif