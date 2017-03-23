#include "ConstBuffer.h"
#include "Graphics.h"
#include "../Time/Time.h"
#include "../Math/Functions.h"



bool eae6320::ConstBuffer::Init(ConstBufferType bufferType, size_t size_buff, void *i_ptr)
{

	if (bufferType == ConstBufferType::FRAME_CALL_BUFFER)
	{
		buffernumber = 0;
	}

	if (bufferType == ConstBufferType::DRAW_CALL_BUFFER)
	{
		buffernumber = 1;
	}
	if (bufferType == ConstBufferType::MATERIAL_CALL_BUFFER)
	{
		buffernumber = 2;
	}

	buffSize = size_buff;
	D3D11_BUFFER_DESC bufferDescription = { 0 };
	{
		// The byte width must be rounded up to a multiple of 16
		bufferDescription.ByteWidth = eae6320::Math::RoundUpToMultiple_powerOf2(static_cast<unsigned int>(buffSize), 16u);;
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;	// The CPU must be able to update the buffer
		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// The CPU must write, but doesn't read
		bufferDescription.MiscFlags = 0;
		bufferDescription.StructureByteStride = 0;	// Not used
	}

	D3D11_SUBRESOURCE_DATA initialData = { 0 };
	{
		// Fill in the constant buffer

		initialData.pSysMem = i_ptr;

	}
	if (buffernumber == 0)
	{
		s_constantFrameBufferData.g_elapsedSecondCount_total = eae6320::Time::GetElapsedSecondCount_total();
	}



	const HRESULT result = eae6320::Graphics::s_direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &s_constantBuffer);
	if (SUCCEEDED(result))
	{
		return true;
	}
	else
	{
		EAE6320_ASSERT(false);
		eae6320::Logging::OutputError("Direct3D failed to create a constant buffer with HRESULT %#010x", result);
		return false;
	}
}

void eae6320::ConstBuffer::Bind()
{
	const unsigned int registerAssignedInShader = buffernumber;
	const unsigned int numberOfBuffers = 1;
	eae6320::Graphics::s_direct3dImmediateContext->VSSetConstantBuffers(registerAssignedInShader, numberOfBuffers, &s_constantBuffer);
	eae6320::Graphics::s_direct3dImmediateContext->PSSetConstantBuffers(registerAssignedInShader, numberOfBuffers, &s_constantBuffer);
}

void eae6320::ConstBuffer::Update(void * i_ptr)
{

	void* memoryToWriteTo = NULL;
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		{
			// Discard previous contents when writing
			const unsigned int noSubResources = 0;
			const D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
			const unsigned int noFlags = 0;
			const HRESULT result = eae6320::Graphics::s_direct3dImmediateContext->Map(s_constantBuffer, noSubResources, mapType, noFlags, &mappedSubResource);
			if (SUCCEEDED(result))
			{
				memoryToWriteTo = mappedSubResource.pData;
			}
			else
			{
				EAE6320_ASSERT(false);
			}
		}
	}

	if (memoryToWriteTo)
	{
		// Copy the new data to the memory that Direct3D has provided

		memcpy(memoryToWriteTo, i_ptr, buffSize);
		// Let Direct3D know that the memory contains the data
		// (the pointer will be invalid after this call)
		const unsigned int noSubResources = 0;
		eae6320::Graphics::s_direct3dImmediateContext->Unmap(s_constantBuffer, noSubResources);
		memoryToWriteTo = NULL;
	}
}

void  eae6320::ConstBuffer::Cleanup()
{
	if (s_constantBuffer)
	{
		s_constantBuffer->Release();
		s_constantBuffer = NULL;
	}
}

