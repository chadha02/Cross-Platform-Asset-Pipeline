#pragma once
#ifndef CONTEXT_H
#define CONTEXT_H

#include "../Logging/Logging.h"
#include "../Asserts/Asserts.h"

#if defined( EAE6320_PLATFORM_D3D )

#include <D3D11.h>


#elif defined( EAE6320_PLATFORM_GL )

#include "../Graphics/OpenGL/Includes.h"
#include "../Windows/OpenGl.h"
#include "../../External/OpenGlExtensions/OpenGlExtensions.h"

#endif
namespace eae6320
{
	namespace Graphics
	{
#if defined( EAE6320_PLATFORM_D3D )
		extern ID3D11DeviceContext* s_direct3dImmediateContext;
		extern ID3D11Device* s_direct3dDevice;
		extern ID3D11SamplerState* s_samplerState;

#endif
	}

}
#endif