#pragma once
#ifndef EAE6320_CEFFECTBUILDER_H
#define EAE6320_CEFFECTBUILDER_H

#include "../../External/Lua/5.2.3/src/lua.hpp"
#include "../AssetBuildLibrary/cbBuilder.h"

namespace eae6320
{
	namespace AssetBuild
	{
		class cEffectBuilder : public cbBuilder
		{
		private:
			std::string completeVertexPath;
			std::string completeFragmentPath;
			uint32_t fragmentPathlength;
			uint32_t vertexPathlength;
			uint8_t renderStateVar;
			virtual bool Build(const std::vector<std::string>& i_arguments);
			virtual void writeMemoryToFile(uint8_t renderStateVar,std::string VertexPath, uint32_t VPathLength, std::string FragmentPath, uint32_t FPathLength);
		public:
			bool load(const char * i_path);
			bool loadPath(lua_State& io_luaState);
		};
	}
}
#endif // EAE6320_CEFFECTBUILDER_H