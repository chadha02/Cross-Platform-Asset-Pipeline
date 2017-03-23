#ifndef EAE6320_MESH_H
#define EAE6320_MESH_H

#include "sVertex.h"
#include "Context.h"
#include "Configuration.h"
#include "../../External/Lua/5.2.3/src/lua.hpp"
//#include "../../Tools/MeshBuilder/cMeshBuilder.h"
#include "../Platform/Platform.h"

class Mesh
{
	
public:
	int vertexCount;
	int indexCount;
#if defined( EAE6320_PLATFORM_D3D )
	// The vertex buffer holds the data for each vertex
	ID3D11Buffer* s_vertexBuffer = NULL;
	ID3D11Buffer* s_indexBuffer = NULL;
	

#elif defined( EAE6320_PLATFORM_GL )

	// The vertex buffer holds the data for each vertex
	GLuint s_vertexArrayId = 0;
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
	// OpenGL debuggers don't seem to support freeing the vertex buffer
	// and letting the vertex array object hold a reference to it,
	// and so if debug info is enabled an explicit reference is held
	GLuint s_vertexBufferId = 0;
	GLuint s_indexBufferId = 0;
#endif
#endif
private:
	sVertex * vertexBuff;
	uint16_t * indexBuffer;
	
	bool Initialize(sVertex * vert_buffer, uint16_t * ind_buffer);

public:
	Mesh();
	bool load(const char * i_path);
	//void RenderFrame(float x, float y);
	
	bool CleanUp();
	

	//bool LoadTableValues(lua_State& io_luaState);
	//bool LoadTableValues_Vertices(lua_State& io_luaState);

	//bool LoadTableValues_Vertex1(lua_State& io_luaState, int i);


	//bool LoadTableValues_Position(lua_State& io_luaState, int);
	//bool LoadTableValues_Indexes(lua_State& io_luaState);
	//bool LoadTableValues_Color(lua_State& io_luaState, int);

};

#endif