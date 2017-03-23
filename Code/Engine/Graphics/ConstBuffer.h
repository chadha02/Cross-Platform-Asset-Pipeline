#pragma once
#ifndef EAE6320_CONSTBUFFER_H
#define EAE6320_CONSTBUFFER_H

#include "cBuffer.h"
#include "sMaterial.h"
#include "../Math/cMatrix_transformation.h"
namespace eae6320
{
	class ConstBuffer
	{
	private:

		int buffernumber;
		size_t buffSize;
	public:
	
		struct sFrame
		{
			Math::cMatrix_transformation g_transform_worldToCamera;
			Math::cMatrix_transformation g_transform_cameraToScreen;
			union
			{
				float g_elapsedSecondCount_total;
				float register8[4];
			};
		}s_constantFrameBufferData;

		struct sDrawCall
		{
			Math::cMatrix_transformation g_transform_localToWorld;
		}s_constantDrawBufferData;

		/*struct sMaterial
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
		}s_constantMaterialBufferData;*/

		Graphics::sMaterial s_constantMaterialBufferData;
		enum ConstBufferType
		{
			FRAME_CALL_BUFFER,
			DRAW_CALL_BUFFER,
			MATERIAL_CALL_BUFFER
		};

#if defined( EAE6320_PLATFORM_D3D )
		ID3D11Buffer* s_constantBuffer;
#elif defined( EAE6320_PLATFORM_GL )
		GLuint s_constantBufferId;
#endif
		//ConstBuffer() : s_constantMaterialBufferData() {} ;
		bool Init(ConstBufferType bufferType, size_t t, void *);
		void Bind();
		void Update(void * i_ptr);

		void Cleanup();
	};
}



#endif