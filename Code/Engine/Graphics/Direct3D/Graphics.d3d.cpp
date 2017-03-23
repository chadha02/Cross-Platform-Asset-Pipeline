// Header Files
//=============

#include <cstddef>
#include <cstdint>
//#include <D3D11.h>
#include <D3DX11async.h>
#include <D3DX11core.h>
#include <DXGI.h>

/*#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"*/
#include "../../Time/Time.h"
#include "../Graphics.h"
#include "../../Platform/Platform.h"
#include <list>
#include "../Material.h"
#include <vector>


// Static Data Initialization
//===========================

namespace
{
	
	// This is the main window handle from Windows
	HWND s_renderingWindow = NULL;
	// These are D3D interfaces
	//	ID3D11Device* s_direct3dDevice = NULL;
	IDXGISwapChain* s_swapChain = NULL;
	//ID3D11DeviceContext* s_direct3dImmediateContext = NULL;
	ID3D11RenderTargetView* s_renderTargetView = NULL;
	ID3D11DepthStencilView * s_depthBufferView = NULL;

	// The vertex shader is a program that operates on vertices.
	// Its input comes from a C/C++ "draw call" and is:
	//	* Position
	//	* Any other data we want
	// Its output is:
	//	* Position
	//		(So that the graphics hardware knows which pixels to fill in for the triangle)
	//	* Any other data we want
	ID3D11VertexShader* s_vertexShader = NULL;

	// The fragment shader is a program that operates on fragments
	// (or potential pixels).
	// Its input is:
	//	* The data that was output from the vertex shader,
	//		interpolated based on how close the fragment is
	//		to each vertex in the triangle.
	// Its output is:
	//	* The final color that the pixel should be
	ID3D11PixelShader* s_fragmentShader = NULL;

	// D3D has an "input layout" object that associates the layout of the struct above
	// with the input from a vertex shader
	ID3D11InputLayout* s_vertexLayout = NULL;
	eae6320::Platform::sDataFromFile compiledVertexShader;
	
}

// Helper Function Declarations
//=============================

namespace
{
	//bool CreateConstantBuffer();
	bool CreateDevice(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight);
	//bool CreateVertexBuffer( ID3D10Blob& i_compiledShader );
	bool CreateView(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight);
	bool CreateSampleState();
	/*bool LoadFragmentShader();
	bool LoadShader();*/
}

// Interface
//==========

// Render
//-------


namespace
{


	eae6320::ConstBuffer cbufferFrame, cbufferData, cMatData;
	std::list<eae6320::Graphics::GameObject> gobj_list;
	std::vector<eae6320::Graphics::GameObject2D> gobj2D_vect;
	eae6320::Camera *camera1;
}

