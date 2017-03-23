#include "Effect.h"


eae6320::Graphics::Effect::Effect()
{
	s_vertexLayout = NULL;
	s_vertexShader = NULL;
	s_fragmentShader = NULL;
}
bool eae6320::Graphics::Effect::LoadShader(const char* ShaderPath)
{
	eae6320::Platform::sDataFromFile dataPntr;
	std::string errorMessage;

	if (!eae6320::Platform::LoadBinaryFile(ShaderPath, dataPntr, &errorMessage))
	{
		eae6320::Logging::OutputError("Failed to load the shader file \"%s\": %s", ShaderPath, errorMessage.c_str());
	}

	char* curentLocation = reinterpret_cast<char*>(dataPntr.data);
	uint8_t renderState = *reinterpret_cast<uint8_t*>(curentLocation);
	curentLocation += sizeof(uint8_t);
	s_renderState.Initialize(renderState);
	uint32_t lenthOfPath = *reinterpret_cast<uint32_t*>(curentLocation);
	curentLocation += sizeof(uint32_t);
	const char* vertexPath = reinterpret_cast<char*>(curentLocation);
	LoadVShader(vertexPath);
	curentLocation += (lenthOfPath+1);
	const char* fragmentPath = reinterpret_cast<char*>(curentLocation);
	LoadFShader(fragmentPath);
	return true;
}

bool eae6320::Graphics::Effect::LoadVShader(const char* vertexShaderpath)
{
	eae6320::Platform::sDataFromFile compiledVertexShader;

	{
		std::string errorMessage;
		bool wereThereErrors = false;

		if (!eae6320::Platform::LoadBinaryFile(vertexShaderpath, compiledVertexShader, &errorMessage))
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, errorMessage.c_str());
			eae6320::Logging::OutputError("Failed to load the shader file \"%s\": %s", vertexShaderpath, errorMessage.c_str());
			goto OnExit;
		}

		{
			ID3D11ClassLinkage* const noInterfaces = NULL;
			const HRESULT result = eae6320::Graphics::s_direct3dDevice->CreateVertexShader(
				compiledVertexShader.data, compiledVertexShader.size, noInterfaces, &s_vertexShader);
			if (FAILED(result))
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("Direct3D failed to create the shader %s with HRESULT %#010x", vertexShaderpath, result);
				goto OnExit;
			}
		}
		const unsigned int vertexElementCount = 3;
		D3D11_INPUT_ELEMENT_DESC layoutDescription[vertexElementCount] = { 0,0 };
		{

			{
				D3D11_INPUT_ELEMENT_DESC& positionElement = layoutDescription[0];

				positionElement.SemanticName = "POSITION";
				positionElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
				positionElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				positionElement.InputSlot = 0;
				positionElement.AlignedByteOffset = offsetof(sVertex, x);
				positionElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				positionElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
			}

			{
				D3D11_INPUT_ELEMENT_DESC& colorElement = layoutDescription[1];

				colorElement.SemanticName = "COLOR";
				colorElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
				colorElement.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				colorElement.InputSlot = 0;
				colorElement.AlignedByteOffset = offsetof(sVertex, R);
				colorElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				colorElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
			}

			{
				D3D11_INPUT_ELEMENT_DESC& textureElement = layoutDescription[2];

				textureElement.SemanticName = "TEXCOORD";
				textureElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
				textureElement.Format = DXGI_FORMAT_R32G32_FLOAT;
				textureElement.InputSlot = 0;
				textureElement.AlignedByteOffset = offsetof(sVertex, u);
				textureElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				textureElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
			}
		}

		const HRESULT result = s_direct3dDevice->CreateInputLayout(layoutDescription, vertexElementCount,
			compiledVertexShader.data, compiledVertexShader.size, &s_vertexLayout);
		if (FAILED(result))
		{
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("Direct3D failed to create a vertex input layout with HRESULT %#010x", result);
			return false;
		}

	OnExit:		
		
		compiledVertexShader.Free();
		return !wereThereErrors;
	}
}

bool eae6320::Graphics::Effect::LoadFShader(const char* fragShaderPath)
{
	eae6320::Platform::sDataFromFile compiledFragmentShader;
	bool wereThereErrors = false;
	{
		std::string errorMessage;
		if (!eae6320::Platform::LoadBinaryFile(fragShaderPath, compiledFragmentShader, &errorMessage))
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, errorMessage.c_str());
			eae6320::Logging::OutputError("Failed to load the shader file \"%s\": %s", fragShaderPath, errorMessage.c_str());
			goto OnExit;
		}
	}

	{
		ID3D11ClassLinkage* const noInterfaces = NULL;
		const HRESULT result = eae6320::Graphics::s_direct3dDevice->CreatePixelShader(
			compiledFragmentShader.data, compiledFragmentShader.size, noInterfaces, &s_fragmentShader);
		if (FAILED(result))
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("Direct3D failed to create the shader %s with HRESULT %#010x", fragShaderPath, result);
			goto OnExit;
		}
	}


	OnExit:
	compiledFragmentShader.Free();
	return !wereThereErrors;
}


void eae6320::Graphics::Effect::CleanUp()
{
	if (s_vertexShader)
	{
		s_vertexShader->Release();
		s_vertexShader = NULL;
	}
	if (s_fragmentShader)
	{
		s_fragmentShader->Release();
		s_fragmentShader = NULL;
	}
	if (s_vertexLayout)
	{
		s_vertexLayout->Release();
		s_vertexLayout = NULL;
	}
}

void eae6320::Graphics::Effect::Bind()
{
	{
		ID3D11ClassInstance** const noInterfaces = NULL;
		const unsigned int interfaceCount = 0;
		s_direct3dImmediateContext->VSSetShader(s_vertexShader, noInterfaces, interfaceCount);
		s_direct3dImmediateContext->PSSetShader(s_fragmentShader, noInterfaces, interfaceCount);
		s_renderState.Bind();
		s_direct3dImmediateContext->IASetInputLayout(s_vertexLayout);
	}
}