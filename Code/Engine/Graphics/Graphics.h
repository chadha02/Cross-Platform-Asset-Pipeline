/*
	This file contains the function declarations for graphics
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Header Files
//=============

#include "Configuration.h"

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include "../Windows/Includes.h"
#endif
#include "GameObject.h"
#include "GameObject2D.h"
//#include "sVertex.h"
#include "ConstBuffer.h"
#include "../Graphics/Camera.h"
#include "../Graphics/Mesh.h"
#include "Material.h"
#include "cSprite.h"

// Interface
//==========

namespace eae6320
{
	//class Mesh;
	namespace Graphics
	{
		// Render
		//-------
#if defined( EAE6320_PLATFORM_D3D ) 
		//extern ID3D10Blob* compiledVertexShader;
	/*	extern ID3D11DeviceContext* s_direct3dImmediateContext;
		extern ID3D11Device* s_direct3dDevice;*/
		
#endif
		void RenderFrame( );
		bool CreateVertexBuffer(Mesh *i_mesh, sVertex * vert_buffer, uint16_t * ind_buffer);
	
		// Initialization / Clean Up
		//--------------------------

		struct sInitializationParameters
		{
#if defined( EAE6320_PLATFORM_WINDOWS )
			HWND mainWindow;
	#if defined( EAE6320_PLATFORM_D3D )
			unsigned int resolutionWidth, resolutionHeight;
	#elif defined( EAE6320_PLATFORM_GL )
			HINSTANCE thisInstanceOfTheApplication;
	#endif
#endif
		};

		bool Initialize( const sInitializationParameters& i_initializationParameters );
		bool CleanUp();
		void GetCam(Camera *cam);
		void GetMesh(Mesh *i_mesh, Material *i_material, Math::cVector pos_vect, Math::cQuaternion orien_quat);
		void GetSprite(cSprite *i_sprite, Material *i_material);
		
		namespace ShaderTypes
		{
			enum eShaderType
			{
				Unknown,
				Vertex,
				Fragment,
			};
		}		
	}
}

#endif	// EAE6320_GRAPHICS_H
