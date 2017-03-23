#include "cMaterialBuilder.h"
#include <fstream>
#include <iostream>
#include "../AssetBuildLibrary/UtilityFunctions.h"
#include "../../Engine/Platform/Platform.h"
#include "../../Engine/Asserts/Asserts.h"

bool  eae6320::AssetBuild::cMaterialBuilder::load(const char* i_path)
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

		writeMemoryToFile(m_consMattBuffer, completeEffectPath, effectPathlength, completeTexturePathPath,texturePathlength); //

	}
	else
	{
		wereThereErrors = true;
	}

	{
		free(m_consMattBuffer);
		m_consMattBuffer = NULL;
	}

	// Pop the table

	lua_pop(luaState, 1);


OnExit:

	if (luaState)
	{
		int topn = lua_gettop(luaState);
		EAE6320_ASSERT(lua_gettop(luaState) == 0);// this line give trouble if property sheets are not added correctly
		lua_close(luaState);
		luaState = NULL;

	}

	return !wereThereErrors;
}

bool eae6320::AssetBuild::cMaterialBuilder::loadPath(lua_State& io_luaState)
{
	int topn = lua_gettop(&io_luaState);
	bool wereThereErrors = false;
	const char* const key = "color";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	m_consMattBuffer = (Graphics::sMaterial *)malloc(sizeof(Graphics::sMaterial));
	 
	if (lua_istable(&io_luaState, -1))
	{
		lua_pushinteger(&io_luaState, 1);
		lua_gettable(&io_luaState, -2);
		m_consMattBuffer->g_color.r = static_cast<float>(lua_tonumber(&io_luaState, -1)); //multiply by 255.0f ????
		lua_pop(&io_luaState, 1);

		lua_pushinteger(&io_luaState, 2);
		lua_gettable(&io_luaState, -2);
		m_consMattBuffer->g_color.g = static_cast<float>(lua_tonumber(&io_luaState, -1) );
		lua_pop(&io_luaState, 1);

		lua_pushinteger(&io_luaState, 3);
		lua_gettable(&io_luaState, -2);
		m_consMattBuffer->g_color.b = static_cast<float>(lua_tonumber(&io_luaState, -1) );
		lua_pop(&io_luaState, 1);

		lua_pushinteger(&io_luaState, 4);
		lua_gettable(&io_luaState, -2);
		m_consMattBuffer->g_color.a = static_cast<float>(lua_tonumber(&io_luaState, -1) );
		lua_pop(&io_luaState, 1);

		lua_pop(&io_luaState, 1);
	}
	else
	{
		wereThereErrors = true;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;		
	}

	const char* const key2 = "effectPath";
	lua_pushstring(&io_luaState, key2);
	lua_gettable(&io_luaState, -2);
	const char *effectPath = (lua_tostring(&io_luaState, -1));
	completeEffectPath = std::string("data/") + effectPath + std::string("\0");
	effectPathlength = static_cast<uint32_t>(strlen(completeEffectPath.c_str()));
	lua_pop(&io_luaState, 1);

	const char* const key3 = "texturPath";
	lua_pushstring(&io_luaState, key3);
	lua_gettable(&io_luaState, -2);
	std::string textpath = "";
	std::string errormsg = "";
	if (!lua_isnil(&io_luaState, -1))
	{
		const char *texturePath = (lua_tostring(&io_luaState, -1));
		eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(texturePath, "textures", textpath, &errormsg);
		completeTexturePathPath = std::string("data/") + textpath + std::string("\0");
	}	
	else
	{
		completeTexturePathPath = std::string("data/default.dds") + std::string("\0");
	}
	texturePathlength = static_cast<uint32_t>(strlen(completeTexturePathPath.c_str()));
	lua_pop(&io_luaState, 1);


	int tipn = lua_gettop(&io_luaState);
	return !wereThereErrors;
}

bool eae6320::AssetBuild::cMaterialBuilder::Build(const std::vector<std::string>&)
{
	bool wereThereErrors = false;
	load(m_path_source);

	return !wereThereErrors;
}

void eae6320::AssetBuild::cMaterialBuilder::writeMemoryToFile(Graphics::sMaterial* i_constBuffer, std::string i_effectPath, uint32_t EPathLength, std::string i_texturePath, uint32_t TPathLength)
{
	std::ofstream outfile(m_path_target, std::ofstream::binary);
	size_t sizeOfMaterialBuffer = sizeof(Graphics::sMaterial);
	outfile.write(reinterpret_cast<char*>(i_constBuffer), sizeOfMaterialBuffer);
	outfile.write((char*)&EPathLength, sizeof(uint32_t));
	outfile.write(i_effectPath.c_str(), (EPathLength + 1));
	outfile.write(i_texturePath.c_str(), (TPathLength + 1));

	outfile.close();
}