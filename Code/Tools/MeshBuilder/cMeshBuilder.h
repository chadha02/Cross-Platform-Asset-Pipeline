/*
	This class builds meshes
*/

#ifndef EAE6320_CMESHBUILDER_H
#define EAE6320_CMESHBUILDER_H

// Header Files
//=============

#include "../AssetBuildLibrary/cbBuilder.h"
#include "../../Engine/Graphics/Graphics.h"
#include "../../External/Lua/5.2.3/src/lua.hpp"

// Class Declaration
//==================

namespace eae6320
{
	namespace AssetBuild
	{
		class cMeshBuilder : public cbBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------
			uint32_t vertexCount;
			uint32_t indexCount;
			sVertex * vertexBuff;
			uint16_t * indexBuffer;
			virtual bool Build( const std::vector<std::string>& i_arguments );
			virtual bool writeMemoryToFile(sVertex* pVertices, uint32_t vertCount, uint16_t* pIndices, uint32_t indCount);
		public:

			bool load(const char * i_path);
			bool LoadTableValues(lua_State& io_luaState);
			bool LoadTableValues_Vertices(lua_State& io_luaState);

			bool LoadTableValues_Vertex1(lua_State& io_luaState, int i);


			bool LoadTableValues_Position(lua_State& io_luaState, int);
			bool LoadTableValues_Indexes(lua_State& io_luaState);
			bool LoadTableValues_Color(lua_State& io_luaState, int);
			bool LoadTableValues_UV(lua_State& io_luaState);
		};
	}
}

#endif	// EAE6320_CMESHBUILDER_H
