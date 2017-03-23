// Header Files
//=============

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <sstream>
//#include "../../Asserts/Asserts.h"
//#include "../../Logging/Logging.h"
#include "../../Time/Time.h"
//#include "Includes.h"
#include "../../Platform/Platform.h"
#include "../../Windows/Functions.h"
#include "../Graphics.h"
#include <list>
#include <vector>

// Static Data Initialization
//===========================

namespace
{
	// The is the main window handle from Windows
	HWND s_renderingWindow = NULL;
	// These are Windows-specific interfaces
	HDC s_deviceContext = NULL;
	HGLRC s_openGlRenderingContext = NULL;

	/*	// The vertex buffer holds the data for each vertex
	GLuint s_vertexArrayId = 0;
	#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
	// OpenGL debuggers don't seem to support freeing the vertex buffer
	// and letting the vertex array object hold a reference to it,
	// and so if debug info is enabled an explicit reference is held
	GLuint s_vertexBufferId = 0;
	#endif*/

	// OpenGL encapsulates a matching vertex shader and fragment shader into what it calls a "program".

	// A vertex shader is a program that operates on vertices.
	// Its input comes from a C/C++ "draw call" and is:
	//	* Position
	//	* Any other data we want
	// Its output is:
	//	* Position
	//		(So that the graphics hardware knows which pixels to fill in for the triangle)
	//	* Any other data we want

	// The fragment shader is a program that operates on fragments
	// (or potential pixels).
	// Its input is:
	//	* The data that was output from the vertex shader,
	//		interpolated based on how close the fragment is
	//		to each vertex in the triangle.
	// Its output is:
	//	* The final color that the pixel should be
	GLuint s_programId = 0;
	GLuint SAMPLERSTATEID=0;
	//GLuint s_constantBufferId = 0;
	eae6320::Graphics::Effect effect;
}


// Helper Function Declarations
//=============================

namespace
{
	bool CreateConstantBuffer();
	//bool CreateProgram();
	bool CreateRenderingContext();
	bool CreateSampleState();
	bool DestroySampler();
	//bool CreateVertexBuffer();
	bool LoadAndAllocateShaderProgram(const char* i_path, void*& o_shader, size_t& o_size, std::string* o_errorMessage);
	/*bool LoadFragmentShader(const GLuint i_programId);
	bool LoadVertexShader(const GLuint i_programId);*/

	// This helper struct exists to be able to dynamically allocate memory to get "log info"
	// which will automatically be freed when the struct goes out of scope
	struct sLogInfo
	{
		GLchar* memory;
		sLogInfo(const size_t i_size) { memory = reinterpret_cast<GLchar*>(malloc(i_size)); }
		~sLogInfo() { if (memory) free(memory); }
	};
}

// Interface
//==========

// Render
//-------
namespace
{
	eae6320::ConstBuffer cbufferFrame, cbufferData;
	std::list<eae6320::Graphics::GameObject> gobj_list;
	std::vector<eae6320::Graphics::GameObject2D> gobj2D_vect;
	eae6320::Camera *camera1;	
}

void eae6320::Graphics::RenderFrame()
{

	// Every frame an entirely new image will be created.
	// Before drawing anything, then, the previous image will be erased
	// by "clearing" the image buffer (filling it with a solid color)
	{
		// Black is usually used
		glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		// In addition to the color, "depth" and "stencil" can also be cleared,
		// but for now we only care about color

		glDepthMask(GL_TRUE);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);

		glClearDepth(1.0f);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);


		const GLbitfield clearColorAndDepth = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
		glClear(clearColorAndDepth);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}


	cbufferFrame.s_constantFrameBufferData.g_elapsedSecondCount_total = eae6320::Time::GetElapsedSecondCount_total();
	cbufferFrame.s_constantFrameBufferData.g_transform_cameraToScreen = camera1->GetCameraToScreen();
	cbufferFrame.s_constantFrameBufferData.g_transform_worldToCamera = camera1->GetWorldToCamera();

	cbufferFrame.Bind();
	cbufferFrame.Update(&cbufferFrame.s_constantFrameBufferData);


	// Set the vertex and fragment shaders
	{
		//glUseProgram(s_programId);
		//EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		//effect.Bind();
	}

	// Draw the geometry
	for (std::list<GameObject>::iterator list_iter = gobj_list.begin(); list_iter != gobj_list.end(); list_iter++)
	{
		list_iter->material_pntr->Bind();
		Math::cMatrix_transformation loc_to_World(list_iter->orient_quat, list_iter->pos_vect);
		cbufferData.s_constantDrawBufferData.g_transform_localToWorld = loc_to_World;

		cbufferData.Bind();
		cbufferData.Update(&cbufferData.s_constantDrawBufferData);



		// Bind a specific vertex buffer to the device as a data source
		{
			glBindVertexArray(list_iter->mesh_pntr->s_vertexArrayId);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}

		//// Render triangles from the currently-bound buffers Using index method
		{
			// The mode defines how to interpret multiple vertices as a single "primitive";
			// we define a triangle list
			// (meaning that every primitive is a triangle and will be defined by three vertices)
			const GLenum mode = GL_TRIANGLES;
			// Every index is a 16 bit unsigned integer
			const GLenum indexType = GL_UNSIGNED_SHORT;
			// It's possible to start rendering primitives in the middle of the stream
			const GLvoid* const offset = 0;
			const unsigned int INDEXCOUNT = list_iter->mesh_pntr->indexCount;// 36;
			glDrawElements(mode, INDEXCOUNT, indexType, offset);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}

	}

	for (std::vector<GameObject2D>::iterator vect_iter = gobj2D_vect.begin(); vect_iter != gobj2D_vect.end(); vect_iter++)
	{
		vect_iter->material_pntr->Bind();
		vect_iter->sprite->Draw();
	}

	gobj_list.clear();
	gobj2D_vect.clear();
	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it, the contents of the back buffer must be swapped with the "front buffer"
	// (which is what the user sees)
	{
		BOOL result = SwapBuffers(s_deviceContext);
		EAE6320_ASSERT(result != FALSE);
	}

}
void eae6320::Graphics::GetMesh(Mesh *i_mesh, eae6320::Graphics::Material *i_material, eae6320::Math::cVector i_vect, eae6320::Math::cQuaternion i_quat)
{

	GameObject i_gObj;
	i_gObj.mesh_pntr = i_mesh;
	i_gObj.material_pntr = i_material;
	i_gObj.pos_vect = i_vect;
	i_gObj.orient_quat = i_quat;
	gobj_list.push_back(i_gObj);


}
void eae6320::Graphics::GetSprite(eae6320::Graphics::cSprite *i_spprite, eae6320::Graphics::Material *i_material)
{
	GameObject2D i_gObj;
	i_gObj.material_pntr = i_material;
	i_gObj.sprite = i_spprite;
	gobj2D_vect.push_back(i_gObj);
}
void eae6320::Graphics::GetCam(eae6320::Camera *i_cam)
{
	EAE6320_ASSERT(i_cam != NULL);
	camera1 = i_cam;
}

