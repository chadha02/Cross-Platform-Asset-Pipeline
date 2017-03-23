// Header Files
//=============

#include "cMeshBuilder.h"
#include <fstream>
#include <sstream>
#include "../AssetBuildLibrary/UtilityFunctions.h"
#include "../../Engine/Platform/Platform.h"

// Inherited Implementation
//=========================

// Build
//------

bool eae6320::AssetBuild::cMeshBuilder::Build( const std::vector<std::string>& )
{
	bool wereThereErrors = false;

	// Copy the source to the target
	//{
	//	std::string errorMessage;
	//	//EAE6320_TODO	// Copy m_path_source to m_path_target
	//	// There are many reasons that a source should be rebuilt,
	//	// and so even if the target already exists it should just be written over
	//	const bool noErrorIfTargetAlreadyExists = false;
	//	// Since we rely on timestamps to determine when a target was built
	//	// its file time should be updated when the source gets copied
	//	const bool updateTheTargetFileTime = true;
	//	
	//	if (!eae6320::Platform::CopyFile(m_path_source, m_path_target, noErrorIfTargetAlreadyExists, updateTheTargetFileTime, &errorMessage))
	//	{
	//		wereThereErrors = true;
	//		std::ostringstream errorMessage;
	//		errorMessage << "Failed to copy \"" << m_path_source << "\" to \"" << m_path_target << "\": " << errorMessage.str();
	//		eae6320::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), __FILE__ );
	//	}
	//}
	load(m_path_source);
	
	return !wereThereErrors;
}

bool eae6320::AssetBuild::cMeshBuilder::writeMemoryToFile(sVertex* pVertices, uint32_t vertCount, uint16_t *pIndices, uint32_t indCount)
{
	std::ofstream outfile(m_path_target, std::ofstream::binary);
	//int vertexLength = vertCount;
	outfile.write((char*)&vertCount, sizeof(uint32_t));
	size_t sizeOfVertexBuffer = sizeof(sVertex)* vertCount;
	outfile.write(reinterpret_cast<char*>(pVertices), sizeOfVertexBuffer);
	outfile.write((char*)&indCount, sizeof(uint32_t));
	size_t sizeofIndexBuffer = sizeof(uint16_t) * indCount;
	outfile.write(reinterpret_cast<char*>(pIndices), sizeofIndexBuffer);
	outfile.close();
	return true;
}


