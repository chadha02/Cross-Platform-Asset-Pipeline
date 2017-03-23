#include "cEffectBuilder.h"
#include <fstream>
#include <iostream>
#include "../AssetBuildLibrary/UtilityFunctions.h"
#include "../../Engine/Platform/Platform.h"
#include "../../Engine/Asserts/Asserts.h"

bool  eae6320::AssetBuild::cEffectBuilder::load(const char* i_path)
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
	if (loadPath(*luaState))
	{
		//wereThereErrors = true;

		writeMemoryToFile(renderStateVar,completeVertexPath, vertexPathlength, completeFragmentPath, fragmentPathlength); //

	}
	else
	{
		wereThereErrors = true;
	}

	//{
	//	free(vertBufferPath);
	//	vertBufferPath = NULL;
	//	free(vertBufferPath);
	//	vertBufferPath = NULL;
	//}

	// Pop the table

	lua_pop(luaState, 1);


OnExit:

	if (luaState)
	{
		int topn = lua_gettop(luaState);
		EAE6320_ASSERT(lua_gettop(luaState) == 0);
		lua_close(luaState);
		luaState = NULL;

	}

	return !wereThereErrors;
}

bool eae6320::AssetBuild::cEffectBuilder::loadPath(lua_State& io_luaState)
{
	int topn = lua_gettop(&io_luaState);
	const char* const shaderkey = "shaderPath";
	lua_pushstring(&io_luaState, shaderkey);
	lua_gettable(&io_luaState, -2);
	if (lua_istable(&io_luaState, -1))
	{
		const char* const key1 = "vertex";
		lua_pushstring(&io_luaState, key1);
		lua_gettable(&io_luaState, -2);
		const char *vertBufferPath = (lua_tostring(&io_luaState, -1));
		completeVertexPath = std::string("data/") + vertBufferPath + std::string("\0");
		vertexPathlength = static_cast<uint32_t>(strlen(completeVertexPath.c_str()));
		lua_pop(&io_luaState, 1);

		const char* const key2 = "fragment";
		lua_pushstring(&io_luaState, key2);
		lua_gettable(&io_luaState, -2);
		const char *fragBufferPath = (lua_tostring(&io_luaState, -1));
		completeFragmentPath = std::string("data/") + fragBufferPath + std::string("\0");
		fragmentPathlength = static_cast<uint32_t>(strlen(completeFragmentPath.c_str()));
		lua_pop(&io_luaState, 1);
	}
	lua_pop(&io_luaState, 1);

	int alpha = 0, depth = 0, dual = 0;
	const char* const renderStatekey = "renderState";
	lua_pushstring(&io_luaState, renderStatekey);
	lua_gettable(&io_luaState, -2);
	if (lua_istable(&io_luaState, -1))
	{
		const char* const Renderkey1 = "alphaTransparency";
		lua_pushstring(&io_luaState, Renderkey1);
		lua_gettable(&io_luaState, -2);
		if (!lua_isnil(&io_luaState, -1))
		{
			if (lua_isboolean(&io_luaState, -1))
			{
				int alphaState = lua_toboolean(&io_luaState, -1);
				if (alphaState == 1)
					alpha = 1;
			}
		}
		else
			alpha = 0;
		
		lua_pop(&io_luaState, 1);

		const char* const Renderkey2 = "depthBuffering";
		lua_pushstring(&io_luaState, Renderkey2);
		lua_gettable(&io_luaState, -2);
		if (!lua_isnil(&io_luaState, -1))
		{
			if (lua_isboolean(&io_luaState, -1))
			{
				int depthState = lua_toboolean(&io_luaState, -1);
				if (depthState == 1)
					depth = 2;
			}
		}
		else
			depth = 2;

		lua_pop(&io_luaState, 1);

		const char* const Renderkey3 = "drawBothTriangle";
		lua_pushstring(&io_luaState, Renderkey3);
		lua_gettable(&io_luaState, -2);
		if (!lua_isnil(&io_luaState, -1))
		{
			if (lua_isboolean(&io_luaState, -1))
			{
				int dualState = lua_toboolean(&io_luaState, -1);
				if (dualState == 1)
					dual = 4;
			}
		}		
		else
			dual = 0;
		lua_pop(&io_luaState, 1);
	}
	lua_pop(&io_luaState, 1);
	renderStateVar = alpha + depth + dual;
	int tipn = lua_gettop(&io_luaState);
	return true;
}

void eae6320::AssetBuild::cEffectBuilder::writeMemoryToFile(uint8_t renderStateVar,std::string VertexPath, uint32_t VPathLength, std::string FragmentPath, uint32_t FPathLength)
{
	std::ofstream outfile(m_path_target, std::ofstream::binary);
	outfile.write((char*)&renderStateVar, sizeof(uint8_t));
	outfile.write((char*)&VPathLength, sizeof(uint32_t));
	outfile.write(VertexPath.c_str(), (VPathLength + 1));
	//outfile.write((char*)&FPathLength, sizeof(int));
	outfile.write(FragmentPath.c_str(), (FPathLength + 1));
	outfile.close();
}

bool eae6320::AssetBuild::cEffectBuilder::Build(const std::vector<std::string>&)
{
	bool wereThereErrors = false;	
	load(m_path_source);

	return !wereThereErrors;
}