// Initialization / Clean Up
//==========================

bool eae6320::Graphics::Initialize(const sInitializationParameters& i_initializationParameters)
{
	Math::cMatrix_transformation identity;
	std::string errorMessage;

	s_renderingWindow = i_initializationParameters.mainWindow;

	// Load any required OpenGL extensions
	if (!eae6320::OpenGlExtensions::Load(&errorMessage))
	{
		EAE6320_ASSERTF(false, errorMessage.c_str());
		eae6320::Logging::OutputError(errorMessage.c_str());
		return false;
	}
	// Create an OpenGL rendering context
	if (!CreateRenderingContext())
	{
		EAE6320_ASSERT(false);
		return false;
	}

	if (!CreateSampleState())
	{
		EAE6320_ASSERT(false);
		return false;
	}
	//effect.LoadShader("data/vertex.shader", "data/fragment.shader");
	// Initialize the graphics objects
	/*if (!CreateVertexBuffer())
	{
	EAE6320_ASSERT(false);
	return false;
	}*/
	//if (!CreateProgram())
	//{
	//	EAE6320_ASSERT(false);
	//	return false;
	//}
	//if (!CreateConstantBuffer())
	//{
	//	EAE6320_ASSERT(false);
	//	return false;
	//}
	//s_constantFrameBufferData.g_elapsedSecondCount_total = eae6320::Time::GetElapsedSecondCount_total();
	cbufferFrame.Init(cbufferFrame.FRAME_CALL_BUFFER, sizeof(cbufferData.s_constantFrameBufferData), &cbufferData.s_constantFrameBufferData);

	cbufferFrame.s_constantFrameBufferData.g_transform_cameraToScreen = identity;
	cbufferFrame.s_constantFrameBufferData.g_transform_worldToCamera = identity;
	cbufferData.Init(cbufferData.DRAW_CALL_BUFFER, sizeof(cbufferData.s_constantDrawBufferData), &cbufferData.s_constantDrawBufferData);

	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	EAE6320_ASSERT(glGetError() == GL_NO_ERROR);

	//glClearDepth(1.0f);
	//EAE6320_ASSERT(glGetError() == GL_NO_ERROR);

	//glEnable(GL_CULL_FACE);
	//EAE6320_ASSERT(glGetError() == GL_NO_ERROR);


	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	EAE6320_ASSERT(glGetError() == GL_NO_ERROR);


	return true;

}