void eae6320::Graphics::RenderFrame()
{

	// Every frame an entirely new image will be created.
	// Before drawing anything, then, the previous image will be erased
	// by "clearing" the image buffer (filling it with a solid color)
	{
		const float depthToClear = 1.0f;
		const uint8_t stencilToClear = 0; // Arbitrary until stencil is used
		s_direct3dImmediateContext->ClearDepthStencilView(s_depthBufferView, D3D11_CLEAR_DEPTH, depthToClear, stencilToClear);

		// Black is usually used
		float clearColor[4] = { 0.0f, 0.0f, 0.5f, 1.0f };
		s_direct3dImmediateContext->ClearRenderTargetView(s_renderTargetView, clearColor);
	}

	cbufferFrame.s_constantFrameBufferData.g_elapsedSecondCount_total = eae6320::Time::GetElapsedSecondCount_total();
	cbufferFrame.s_constantFrameBufferData.g_transform_cameraToScreen = camera1->GetCameraToScreen();
	cbufferFrame.s_constantFrameBufferData.g_transform_worldToCamera = camera1->GetWorldToCamera();
	cbufferFrame.Bind();
	cbufferFrame.Update(&cbufferFrame.s_constantFrameBufferData);


	//// Set the vertex and fragment shaders
	/*{
		ID3D11ClassInstance** const noInterfaces = NULL;
		const unsigned int interfaceCount = 0;
		s_direct3dImmediateContext->VSSetShader(s_vertexShader, noInterfaces, interfaceCount);
		s_direct3dImmediateContext->PSSetShader(s_fragmentShader, noInterfaces, interfaceCount);
	}*/
	// Specify what kind of data the vertex buffer holds
	{
		// Set the layout (which defines how to interpret a single vertex)
		//s_direct3dImmediateContext->IASetInputLayout(s_vertexLayout);
		// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
		// we have defined the vertex buffer as a triangle list
		// (meaning that every primitive is a triangle and will be defined by three vertices)
		//effect.Bind();
		s_direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	for (std::list<GameObject>::iterator list_iter = gobj_list.begin(); list_iter != gobj_list.end(); list_iter++)
	{
		
		list_iter->material_pntr->Bind();
		Math::cMatrix_transformation loc_to_World(list_iter->orient_quat, list_iter->pos_vect);

		cbufferData.s_constantDrawBufferData.g_transform_localToWorld = loc_to_World;
		
		cbufferData.Bind();
		cbufferData.Update(&cbufferData.s_constantDrawBufferData);
	

		// Bind a specific vertex buffer to the device as a data source
		{
			const unsigned int startingSlot = 0;
			const unsigned int vertexBufferCount = 1;
			// The "stride" defines how large a single vertex is in the stream of data
			const unsigned int bufferStride = sizeof(sVertex);
			// It's possible to start streaming data in the middle of a vertex buffer
			const unsigned int bufferOffset = 0;
			s_direct3dImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &list_iter->mesh_pntr->s_vertexBuffer, &bufferStride, &bufferOffset);
		}


		// Bind a specific index buffer to the device as a data source
		{
			EAE6320_ASSERT(list_iter->mesh_pntr->s_indexBuffer != NULL);
			// Every index is a 16 bit unsigned integer
			const DXGI_FORMAT format = DXGI_FORMAT_R16_UINT;
			// The indices start at the beginning of the buffer
			const unsigned int offset = 0;
			s_direct3dImmediateContext->IASetIndexBuffer(list_iter->mesh_pntr->s_indexBuffer, format, offset);
		}

		// Render triangles from the currently-bound vertex and index buffers
		{
			// It's possible to start rendering primitives in the middle of the stream
			const unsigned int indexOfFirstIndexToUse = 0;
			const unsigned int offsetToAddToEachIndex = 0;
			list_iter->mesh_pntr->indexCount;
			const unsigned int INDEXCOUNT = list_iter->mesh_pntr->indexCount;//2280;
			s_direct3dImmediateContext->DrawIndexed(INDEXCOUNT, indexOfFirstIndexToUse, offsetToAddToEachIndex);
		}

	}


	for (std::vector<GameObject2D>::iterator vect_iter = gobj2D_vect.begin(); vect_iter != gobj2D_vect.end(); vect_iter++)
	{
		vect_iter->material_pntr->Bind();
		vect_iter->sprite->Draw();
	}
	gobj_list.clear();
	gobj2D_vect.clear();

	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it the contents of the back buffer must be "presented"
	// (to the front buffer)
	{
		const unsigned int swapImmediately = 0;
		const unsigned int presentNextFrame = 0;
		const HRESULT result = s_swapChain->Present(swapImmediately, presentNextFrame);
		EAE6320_ASSERT(SUCCEEDED(result));
	}

}





void eae6320::Graphics::GetMesh(Mesh *i_mesh, eae6320::Graphics::Material *i_material,eae6320::Math::cVector i_vect, eae6320::Math::cQuaternion i_quat)
{
	GameObject i_gObj;
	i_gObj.mesh_pntr = i_mesh;
	i_gObj.material_pntr = i_material;
	i_gObj.pos_vect = i_vect;
	i_gObj.orient_quat = i_quat;
	gobj_list.push_back(i_gObj);
}
void eae6320::Graphics::GetSprite(eae6320::Graphics::cSprite *i_spprite, eae6320::Graphics::Material *i_material)
{
	GameObject2D i_gObj;
	i_gObj.material_pntr = i_material;
	i_gObj.sprite = i_spprite;
	gobj2D_vect.push_back(i_gObj);
}

void eae6320::Graphics::GetCam(eae6320::Camera *i_cam)
{
	EAE6320_ASSERT(i_cam != NULL);
	camera1 = i_cam;
}

// Initialization / Clean Up
//

