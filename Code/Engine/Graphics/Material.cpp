#include "Material.h"

void eae6320::Graphics::Material::Load(const char* i_matPath)
{
	eae6320::Platform::sDataFromFile dataPntr;
	std::string errorMessage;

	if (!eae6320::Platform::LoadBinaryFile(i_matPath, dataPntr, &errorMessage))
	{
		eae6320::Logging::OutputError("Failed to load the shader file \"%s\": %s", i_matPath, errorMessage.c_str());
	}
	char* curentLocation = reinterpret_cast<char*>(dataPntr.data);
	m_constBuffer.s_constantMaterialBufferData = *reinterpret_cast<Graphics::sMaterial*>(curentLocation);
	m_constBuffer.Init(m_constBuffer.MATERIAL_CALL_BUFFER, sizeof(m_constBuffer.s_constantMaterialBufferData), &m_constBuffer.s_constantMaterialBufferData);
	
	curentLocation += sizeof(Graphics::sMaterial);
	uint32_t lenthOfEffectPath = *reinterpret_cast<uint32_t*>(curentLocation);
	curentLocation += sizeof(uint32_t);
	const char* effectPath = reinterpret_cast<char*>(curentLocation);
	curentLocation += (lenthOfEffectPath + 1);
	const char* texturePath = reinterpret_cast<char*>(curentLocation);
	m_texture.Load(texturePath);
	m_effect.LoadShader(effectPath);
	
}

void eae6320::Graphics::Material::CleanUp()
{
	m_constBuffer.Cleanup();
	m_effect.CleanUp();
	m_texture.CleanUp();
}

void eae6320::Graphics::Material::Bind()
{
	m_texture.Bind(0);	
	m_effect.Bind();
	m_constBuffer.Bind();
}