bool eae6320::Graphics::CleanUp()
{
	bool wereThereErrors = false;

	if (s_openGlRenderingContext != NULL)
	{
	/*	if (s_programId != 0)
		{
			glDeleteProgram(s_programId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to delete the program: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			s_programId = 0;
		}*/

	//	effect.CleanUp();

#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
		/*	if (s_vertexBufferId != 0)
		{
		const GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &s_vertexBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
		wereThereErrors = true;
		EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
		eae6320::Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
		reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		s_vertexBufferId = 0;
		}*/
#endif
		/*	if (s_vertexArrayId != 0)
		{
		const GLsizei arrayCount = 1;
		glDeleteVertexArrays(arrayCount, &s_vertexArrayId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
		wereThereErrors = true;
		EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
		eae6320::Logging::OutputError("OpenGL failed to delete the vertex array: %s",
		reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		s_vertexArrayId = 0;
		}
		*/
		/*if (s_constantBufferId != 0)
		{
		const GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &s_constantBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
		wereThereErrors = true;
		EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
		eae6320::Logging::OutputError("OpenGL failed to delete the constant buffer: %s",
		reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		s_constantBufferId = 0;
		}*/

		if (!DestroySampler())
		{
			EAE6320_ASSERT(false);
			return false;
		}
		

		cbufferFrame.Cleanup();
		cbufferData.Cleanup();

		if (wglMakeCurrent(s_deviceContext, NULL) != FALSE)
		{
			if (wglDeleteContext(s_openGlRenderingContext) == FALSE)
			{
				wereThereErrors = true;
				const std::string windowsErrorMessage = eae6320::Windows::GetLastSystemError();
				EAE6320_ASSERTF(false, windowsErrorMessage.c_str());
				eae6320::Logging::OutputError("Windows failed to delete the OpenGL rendering context: %s", windowsErrorMessage.c_str());
			}
		}
		else
		{
			wereThereErrors = true;
			const std::string windowsErrorMessage = eae6320::Windows::GetLastSystemError();
			EAE6320_ASSERTF(false, windowsErrorMessage.c_str());
			eae6320::Logging::OutputError("Windows failed to unset the current OpenGL rendering context: %s", windowsErrorMessage.c_str());
		}
		s_openGlRenderingContext = NULL;
	}
	
	if (s_deviceContext != NULL)
	{
		// The documentation says that this call isn't necessary when CS_OWNDC is used
		ReleaseDC(s_renderingWindow, s_deviceContext);
		s_deviceContext = NULL;
	}

	s_renderingWindow = NULL;

	return !wereThereErrors;
}




// Helper Function Declarations
//=============================

bool eae6320::Graphics::CreateVertexBuffer(Mesh *i_mesh, sVertex * vert_buffer, uint16_t * ind_buffer)
{
	bool wereThereErrors = false;
	GLuint vertexBufferId = 0;
	GLuint indexBufferId = 0;

	// Create a vertex array object and make it active
	{
		const GLsizei arrayCount = 1;
		glGenVertexArrays(arrayCount, &(i_mesh->s_vertexArrayId));
		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindVertexArray(i_mesh->s_vertexArrayId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind the vertex array: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex array ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

	// Create a vertex buffer object and make it active
	{
		const GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &vertexBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind the vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}
	// Create a index buffer object and make it active
	{
		const GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &indexBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind the index buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused index buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}


	// Assign the data to the buffer
	{
		// Eventually the vertex data should come from a file but for now it is hard-coded here.
		// You will have to update this in a future assignment
		// (one of the most common mistakes in the class is to leave hard-coded values here).

		

		const unsigned int bufferSize = i_mesh->vertexCount * sizeof(sVertex);
		const unsigned int bufferIndexSize = i_mesh->indexCount * sizeof(uint16_t);

		//Temporarily Create indices over here
		/*	uint16_t indices[] = { 0, 1, 2,
		0, 2, 3 };*/

		
		glBufferData(GL_ARRAY_BUFFER, bufferSize, reinterpret_cast<GLvoid*>(vert_buffer), //(vertexData),
																						  // In our class we won't ever read from the buffer
			GL_STATIC_DRAW);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferIndexSize, reinterpret_cast<GLvoid*>(ind_buffer),//(indices),
																									 // In our class we won't ever read from the buffer
			GL_STATIC_DRAW);

		const GLenum errorCodeIndex = glGetError();
		if (errorCodeIndex != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCodeIndex)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the Index buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCodeIndex)));
			goto OnExit;
		}

	}
	// Initialize the vertex format
	{
		// The "stride" defines how large a single vertex is in the stream of data
		// (or, said another way, how far apart each position element is)
		const GLsizei stride = sizeof(sVertex);

		// Position (0)
		// 2 floats == 8 bytes
		// Offset = 0
		{
			const GLuint vertexElementLocation = 0;
			const GLint elementCount = 3;
			const GLboolean notNormalized = GL_FALSE;	// The given floats should be used as-is
			glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(sVertex, x)));

			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}


			const GLuint colorElementLocation = 1;
			const GLint elementColorCount = 4;
			const GLboolean isNormalized = GL_TRUE;
			glVertexAttribPointer(colorElementLocation, elementColorCount, GL_UNSIGNED_BYTE, isNormalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(sVertex, R)));
			const GLenum errorColorCode = glGetError();

			if (errorColorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(colorElementLocation);
				const GLenum errorColorCode = glGetError();
				if (errorColorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorColorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the COLOR vertex attribute at location %u: %s",
						colorElementLocation, reinterpret_cast<const char*>(gluErrorString(errorColorCode)));
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorColorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the COLOR vertex attribute at location %u: %s",
					colorElementLocation, reinterpret_cast<const char*>(gluErrorString(errorColorCode)));
				goto OnExit;
			}

			/// texture
			const GLuint textureElementLocation = 2;
			const GLint elementTextureCount = 2;
			const GLboolean notNormalizedTexture = GL_FALSE;	// The given floats should be used as-is
			glVertexAttribPointer(textureElementLocation, elementCount, GL_FLOAT, notNormalizedTexture, stride,
				reinterpret_cast<GLvoid*>(offsetof(sVertex, u)));
			const GLenum errorTextureCode = glGetError();

			if (errorTextureCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(textureElementLocation);
				const GLenum errorTextureCode = glGetError();
				if (errorTextureCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorTextureCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the TEXTURE vertex attribute at location %u: %s",
						textureElementLocation, reinterpret_cast<const char*>(gluErrorString(errorTextureCode)));
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorTextureCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the TEXTURE vertex attribute at location %u: %s",
					textureElementLocation, reinterpret_cast<const char*>(gluErrorString(errorTextureCode)));
				goto OnExit;
			}

		}
	}