bool eae6320::Graphics::Initialize(const sInitializationParameters& i_initializationParameters)
{
	Math::cMatrix_transformation identity;
	bool wereThereErrors = false;

	s_renderingWindow = i_initializationParameters.mainWindow;


	// Create an interface to a Direct3D device
	if (!CreateDevice(i_initializationParameters.resolutionWidth, i_initializationParameters.resolutionHeight))
	{
		wereThereErrors = true;
		goto OnExit;
	}
	// Initialize the viewport of the device
	if (!CreateView(i_initializationParameters.resolutionWidth, i_initializationParameters.resolutionHeight))
	{
		wereThereErrors = true;
		goto OnExit;
	}
	if (!CreateSampleState())
	{
		wereThereErrors = true;
		goto OnExit;
	}
		
	cbufferFrame.s_constantFrameBufferData.g_transform_cameraToScreen = identity; //camera1->CameraToScreen()
	cbufferFrame.s_constantFrameBufferData.g_transform_worldToCamera = identity; //camera1->WorldToCamera();
	cbufferFrame.Init(cbufferFrame.FRAME_CALL_BUFFER, sizeof(cbufferData.s_constantFrameBufferData), &cbufferData.s_constantFrameBufferData);

	cbufferData.Init(cbufferData.DRAW_CALL_BUFFER, sizeof(cbufferData.s_constantDrawBufferData), &cbufferData.s_constantDrawBufferData);

	//cMatData.Init(cMatData.MATERIAL_CALL_BUFFER, sizeof(cMatData.s_constantMaterialBufferData), &cMatData.s_constantMaterialBufferData);

OnExit:

	// A vertex shader object is used to render the triangle.
	// The compiled vertex shader is the actual compiled code,
	// and once it has been used to create the vertex input layout
	// it can be freed.
	//if (compiledVertexShader)
	//{
	//	compiledVertexShader->Release();
	//	compiledVertexShader = NULL;
	//}

	return !wereThereErrors;

}

bool eae6320::Graphics::CleanUp()
{
	bool wereThereErrors = false;

	if (s_direct3dDevice)
	{
		/*	if (s_vertexBuffer)
		{
		s_vertexBuffer->Release();
		s_vertexBuffer = NULL;
		}
		*/
		/*if (s_vertexLayout)
		{
			s_vertexLayout->Release();
			s_vertexLayout = NULL;
		}
	
		if (s_vertexShader)
		{
			s_vertexShader->Release();
			s_vertexShader = NULL;
		}
		if (s_fragmentShader)
		{
			s_fragmentShader->Release();
			s_fragmentShader = NULL;
		}*/
		//effect.CleanUp();

		//if (s_constantBuffer)
		//{
		//	s_constantBuffer->Release();
		//	s_constantBuffer = NULL;
		//}
		cbufferFrame.Cleanup();
		cbufferData.Cleanup();
	//	cMatData.Cleanup();

		if (s_renderTargetView)
		{
			s_renderTargetView->Release();
			s_renderTargetView = NULL;
		}
		if (s_depthBufferView)
		{
			s_depthBufferView->Release();
			s_depthBufferView = NULL;
		}
		s_direct3dDevice->Release();
		s_direct3dDevice = NULL;
	}
	if (s_direct3dImmediateContext)
	{
		s_direct3dImmediateContext->Release();
		s_direct3dImmediateContext = NULL;
	}
	if (s_samplerState)
	{
		s_samplerState->Release();
		s_samplerState = NULL;
	}
	if (s_swapChain)
	{
		s_swapChain->Release();
		s_swapChain = NULL;
	}

	compiledVertexShader.Free();

	s_renderingWindow = NULL;

	return !wereThereErrors;
}


// Helper Function Definitions
//============================

bool eae6320::Graphics::CreateVertexBuffer(Mesh * i_mesh, sVertex * vert_buffer, uint16_t * ind_buffer)
{
	
	const unsigned int bufferSize = i_mesh->vertexCount * sizeof(sVertex);
	const unsigned int indexBufferSize = i_mesh->indexCount * sizeof(uint16_t);



	D3D11_BUFFER_DESC bufferDescription = { 0 };
	{
		bufferDescription.ByteWidth = bufferSize;
		bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
		bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
		bufferDescription.MiscFlags = 0;
		bufferDescription.StructureByteStride = 0;	// Not used
	}
	D3D11_SUBRESOURCE_DATA initialData = { 0 };
	{
		initialData.pSysMem = vert_buffer;
		//initialData.pSysMem = vertexData;
		// (The other data members are ignored for non-texture buffers)
	}

	D3D11_BUFFER_DESC indexBufferDescription = { 0 };
	{
		indexBufferDescription.ByteWidth = indexBufferSize;
		indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
		indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;;
		indexBufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
		indexBufferDescription.MiscFlags = 0;
		indexBufferDescription.StructureByteStride = 0;	// Not used
	}
	D3D11_SUBRESOURCE_DATA initialIndexData = { 0 };
	{
		initialIndexData.pSysMem = ind_buffer;
		//initialIndexData.pSysMem = indices;
		// (The other data members are ignored for non-texture buffers)
	}

	const HRESULT resultIndex = s_direct3dDevice->CreateBuffer(&indexBufferDescription, &initialIndexData, &i_mesh->s_indexBuffer);



	const HRESULT result = s_direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &i_mesh->s_vertexBuffer);
	if (FAILED(resultIndex))
	{
		EAE6320_ASSERT(false);
		eae6320::Logging::OutputError("Direct3D failed to create the Index buffer with HRESULT %#010x", resultIndex);
		return false;
	}
	if (FAILED(result))
	{
		EAE6320_ASSERT(false);
		eae6320::Logging::OutputError("Direct3D failed to create the vertex buffer with HRESULT %#010x", result);
		return false;
	}

	return true;
}





