#pragma once
#ifndef EAE6320_CMATERIALBUILDER_H
#define EAE6320_CMATERIALBUILDER_H

#include "../../External/Lua/5.2.3/src/lua.hpp"
#include "../AssetBuildLibrary/cbBuilder.h"
#include "../../Engine/Graphics/ConstBuffer.h"

namespace eae6320
{
	namespace AssetBuild
	{
		class cMaterialBuilder : public cbBuilder
		{
		private:
			Graphics::sMaterial *m_consMattBuffer;
			std::string completeEffectPath;
			std::string completeTexturePathPath;
			uint32_t effectPathlength;
			uint32_t texturePathlength;
			virtual bool Build(const std::vector<std::string>& i_arguments);
			virtual void writeMemoryToFile(Graphics::sMaterial* i_constBuffer, std::string i_effectPath, uint32_t EPathLength, std::string i_texturePath, uint32_t TPathLength);
		public:
			bool load(const char * i_path);
			bool loadPath(lua_State& io_luaState);

		};
	}
}

#endif // EAE6320_CMATERIALBUILDER_H