//#include "Mesh.h"
#include "../Graphics/Graphics.h"
#include <iostream>

Mesh::Mesh()
{
	vertexBuff = NULL;
	indexBuffer = NULL;
	vertexCount = 0;
	indexCount = 0;
}


bool Mesh::load(const char* i_path)
{
	bool wereThereErrors = false;
	eae6320::Platform::sDataFromFile dataPntr;
	std::string errorMessage;


	if (!eae6320::Platform::LoadBinaryFile(i_path, dataPntr, &errorMessage))
	{
		eae6320::Logging::OutputError("Failed to load the shader file \"%s\": %s", i_path, errorMessage.c_str());
	}
	char* curentLocation = reinterpret_cast<char*>(dataPntr.data);
	vertexCount = *reinterpret_cast<int*>(curentLocation);
	curentLocation += sizeof(uint32_t);
	vertexBuff = reinterpret_cast<sVertex*>(curentLocation);
	curentLocation += sizeof(sVertex)*vertexCount;
	indexCount = *reinterpret_cast<int*>(curentLocation);
	curentLocation += sizeof(uint32_t);
	indexBuffer = reinterpret_cast<uint16_t *>(curentLocation);
	if (!Initialize(vertexBuff, indexBuffer))
	{
		wereThereErrors = true;
	}
	dataPntr.Free();

	return !wereThereErrors;
}



bool Mesh::Initialize(sVertex * vert_buffer, uint16_t * ind_buffer)
{

	eae6320::Graphics::CreateVertexBuffer(this, vert_buffer, ind_buffer);
	return true;
}