namespace
{

	bool CreateDevice(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight)
	{
		IDXGIAdapter* const useDefaultAdapter = NULL;
		const D3D_DRIVER_TYPE useHardwareRendering = D3D_DRIVER_TYPE_HARDWARE;
		const HMODULE dontUseSoftwareRendering = NULL;
		unsigned int flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
		{
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
			flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		}
		D3D_FEATURE_LEVEL* const useDefaultFeatureLevels = NULL;
		const unsigned int requestedFeatureLevelCount = 0;
		const unsigned int sdkVersion = D3D11_SDK_VERSION;
		DXGI_SWAP_CHAIN_DESC swapChainDescription = { 0 };
		{
			{
				DXGI_MODE_DESC& bufferDescription = swapChainDescription.BufferDesc;

				bufferDescription.Width = i_resolutionWidth;
				bufferDescription.Height = i_resolutionHeight;
				{
					DXGI_RATIONAL& refreshRate = bufferDescription.RefreshRate;

					refreshRate.Numerator = 0;	// Refresh as fast as possible
					refreshRate.Denominator = 1;
				}
				bufferDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				bufferDescription.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				bufferDescription.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			}
			{
				DXGI_SAMPLE_DESC& multiSamplingDescription = swapChainDescription.SampleDesc;

				multiSamplingDescription.Count = 1;
				multiSamplingDescription.Quality = 0;	// Anti-aliasing is disabled
			}
			swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDescription.BufferCount = 1;
			swapChainDescription.OutputWindow = s_renderingWindow;
			swapChainDescription.Windowed = TRUE;
			swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			swapChainDescription.Flags = 0;
		}
		D3D_FEATURE_LEVEL highestSupportedFeatureLevel;
		const HRESULT result = D3D11CreateDeviceAndSwapChain(useDefaultAdapter, useHardwareRendering, dontUseSoftwareRendering,
			flags, useDefaultFeatureLevels, requestedFeatureLevelCount, sdkVersion, &swapChainDescription,
			&s_swapChain, &eae6320::Graphics::s_direct3dDevice, &highestSupportedFeatureLevel, &eae6320::Graphics::s_direct3dImmediateContext);
		if (SUCCEEDED(result))
		{
			return true;
		}
		else
		{
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("Direct3D failed to create a Direct3D11 device with HRESULT %#010x", result);
			return false;
		}

	
	}

	bool CreateSampleState()
	{
		// Create a sampler state object
		{
			D3D11_SAMPLER_DESC samplerStateDescription;
			{
				// Linear filtering
				samplerStateDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				// If UVs go outside [0,1] wrap them around (so that textures can tile)
				samplerStateDescription.AddressU = samplerStateDescription.AddressV
					= samplerStateDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				// Default values
				samplerStateDescription.MipLODBias = 0.0f;
				samplerStateDescription.MaxAnisotropy = 1;
				samplerStateDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
				samplerStateDescription.BorderColor[0] = samplerStateDescription.BorderColor[1]
					= samplerStateDescription.BorderColor[2] = samplerStateDescription.BorderColor[3] = 1.0f;
				samplerStateDescription.MinLOD = -FLT_MAX;
				samplerStateDescription.MaxLOD = FLT_MAX;
			}
			const HRESULT result = eae6320::Graphics::s_direct3dDevice->CreateSamplerState(&samplerStateDescription, &eae6320::Graphics::s_samplerState);
			if (FAILED(result))
			{
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("Direct3D failed to create a sampler state with HRESULT %#010x", result);
				return false;
			}
		}
		// Bind the sampler state
		{
			const unsigned int samplerStateRegister = 0; // This must match the SamplerState register defined in the shader
			const unsigned int samplerStateCount = 1;
			eae6320::Graphics::s_direct3dImmediateContext->PSSetSamplers(samplerStateRegister, samplerStateCount, &eae6320::Graphics::s_samplerState);
		}
		return true;
	}