bool  eae6320::AssetBuild::cMeshBuilder::load(const char* i_path)
{
	bool wereThereErrors = false;

	// Create a new Lua state
	lua_State* luaState = NULL;
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			wereThereErrors = true;
			std::cerr << "Failed to create a new Lua state" << std::endl;
			goto OnExit;
		}
	}

	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
	const int stackTopBeforeLoad = lua_gettop(luaState);
	{
		const int luaResult = luaL_loadfile(luaState, i_path);
		if (luaResult != LUA_OK)
		{
			wereThereErrors = true;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			goto OnExit;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	{
		const int argumentCount = 0;
		const int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		const int noMessageHandler = 0;
		const int luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const int returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
			if (returnedValueCount == 1)
			{
				// A correct asset file _must_ return a table
				if (!lua_istable(luaState, -1))
				{
					wereThereErrors = true;
					std::cerr << "Asset files must return a table (instead of a " <<
						luaL_typename(luaState, -1) << ")" << std::endl;
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				std::cerr << "Asset files must return a single table (instead of " <<
					returnedValueCount << " values)" << std::endl;
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			goto OnExit;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1
	if (LoadTableValues(*luaState))
	{
		//wereThereErrors = true;
		
		writeMemoryToFile(vertexBuff, vertexCount, indexBuffer, indexCount);

	}
	else
	{
		wereThereErrors = true;
	}

	{
		free(vertexBuff);
		vertexBuff = NULL;
		free(indexBuffer);
		indexBuffer = NULL;
	}

	// Pop the table

	lua_pop(luaState, 1);


OnExit:

	if (luaState)
	{
		// If I haven't made any mistakes
		// there shouldn't be anything on the stack,
		// regardless of any errors encountered while loading the file:

		int topn = lua_gettop(luaState);
		EAE6320_ASSERT(lua_gettop(luaState) == 0);

		lua_close(luaState);
		luaState = NULL;

	}

	return !wereThereErrors;
}

bool eae6320::AssetBuild::cMeshBuilder::LoadTableValues(lua_State& io_luaState)
{
	if (!LoadTableValues_Vertices(io_luaState))
	{
		return false;
	}
	if (!LoadTableValues_UV(io_luaState))
	{
		return false;
	}
	if (!LoadTableValues_Indexes(io_luaState))
	{
		return false;
	}

	return true;

}

bool eae6320::AssetBuild::cMeshBuilder::LoadTableValues_Vertices(lua_State& io_luaState)
{
	bool wereThereErrors = false;
	int topn = lua_gettop(&io_luaState);

	const char* const key = "vertices";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);



	vertexCount = luaL_len(&io_luaState, -1);
	vertexBuff = (sVertex *)malloc(sizeof(sVertex) *vertexCount);

	if (lua_istable(&io_luaState, -1))
	{
		for (uint32_t i = 0; i < vertexCount; i++) ///////////////////////////////
		{
			if (!LoadTableValues_Vertex1(io_luaState, i))
			{
				wereThereErrors = true;
				goto OnExit;
			}
		}
	}
	else
	{
		wereThereErrors = true;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		goto OnExit;
	}

OnExit:

	// Pop the textures table

	lua_pop(&io_luaState, 1);
	int tipn = lua_gettop(&io_luaState);
	return !wereThereErrors;
}

bool eae6320::AssetBuild::cMeshBuilder::LoadTableValues_Vertex1(lua_State& io_luaState, int i)
{
	bool wereThereErrors = false;
	int topn = lua_gettop(&io_luaState);
	// Right now the asset table is at -2.
	// and the vertices table is at -1
	// After the following table operation  asset table will be at -3
	// and the "Vertex1" table will be at -1:
	//const char* const key = "vertex1";
	int key = i + 1;
	lua_pushnumber(&io_luaState, key);
	lua_gettable(&io_luaState, -2); // beacause the main table is at -3


	if (lua_istable(&io_luaState, -1))
	{


		if (!LoadTableValues_Position(io_luaState, i))
		{
			wereThereErrors = true;
			goto OnExit;
		}
		if (!LoadTableValues_Color(io_luaState, i))
		{
			wereThereErrors = true;
			goto OnExit;
		}


	}

OnExit:

	// Pop the vertex1 table
	lua_pop(&io_luaState, 1);
	int tipn = lua_gettop(&io_luaState);
	return !wereThereErrors;
}

bool eae6320::AssetBuild::cMeshBuilder::LoadTableValues_Position(lua_State& io_luaState, int i)
{
	// Right now the asset table is at -3.
	// and the vertices table is at -2 and the individual vertex table is at -1
	// After the following table operation  asset table will be at -4
	// and the vertices table will be at -3  individual vertex table is at -2:
	int topn = lua_gettop(&io_luaState);
	bool wereThereErrors = false;
	const char* const key = "position";
	lua_pushstring(&io_luaState, key);

	lua_gettable(&io_luaState, -2); // 
	lua_pushinteger(&io_luaState, 1);
	lua_gettable(&io_luaState, -2);
	vertexBuff[i].x = static_cast<float>(lua_tonumber(&io_luaState, -1));
	lua_pop(&io_luaState, 1);

	lua_pushinteger(&io_luaState, 2);
	lua_gettable(&io_luaState, -2);
	vertexBuff[i].y = static_cast<float>(lua_tonumber(&io_luaState, -1));
	lua_pop(&io_luaState, 1);

	lua_pushinteger(&io_luaState, 3);
	lua_gettable(&io_luaState, -2);
	vertexBuff[i].z = static_cast<float>(lua_tonumber(&io_luaState, -1));
	lua_pop(&io_luaState, 1);

	lua_pop(&io_luaState, 1);
	int tipn = lua_gettop(&io_luaState);
	return true;
}

bool eae6320::AssetBuild::cMeshBuilder::LoadTableValues_Color(lua_State& io_luaState, int i)
{
	int topn = lua_gettop(&io_luaState);
	bool wereThereErrors = false;
	const char* const key = "color";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	lua_pushinteger(&io_luaState, 1);
	lua_gettable(&io_luaState, -2);
	vertexBuff[i].R = static_cast<uint8_t>((lua_tonumber(&io_luaState, -1))* 255.0f);
	lua_pop(&io_luaState, 1);

	lua_pushinteger(&io_luaState, 2);
	lua_gettable(&io_luaState, -2);
	vertexBuff[i].G = static_cast<uint8_t>((lua_tonumber(&io_luaState, -1))* 255.0f);
	lua_pop(&io_luaState, 1);


	lua_pushinteger(&io_luaState, 3);
	lua_gettable(&io_luaState, -2);
	vertexBuff[i].B = static_cast<uint8_t>((lua_tonumber(&io_luaState, -1))* 255.0f);
	lua_pop(&io_luaState, 1);

	lua_pushinteger(&io_luaState, 4);
	lua_gettable(&io_luaState, -2);
	vertexBuff[i].A = static_cast<uint8_t>((lua_tonumber(&io_luaState, -1))* 255.0f);
	lua_pop(&io_luaState, 1);

	lua_pop(&io_luaState, 1);
	int tipn = lua_gettop(&io_luaState);
	return true;
}

bool eae6320::AssetBuild::cMeshBuilder::LoadTableValues_Indexes(lua_State& io_luaState)
{
	int topn = lua_gettop(&io_luaState);
	bool wereThereErrors = false;
	const char* const key = "indexes";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	indexCount = luaL_len(&io_luaState, -1);
	indexBuffer = (uint16_t*)malloc(sizeof(uint16_t)*indexCount);

	if (lua_istable(&io_luaState, -1))
	{

		for (uint32_t i = 1; i <= indexCount; ++i)
		{
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
#if defined( EAE6320_PLATFORM_GL )
			indexBuffer[i - 1] = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
#elif defined( EAE6320_PLATFORM_D3D )
			indexBuffer[indexCount - i] = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
#endif
			lua_pop(&io_luaState, 1);
		}
		goto OnExit;
	}
	else
	{
		wereThereErrors = true;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		goto OnExit;
	}

OnExit:

	// Pop the textures table
	lua_pop(&io_luaState, 1);
	int tipn = lua_gettop(&io_luaState);
	return !wereThereErrors;
}

bool eae6320::AssetBuild::cMeshBuilder::LoadTableValues_UV(lua_State& io_luaState)
{
	bool wereThereErrors = false;
	const char* const key = "UV";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	

	if (lua_istable(&io_luaState, -1))
	{
		const uint32_t length = luaL_len(&io_luaState, -1);
		int currentVertex = 0;
		for (uint32_t i = 1; i <= length; ++i)
		{
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);

			vertexBuff[currentVertex].u = static_cast<float>(lua_tonumber(&io_luaState, -1));
			lua_pop(&io_luaState, 1);

			i++;
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
#if defined( EAE6320_PLATFORM_GL )
			vertexBuff[currentVertex].v = static_cast<float>(lua_tonumber(&io_luaState, -1));
#elif defined( EAE6320_PLATFORM_D3D )
			vertexBuff[currentVertex].v = 1.0f - static_cast<float>(lua_tonumber(&io_luaState, -1));
#endif
			lua_pop(&io_luaState, 1);
			currentVertex++;
		}
		goto OnExit;
	}
OnExit:

	// Pop the textures table
	lua_pop(&io_luaState, 1);
	//int tipn = lua_gettop(&io_luaState);

	return !wereThereErrors;

}