OnExit:

	if (i_mesh->s_vertexArrayId != 0)
	{
		// Unbind the vertex array
		// (this must be done before deleting the vertex buffer)
		glBindVertexArray(0);
		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// The vertex and index buffer objects can be freed
			// (the vertex array object will still hold references to them,
			// and so they won't actually goes away until it gets freed).
			// Unfortunately debuggers don't work well when these are freed
			// (gDEBugger just doesn't show anything and RenderDoc crashes),
			// and so don't free them if debug info is enabled.
			if (vertexBufferId != 0)
			{
#ifndef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
				const GLsizei bufferCount = 1;
				glDeleteBuffers(bufferCount, &vertexBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to vertex buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
				vertexBufferId = 0;
#else
				i_mesh->s_vertexBufferId = vertexBufferId;
#endif
			}

			if (indexBufferId != 0)
			{
#ifndef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
				const GLsizei bufferCount = 1;
				glDeleteBuffers(bufferCount, &indexBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to vertex buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
				vertexBufferId = 0;
#else
				i_mesh->s_indexBufferId = indexBufferId;
#endif
			}


		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to unbind the vertex or index array: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}



	}

	return !wereThereErrors;
}
namespace
{
	//bool CreateConstantBuffer()
	//{
	//	bool wereThereErrors = false;

	//	// Create a uniform buffer object and make it active
	//	{
	//		const GLsizei bufferCount = 1;
	//		glGenBuffers( bufferCount, &s_constantBufferId );
	//		const GLenum errorCode = glGetError();
	//		if ( errorCode == GL_NO_ERROR )
	//		{
	//			glBindBuffer( GL_UNIFORM_BUFFER, s_constantBufferId );
	//			const GLenum errorCode = glGetError();
	//			if ( errorCode != GL_NO_ERROR )
	//			{
	//				wereThereErrors = true;
	//				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
	//				eae6320::Logging::OutputError( "OpenGL failed to bind the new uniform buffer %u: %s",
	//					s_constantBufferId, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
	//				goto OnExit;
	//			}
	//		}
	//		else
	//		{
	//			wereThereErrors = true;
	//			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
	//			eae6320::Logging::OutputError( "OpenGL failed to get an unused uniform buffer ID: %s",
	//				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
	//			goto OnExit;
	//		}
	//	}
	//	// Fill in the constant buffer
	//	s_constantBufferData.g_elapsedSecondCount_total = eae6320::Time::GetElapsedSecondCount_total();
	//	// Allocate space and copy the constant data into the uniform buffer
	//	{
	//		const GLenum usage = GL_DYNAMIC_DRAW;	// The buffer will be modified frequently and used to draw
	//		glBufferData( GL_UNIFORM_BUFFER, sizeof( s_constantBufferData ), reinterpret_cast<const GLvoid*>( &s_constantBufferData ), usage );
	//		const GLenum errorCode = glGetError();
	//		if ( errorCode != GL_NO_ERROR )
	//		{
	//			wereThereErrors = true;
	//			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
	//			eae6320::Logging::OutputError( "OpenGL failed to allocate the new uniform buffer %u: %s",
	//				s_constantBufferId, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
	//			goto OnExit;
	//		}
	//	}

	//OnExit:

	//	return !wereThereErrors;
	//}

