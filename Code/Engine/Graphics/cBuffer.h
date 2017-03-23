#pragma once
#ifndef EAE6320_CBUFFER_H
#define EAE6320_CBUFFER_H

#include <iostream>

#if defined( EAE6320_PLATFORM_D3D )

#include <D3D11.h>


#elif defined (EAE6320_PLATFORM_GL)
#include "../Graphics/OpenGL/Includes.h"
#include "../Windows/OpenGl.h"
#include "../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif

//namespace
//{
//	struct sFrame
//	{
//		union
//		{
//			float g_elapsedSecondCount_total;
//			float register0[4];
//		};
//	}s_constantFrameBufferData;
//
//	struct sDrawCall
//	{
//		union
//		{
//			float g_objectPosition_screen[2];
//			float register0[4];
//		};
//	}s_constantDrawBufferData;
//}


#endif