//bool Mesh::LoadTableValues(lua_State& io_luaState)
//{
//	if (!LoadTableValues_Vertices(io_luaState))
//	{
//		return false;
//	}
//	if (!LoadTableValues_Indexes(io_luaState))
//	{
//		return false;
//	}
//	return true;
//
//}
//
//bool Mesh::LoadTableValues_Vertices(lua_State& io_luaState)
//{
//	bool wereThereErrors = false;
//	int topn = lua_gettop(&io_luaState);
//
//	const char* const key = "vertices";
//	lua_pushstring(&io_luaState, key);
//	lua_gettable(&io_luaState, -2);
//
//
//
//	vertexCount =  luaL_len(&io_luaState, -1);
//	vertexBuff = (sVertex *)malloc(sizeof(sVertex) *vertexCount);
//
//	if (lua_istable(&io_luaState, -1))
//	{
//		for (int i = 0; i < vertexCount; i++)
//		{
//			if (!LoadTableValues_Vertex1(io_luaState, i))
//			{
//				wereThereErrors = true;
//				goto OnExit;
//			}
//		}
//		
//		/*if (!LoadTableValues_Vertex2(io_luaState))
//		{
//			wereThereErrors = true;
//			goto OnExit;
//		}
//		if (!LoadTableValues_Vertex3(io_luaState))
//		{
//			wereThereErrors = true;
//			goto OnExit;
//		}
//		if (!LoadTableValues_Vertex4(io_luaState))
//		{
//			wereThereErrors = true;
//			goto OnExit;
//		}
//		if (!LoadTableValues_Vertex5(io_luaState))
//		{
//			wereThereErrors = true;
//			goto OnExit;
//		}
//		if (!LoadTableValues_Vertex6(io_luaState))
//		{
//			wereThereErrors = true;
//			goto OnExit;
//		}
//		if (!LoadTableValues_Vertex7(io_luaState))
//		{
//			wereThereErrors = true;
//			goto OnExit;
//		}
//		if (!LoadTableValues_Vertex8(io_luaState))
//		{
//			wereThereErrors = true;
//			goto OnExit;
//		}*/
//	}
//	else
//	{
//		wereThereErrors = true;
//		std::cerr << "The value at \"" << key << "\" must be a table "
//			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
//		goto OnExit;
//	}
//
//OnExit:
//
//	// Pop the textures table
//
//	lua_pop(&io_luaState, 1);
//	int tipn = lua_gettop(&io_luaState);
//	return !wereThereErrors;
//}
//
//bool Mesh::LoadTableValues_Vertex1(lua_State& io_luaState, int i)
//{
//	bool wereThereErrors = false;
//	int topn = lua_gettop(&io_luaState);
//	// Right now the asset table is at -2.
//	// and the vertices table is at -1
//	// After the following table operation  asset table will be at -3
//	// and the "Vertex1" table will be at -1:
//	//const char* const key = "vertex1";
//	int key = i + 1;
//	lua_pushnumber(&io_luaState, key);
//	lua_gettable(&io_luaState, -2); // beacause the main table is at -3
//
//
//	if (lua_istable(&io_luaState, -1))
//	{
//
//
//		if (!LoadTableValues_Position(io_luaState, i))
//		{
//			wereThereErrors = true;
//			goto OnExit;
//		}
//		if (!LoadTableValues_Color(io_luaState, i))
//		{
//			wereThereErrors = true;
//			goto OnExit;
//		}
//
//
//	}
//
//OnExit:
//
//	// Pop the textures table
//	lua_pop(&io_luaState, 1);
//	int tipn = lua_gettop(&io_luaState);
//	return !wereThereErrors;
//}
//
//
//
//
//bool Mesh::LoadTableValues_Position(lua_State& io_luaState, int i)
//{
//	// Right now the asset table is at -3.
//	// and the vertices table is at -2 and the individual vertex table is at -1
//	// After the following table operation  asset table will be at -4
//	// and the vertices table will be at -3  individual vertex table is at -2:
//	int topn = lua_gettop(&io_luaState);
//	bool wereThereErrors = false;
//	const char* const key = "position";
//	lua_pushstring(&io_luaState, key);
//
//	lua_gettable(&io_luaState, -2); // 
//	lua_pushinteger(&io_luaState, 1);
//	lua_gettable(&io_luaState, -2);
//	vertexBuff[i].x = static_cast<float>(lua_tonumber(&io_luaState, -1));
//	lua_pop(&io_luaState, 1);
//
//	lua_pushinteger(&io_luaState, 2);
//	lua_gettable(&io_luaState, -2);
//	vertexBuff[i].y = static_cast<float>(lua_tonumber(&io_luaState, -1));
//	lua_pop(&io_luaState, 1);
//	
//	lua_pushinteger(&io_luaState, 3);
//	lua_gettable(&io_luaState, -2);
//	vertexBuff[i].z = static_cast<float>(lua_tonumber(&io_luaState, -1));
//	lua_pop(&io_luaState, 1);
//
//	lua_pop(&io_luaState, 1);
//	int tipn = lua_gettop(&io_luaState);
//	return true;
//}
//
//bool Mesh::LoadTableValues_Color(lua_State& io_luaState, int i)
//{
//	int topn = lua_gettop(&io_luaState);
//	bool wereThereErrors = false;
//	const char* const key = "color";
//	lua_pushstring(&io_luaState, key);
//	lua_gettable(&io_luaState, -2);
//
//	lua_pushinteger(&io_luaState, 1);
//	lua_gettable(&io_luaState, -2);
//	vertexBuff[i].R = static_cast<uint8_t>((lua_tonumber(&io_luaState, -1))* 255.0f);
//	lua_pop(&io_luaState, 1);
//
//	lua_pushinteger(&io_luaState, 2);
//	lua_gettable(&io_luaState, -2);
//	vertexBuff[i].G = static_cast<uint8_t>((lua_tonumber(&io_luaState, -1))* 255.0f);
//	lua_pop(&io_luaState, 1);
//
//
//	lua_pushinteger(&io_luaState, 3);
//	lua_gettable(&io_luaState, -2);
//	vertexBuff[i].B = static_cast<uint8_t>((lua_tonumber(&io_luaState, -1))* 255.0f);
//	lua_pop(&io_luaState, 1);
//
//	lua_pushinteger(&io_luaState, 4);
//	lua_gettable(&io_luaState, -2);
//	vertexBuff[i].A = static_cast<uint8_t>((lua_tonumber(&io_luaState, -1))* 255.0f);
//	lua_pop(&io_luaState, 1);
//
//	lua_pop(&io_luaState, 1);
//	int tipn = lua_gettop(&io_luaState);
//	return true;
//}
//
//bool Mesh::LoadTableValues_Indexes(lua_State& io_luaState)
//{
//	int topn = lua_gettop(&io_luaState);
//	bool wereThereErrors = false;
//	const char* const key = "indexes";
//	lua_pushstring(&io_luaState, key);
//	lua_gettable(&io_luaState, -2);
//	indexCount = luaL_len(&io_luaState, -1);
//	indexBuffer = (uint16_t*)malloc(sizeof(uint16_t)*indexCount);
//
//	if (lua_istable(&io_luaState, -1))
//	{
//
//		for (int i = 1; i <= indexCount; ++i)
//		{
//			lua_pushinteger(&io_luaState, i);
//			lua_gettable(&io_luaState, -2);
//			indexBuffer[i - 1] = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
//			//indexBuffer[indexCount - i] = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
//			lua_pop(&io_luaState, 1);
//		}
//		goto OnExit;
//	}
//	else
//	{
//		wereThereErrors = true;
//		std::cerr << "The value at \"" << key << "\" must be a table "
//			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
//		goto OnExit;
//	}
//
//OnExit:
//
//	// Pop the textures table
//	lua_pop(&io_luaState, 1);
//	int tipn = lua_gettop(&io_luaState);
//	return !wereThereErrors;
//}


bool Mesh::CleanUp()
{
	bool wereThereErrors = false;


#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
		if (s_vertexBufferId != 0)
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
		}
#endif
		if (s_vertexArrayId != 0)
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

#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
		if (s_indexBufferId != 0)
		{
			const GLsizei bufferCount = 1;
			glDeleteBuffers(bufferCount, &s_indexBufferId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to delete the index buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			s_indexBufferId = 0;
		}
#endif


	return !wereThereErrors;
}