	bool CreateView(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight)
	{
		bool wereThereErrors = false;

		// Create and bind the render target view
		ID3D11Texture2D* backBuffer = NULL;
		ID3D11Texture2D* depthBuffer = NULL;


		// Get the back buffer from the swap chain
		HRESULT result;
		{
			const unsigned int bufferIndex = 0;	// This must be 0 since the swap chain is discarded
			result = s_swapChain->GetBuffer(bufferIndex, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
			if (FAILED(result))
			{
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("Direct3D failed to get the back buffer from the swap chain with HRESULT %#010x", result);
				goto OnExit;
			}
		}
		// Create the view
		{
			const D3D11_RENDER_TARGET_VIEW_DESC* const accessAllSubResources = NULL;
			result = eae6320::Graphics::s_direct3dDevice->CreateRenderTargetView(backBuffer, accessAllSubResources, &s_renderTargetView);
			if (FAILED(result))
			{
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("Direct3D failed to create the render target view with HRESULT %#010x", result);
				goto OnExit;
			}
		}
		// Create a depth/stencil buffer and a view of it
		{
			// Unlike the back buffer no depth/stencil buffer exists until and unless we create it
			{
				D3D11_TEXTURE2D_DESC textureDescription = { 0 };
				{
					textureDescription.Width = i_resolutionWidth;
					textureDescription.Height = i_resolutionHeight;
					textureDescription.MipLevels = 1; // A depth buffer has no MIP maps
					textureDescription.ArraySize = 1;
					textureDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 bits for depth and 8 bits for stencil
					{
						DXGI_SAMPLE_DESC& sampleDescription = textureDescription.SampleDesc;
						sampleDescription.Count = 1; // No multisampling
						sampleDescription.Quality = 0; // Doesn't matter when Count is 1
					}
					textureDescription.Usage = D3D11_USAGE_DEFAULT; // Allows the GPU to write to it
					textureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
					textureDescription.CPUAccessFlags = 0; // CPU doesn't need access
					textureDescription.MiscFlags = 0;
				}
				// The GPU renders to the depth/stencil buffer and so there is no initial data
				// (like there would be with a traditional texture loaded from disk)
				const D3D11_SUBRESOURCE_DATA* const noInitialData = NULL;
				const HRESULT result = eae6320::Graphics::s_direct3dDevice->CreateTexture2D(&textureDescription, noInitialData, &depthBuffer);
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to create the depth buffer resource with HRESULT %#010x", result);
					goto OnExit;
				}
			}
			// Create the view
			{
				const D3D11_DEPTH_STENCIL_VIEW_DESC* const noSubResources = NULL;
				const HRESULT result = eae6320::Graphics::s_direct3dDevice->CreateDepthStencilView(depthBuffer, noSubResources, &s_depthBufferView);
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to create the depth stencil view with HRESULT %#010x", result);
					goto OnExit;
				}
			}
		}

		// Bind the views
		{
			const unsigned int renderTargetCount = 1;
			eae6320::Graphics::s_direct3dImmediateContext->OMSetRenderTargets(renderTargetCount, &s_renderTargetView, s_depthBufferView);
		}




		// Specify that the entire render target should be visible
		{
			D3D11_VIEWPORT viewPort = { 0 };
			viewPort.TopLeftX = viewPort.TopLeftY = 0.0f;
			viewPort.Width = static_cast<float>(i_resolutionWidth);
			viewPort.Height = static_cast<float>(i_resolutionHeight);
			viewPort.MinDepth = 0.0f;
			viewPort.MaxDepth = 1.0f;
			const unsigned int viewPortCount = 1;
			eae6320::Graphics::s_direct3dImmediateContext->RSSetViewports(viewPortCount, &viewPort);
		}

	OnExit:

		if (backBuffer)
		{
			backBuffer->Release();
			backBuffer = NULL;
		}
		if (depthBuffer)
		{
			depthBuffer->Release();
			depthBuffer = NULL;
		}

		return !wereThereErrors;
	}



}