	//bool CreateProgram()
	//{
	//	// Create a program
	//	{
	//		s_programId = glCreateProgram();
	//		const GLenum errorCode = glGetError();
	//		if (errorCode != GL_NO_ERROR)
	//		{
	//			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//			eae6320::Logging::OutputError("OpenGL failed to create a program: %s",
	//				reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//			return false;
	//		}
	//		else if (s_programId == 0)
	//		{
	//			EAE6320_ASSERT(false);
	//			eae6320::Logging::OutputError("OpenGL failed to create a program");
	//			return false;
	//		}
	//	}
	//	// Load and attach the shaders
	//	if (!LoadVertexShader(s_programId))
	//	{
	//		EAE6320_ASSERT(false);
	//		return false;
	//	}
	//	if (!LoadFragmentShader(s_programId))
	//	{
	//		EAE6320_ASSERT(false);
	//		return false;
	//	}
	//	// Link the program
	//	{
	//		glLinkProgram(s_programId);
	//		GLenum errorCode = glGetError();
	//		if (errorCode == GL_NO_ERROR)
	//		{
	//			// Get link info
	//			// (this won't be used unless linking fails
	//			// but it can be useful to look at when debugging)
	//			std::string linkInfo;
	//			{
	//				GLint infoSize;
	//				glGetProgramiv(s_programId, GL_INFO_LOG_LENGTH, &infoSize);
	//				errorCode = glGetError();
	//				if (errorCode == GL_NO_ERROR)
	//				{
	//					sLogInfo info(static_cast<size_t>(infoSize));
	//					GLsizei* dontReturnLength = NULL;
	//					glGetProgramInfoLog(s_programId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
	//					errorCode = glGetError();
	//					if (errorCode == GL_NO_ERROR)
	//					{
	//						linkInfo = info.memory;
	//					}
	//					else
	//					{
	//						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//						eae6320::Logging::OutputError("OpenGL failed to get link info of the program: %s",
	//							reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//						return false;
	//					}
	//				}
	//				else
	//				{
	//					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//					eae6320::Logging::OutputError("OpenGL failed to get the length of the program link info: %s",
	//						reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//					return false;
	//				}
	//			}
	//			// Check to see if there were link errors
	//			GLint didLinkingSucceed;
	//			{
	//				glGetProgramiv(s_programId, GL_LINK_STATUS, &didLinkingSucceed);
	//				errorCode = glGetError();
	//				if (errorCode == GL_NO_ERROR)
	//				{
	//					if (didLinkingSucceed == GL_FALSE)
	//					{
	//						EAE6320_ASSERTF(false, linkInfo.c_str());
	//						eae6320::Logging::OutputError("The program failed to link: %s",
	//							linkInfo.c_str());
	//						return false;
	//					}
	//				}
	//				else
	//				{
	//					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//					eae6320::Logging::OutputError("OpenGL failed to find out if linking of the program succeeded: %s",
	//						reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//					return false;
	//				}
	//			}
	//		}
	//		else
	//		{
	//			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//			eae6320::Logging::OutputError("OpenGL failed to link the program: %s",
	//				reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//			return false;
	//		}
	//	}

	//	return true;
	//}

	bool CreateRenderingContext()
	{
		// Get the device context
		{
			s_deviceContext = GetDC(s_renderingWindow);
			if (s_deviceContext == NULL)
			{
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("Windows failed to get the device context");
				return false;
			}
		}
		// Set the pixel format for the window
		// (This can only be done _once_ for a given window)
		{
			// Get the ID of the desired pixel format
			int pixelFormatId;
			{
				// Create a key/value list of attributes that the pixel format should have
				const int desiredAttributes[] =
				{
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
					WGL_COLOR_BITS_ARB, 24,
					WGL_RED_BITS_ARB, 8,
					WGL_GREEN_BITS_ARB, 8,
					WGL_BLUE_BITS_ARB, 8,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_STENCIL_BITS_ARB, 8,
					// NULL terminator
					NULL
				};
				const float* const noFloatAttributes = NULL;
				const unsigned int onlyReturnBestMatch = 1;
				unsigned int returnedFormatCount;
				if (wglChoosePixelFormatARB(s_deviceContext, desiredAttributes, noFloatAttributes, onlyReturnBestMatch,
					&pixelFormatId, &returnedFormatCount) != FALSE)
				{
					if (returnedFormatCount == 0)
					{
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("Windows couldn't find a pixel format that satisfied the desired attributes");
						return false;
					}
				}
				else
				{
					const std::string windowsErrorMessage = eae6320::Windows::GetLastSystemError();
					EAE6320_ASSERTF(false, windowsErrorMessage.c_str());
					eae6320::Logging::OutputError("Windows failed to choose the closest pixel format: %s", windowsErrorMessage.c_str());
					return false;
				}
			}
			// Set it
			{
				PIXELFORMATDESCRIPTOR pixelFormatDescriptor = { 0 };
				{
					// I think that the values of this struct are ignored
					// and unnecessary when using wglChoosePixelFormatARB() instead of ChoosePixelFormat(),
					// but the documentation is very unclear and so filling it in seems the safest bet
					pixelFormatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
					pixelFormatDescriptor.nVersion = 1;
					pixelFormatDescriptor.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
					pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
					pixelFormatDescriptor.cColorBits = 24;
					pixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;
					pixelFormatDescriptor.cDepthBits = 24;
					pixelFormatDescriptor.cStencilBits = 8;
				}
				if (SetPixelFormat(s_deviceContext, pixelFormatId, &pixelFormatDescriptor) == FALSE)
				{
					const std::string windowsErrorMessage = eae6320::Windows::GetLastSystemError();
					EAE6320_ASSERTF(false, windowsErrorMessage.c_str());
					eae6320::Logging::OutputError("Windows couldn't set the desired pixel format: %s", windowsErrorMessage.c_str());
					return false;
				}
			}
		}
		// Create an OpenGL rendering context and make it current
		{
			// Create the context
			{
				// Create a key/value list of attributes that the context should have
				const int desiredAttributes[] =
				{
					// Request at least version 4.2
					WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
					WGL_CONTEXT_MINOR_VERSION_ARB, 2,
					// Request only "core" functionality and not "compatibility"
					// (i.e. only use modern features of version 4.2)
					WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
					WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
					// NULL terminator
					NULL
				};
				const HGLRC noSharedContexts = NULL;
				s_openGlRenderingContext = wglCreateContextAttribsARB(s_deviceContext, noSharedContexts, desiredAttributes);
				if (s_openGlRenderingContext == NULL)
				{
					DWORD errorCode;
					const std::string windowsErrorMessage = eae6320::Windows::GetLastSystemError(&errorCode);
					std::ostringstream errorMessage;
					errorMessage << "Windows failed to create an OpenGL rendering context: ";
					if ((errorCode == ERROR_INVALID_VERSION_ARB)
						|| (HRESULT_CODE(errorCode) == ERROR_INVALID_VERSION_ARB))
					{
						errorMessage << "The requested version number is invalid";
					}
					else if ((errorCode == ERROR_INVALID_PROFILE_ARB)
						|| (HRESULT_CODE(errorCode) == ERROR_INVALID_PROFILE_ARB))
					{
						errorMessage << "The requested profile is invalid";
					}
					else
					{
						errorMessage << windowsErrorMessage;
					}
					EAE6320_ASSERTF(false, errorMessage.str().c_str());
					eae6320::Logging::OutputError(errorMessage.str().c_str());

					return false;
				}
			}
			// Set it as the rendering context of this thread
			if (wglMakeCurrent(s_deviceContext, s_openGlRenderingContext) == FALSE)
			{
				const std::string windowsErrorMessage = eae6320::Windows::GetLastSystemError();
				EAE6320_ASSERTF(false, windowsErrorMessage.c_str());
				eae6320::Logging::OutputError("Windows failed to set the current OpenGL rendering context: %s",
					windowsErrorMessage.c_str());
				return false;
			}
		}
		
		return true;
	}

