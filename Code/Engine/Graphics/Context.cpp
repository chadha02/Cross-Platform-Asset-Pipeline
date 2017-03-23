#include "Context.h"

#if defined( EAE6320_PLATFORM_D3D )

ID3D11DeviceContext*  eae6320::Graphics::s_direct3dImmediateContext = NULL;
ID3D11Device* eae6320::Graphics::s_direct3dDevice = NULL;
ID3D11SamplerState* eae6320::Graphics::s_samplerState = NULL;
#endif