	bool CreateSampleState()
	{
		// Create a sampler state object
		{

			const GLsizei samplerStateCount = 1;
			glGenSamplers(samplerStateCount, &SAMPLERSTATEID);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				if (SAMPLERSTATEID != 0)
				{
					// Linear Filtering
					glSamplerParameteri(SAMPLERSTATEID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
					glSamplerParameteri(SAMPLERSTATEID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
					// If UVs go outside [0,1] wrap them around (so that textures can tile)
					glSamplerParameteri(SAMPLERSTATEID, GL_TEXTURE_WRAP_S, GL_REPEAT);
					EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
					glSamplerParameteri(SAMPLERSTATEID, GL_TEXTURE_WRAP_T, GL_REPEAT);
					EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
				}
				else
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("OpenGL failed to create a sampler state");
					return false;
				}
			}
			else
			{
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to create a sampler state: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return false;
			}
		}
		// Bind the sampler state
		{
			// We will never be required to use more than one texture in an Effect in this class,
			// but it doesn't hurt to bind the state to a few extra texture units
			// just in case you decide to try using more
			const GLuint maxTextureUnitCountYouThinkYoullUse = 5;
			for (GLuint i = 0; i < maxTextureUnitCountYouThinkYoullUse; ++i)
			{
				glBindSampler(i, SAMPLERSTATEID);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to bind the sampler state to texture unit %u: %s",
						i, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return false;
				}
			}
			return true;
		}		
	}

	bool DestroySampler()
	{
		if(SAMPLERSTATEID!=0)
			glDeleteSamplers(1, &SAMPLERSTATEID);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		return true;
	}


	//bool LoadFragmentShader(const GLuint i_programId)
	//{
	//	// Verify that compiling shaders at run-time is supported
	//	{
	//		GLboolean isShaderCompilingSupported;
	//		glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
	//		if (!isShaderCompilingSupported)
	//		{
	//			//eae6320::UserOutput::Print( "Compiling shaders at run-time isn't supported on this implementation (this should never happen)" );
	//			return false;
	//		}
	//	}

	//	bool wereThereErrors = false;

	//	// Load the source code from file and set it into a shader
	//	GLuint fragmentShaderId = 0;
	//	eae6320::Platform::sDataFromFile dataFromFile;
	//	{
	//		// Load the shader source code
	//		{
	//			const char* path_sourceCode = "data/fragment.shader";
	//			std::string errorMessage;
	//			if (!eae6320::Platform::LoadBinaryFile(path_sourceCode, dataFromFile, &errorMessage))
	//			{
	//				wereThereErrors = true;
	//				EAE6320_ASSERTF(false, errorMessage.c_str());
	//				eae6320::Logging::OutputError("Failed to load the fragment shader \"%s\": %s",
	//					path_sourceCode, errorMessage.c_str());
	//				goto OnExit;
	//			}
	//		}
	//		// Generate a shader
	//		fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	//		{
	//			const GLenum errorCode = glGetError();
	//			if (errorCode != GL_NO_ERROR)
	//			{
	//				wereThereErrors = true;
	//				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//				eae6320::Logging::OutputError("OpenGL failed to get an unused fragment shader ID: %s",
	//					reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//				goto OnExit;
	//			}
	//			else if (fragmentShaderId == 0)
	//			{
	//				wereThereErrors = true;
	//				EAE6320_ASSERT(false);
	//				eae6320::Logging::OutputError("OpenGL failed to get an unused fragment shader ID");
	//				goto OnExit;
	//			}
	//		}
	//		// Set the source code into the shader
	//		{
	//			const GLsizei shaderSourceCount = 1;
	//			const GLint length = static_cast<GLuint>(dataFromFile.size);
	//			glShaderSource(fragmentShaderId, shaderSourceCount, reinterpret_cast<GLchar**>(&dataFromFile.data), &length);
	//			const GLenum errorCode = glGetError();
	//			if (errorCode != GL_NO_ERROR)
	//			{
	//				wereThereErrors = true;
	//				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//				eae6320::Logging::OutputError("OpenGL failed to set the fragment shader source code: %s",
	//					reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//				goto OnExit;
	//			}
	//		}
	//	}
	//	// Compile the shader source code
	//	{
	//		glCompileShader(fragmentShaderId);
	//		GLenum errorCode = glGetError();
	//		if (errorCode == GL_NO_ERROR)
	//		{
	//			// Get compilation info
	//			// (this won't be used unless compilation fails
	//			// but it can be useful to look at when debugging)
	//			std::string compilationInfo;
	//			{
	//				GLint infoSize;
	//				glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoSize);
	//				errorCode = glGetError();
	//				if (errorCode == GL_NO_ERROR)
	//				{
	//					sLogInfo info(static_cast<size_t>(infoSize));
	//					GLsizei* dontReturnLength = NULL;
	//					glGetShaderInfoLog(fragmentShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
	//					errorCode = glGetError();
	//					if (errorCode == GL_NO_ERROR)
	//					{
	//						compilationInfo = info.memory;
	//					}
	//					else
	//					{
	//						wereThereErrors = true;
	//						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//						eae6320::Logging::OutputError("OpenGL failed to get compilation info about the fragment shader source code: %s",
	//							reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//						goto OnExit;
	//					}
	//				}
	//				else
	//				{
	//					wereThereErrors = true;
	//					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//					eae6320::Logging::OutputError("OpenGL failed to get the length of the fragment shader compilation info: %s",
	//						reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//					goto OnExit;
	//				}
	//			}
	//			// Check to see if there were compilation errors
	//			GLint didCompilationSucceed;
	//			{
	//				glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
	//				errorCode = glGetError();
	//				if (errorCode == GL_NO_ERROR)
	//				{
	//					if (didCompilationSucceed == GL_FALSE)
	//					{
	//						wereThereErrors = true;
	//						EAE6320_ASSERTF(false, compilationInfo.c_str());
	//						eae6320::Logging::OutputError("OpenGL failed to compile the fragment shader: %s",
	//							compilationInfo.c_str());
	//						goto OnExit;
	//					}
	//				}
	//				else
	//				{
	//					wereThereErrors = true;
	//					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//					eae6320::Logging::OutputError("OpenGL failed to find if compilation of the fragment shader source code succeeded: %s",
	//						reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//					goto OnExit;
	//				}
	//			}
	//		}
	//		else
	//		{
	//			wereThereErrors = true;
	//			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//			eae6320::Logging::OutputError("OpenGL failed to compile the fragment shader source code: %s",
	//				reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//			goto OnExit;
	//		}
	//	}
	//	// Attach the shader to the program
	//	{
	//		glAttachShader(i_programId, fragmentShaderId);
	//		const GLenum errorCode = glGetError();
	//		if (errorCode != GL_NO_ERROR)
	//		{
	//			wereThereErrors = true;
	//			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//			eae6320::Logging::OutputError("OpenGL failed to attach the fragment to the program: %s",
	//				reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//			goto OnExit;
	//		}
	//	}

	//OnExit:

	//	if (fragmentShaderId != 0)
	//	{
	//		// Even if the shader was successfully compiled
	//		// once it has been attached to the program we can (and should) delete our reference to it
	//		// (any associated memory that OpenGL has allocated internally will be freed
	//		// once the program is deleted)
	//		glDeleteShader(fragmentShaderId);
	//		const GLenum errorCode = glGetError();
	//		if (errorCode != GL_NO_ERROR)
	//		{
	//			wereThereErrors = true;
	//			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//			eae6320::Logging::OutputError("OpenGL failed to delete the fragment shader ID %u: %s",
	//				fragmentShaderId, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//		}
	//		fragmentShaderId = 0;
	//	}
	//	dataFromFile.Free();

	//	return !wereThereErrors;
	//}

	//bool LoadVertexShader(const GLuint i_programId)
	//{
	//	// Verify that compiling shaders at run-time is supported
	//	{
	//		GLboolean isShaderCompilingSupported;
	//		glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
	//		if (!isShaderCompilingSupported)
	//		{
	//			//eae6320::UserOutput::Print( "Compiling shaders at run-time isn't supported on this implementation (this should never happen)" );
	//			return false;
	//		}
	//	}

	//	bool wereThereErrors = false;

	//	// Load the source code from file and set it into a shader
	//	GLuint vertexShaderId = 0;
	//	eae6320::Platform::sDataFromFile dataFromFile;
	//	{
	//		// Load the shader source code
	//		{
	//			const char* path_sourceCode = "data/vertex.shader";
	//			std::string errorMessage;
	//			if (!eae6320::Platform::LoadBinaryFile(path_sourceCode, dataFromFile, &errorMessage))
	//			{
	//				wereThereErrors = true;
	//				EAE6320_ASSERTF(false, errorMessage.c_str());
	//				eae6320::Logging::OutputError("Failed to load the vertex shader \"%s\": %s",
	//					path_sourceCode, errorMessage.c_str());
	//				goto OnExit;
	//			}
	//		}
	//		// Generate a shader
	//		vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	//		{
	//			const GLenum errorCode = glGetError();
	//			if (errorCode != GL_NO_ERROR)
	//			{
	//				wereThereErrors = true;
	//				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//				eae6320::Logging::OutputError("OpenGL failed to get an unused vertex shader ID: %s",
	//					reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//				goto OnExit;
	//			}
	//			else if (vertexShaderId == 0)
	//			{
	//				wereThereErrors = true;
	//				EAE6320_ASSERT(false);
	//				eae6320::Logging::OutputError("OpenGL failed to get an unused vertex shader ID");
	//				goto OnExit;
	//			}
	//		}
	//		// Set the source code into the shader
	//		{
	//			const GLsizei shaderSourceCount = 1;
	//			const GLint length = static_cast<GLuint>(dataFromFile.size);
	//			glShaderSource(vertexShaderId, shaderSourceCount, reinterpret_cast<GLchar**>(&dataFromFile.data), &length);
	//			const GLenum errorCode = glGetError();
	//			if (errorCode != GL_NO_ERROR)
	//			{
	//				wereThereErrors = true;
	//				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//				eae6320::Logging::OutputError("OpenGL failed to set the vertex shader source code: %s",
	//					reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//				goto OnExit;
	//			}
	//		}
	//	}
	//	// Compile the shader source code
	//	{
	//		glCompileShader(vertexShaderId);
	//		GLenum errorCode = glGetError();
	//		if (errorCode == GL_NO_ERROR)
	//		{
	//			// Get compilation info
	//			// (this won't be used unless compilation fails
	//			// but it can be useful to look at when debugging)
	//			std::string compilationInfo;
	//			{
	//				GLint infoSize;
	//				glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoSize);
	//				errorCode = glGetError();
	//				if (errorCode == GL_NO_ERROR)
	//				{
	//					sLogInfo info(static_cast<size_t>(infoSize));
	//					GLsizei* dontReturnLength = NULL;
	//					glGetShaderInfoLog(vertexShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
	//					errorCode = glGetError();
	//					if (errorCode == GL_NO_ERROR)
	//					{
	//						compilationInfo = info.memory;
	//					}
	//					else
	//					{
	//						wereThereErrors = true;
	//						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//						eae6320::Logging::OutputError("OpenGL failed to get compilation info about the vertex shader source code: %s",
	//							reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//						goto OnExit;
	//					}
	//				}
	//				else
	//				{
	//					wereThereErrors = true;
	//					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//					eae6320::Logging::OutputError("OpenGL failed to get the length of the vertex shader compilation info: %s",
	//						reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//					goto OnExit;
	//				}
	//			}
	//			// Check to see if there were compilation errors
	//			GLint didCompilationSucceed;
	//			{
	//				glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
	//				errorCode = glGetError();
	//				if (errorCode == GL_NO_ERROR)
	//				{
	//					if (didCompilationSucceed == GL_FALSE)
	//					{
	//						wereThereErrors = true;
	//						EAE6320_ASSERTF(false, compilationInfo.c_str());
	//						eae6320::Logging::OutputError("OpenGL failed to compile the vertex shader: %s",
	//							compilationInfo.c_str());
	//						goto OnExit;
	//					}
	//				}
	//				else
	//				{
	//					wereThereErrors = true;
	//					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//					eae6320::Logging::OutputError("OpenGL failed to find if compilation of the vertex shader source code succeeded: %s",
	//						reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//					goto OnExit;
	//				}
	//			}
	//		}
	//		else
	//		{
	//			wereThereErrors = true;
	//			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//			eae6320::Logging::OutputError("OpenGL failed to compile the vertex shader source code: %s",
	//				reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//			goto OnExit;
	//		}
	//	}
	//	// Attach the shader to the program
	//	{
	//		glAttachShader(i_programId, vertexShaderId);
	//		const GLenum errorCode = glGetError();
	//		if (errorCode != GL_NO_ERROR)
	//		{
	//			wereThereErrors = true;
	//			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//			eae6320::Logging::OutputError("OpenGL failed to attach the vertex to the program: %s",
	//				reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//			goto OnExit;
	//		}
	//	}

	//OnExit:

	//	if (vertexShaderId != 0)
	//	{
	//		// Even if the shader was successfully compiled
	//		// once it has been attached to the program we can (and should) delete our reference to it
	//		// (any associated memory that OpenGL has allocated internally will be freed
	//		// once the program is deleted)
	//		glDeleteShader(vertexShaderId);
	//		const GLenum errorCode = glGetError();
	//		if (errorCode != GL_NO_ERROR)
	//		{
	//			wereThereErrors = true;
	//			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//			eae6320::Logging::OutputError("OpenGL failed to delete the vertex shader ID %u: %s",
	//				vertexShaderId, reinterpret_cast<const char*>(gluErrorString(errorCode)));
	//		}
	//		vertexShaderId = 0;
	//	}
	//	dataFromFile.Free();

	//	return !wereThereErrors;
	//}
}
