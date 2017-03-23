/*
	This is an example of a fragment shader
*/
#if defined( EAE6320_PLATFORM_D3D )
	// Constants
	//==========

	cbuffer constantBuffer : register( b0 )
	{
		float g_elapsedSecondCount_total;
	}

	// Entry Point
	//============

	void main(

		// Input
		//======

		// The GPU provides us with position
		in float4 i_position : SV_POSITION,

		// Whatever arbitrary data (i.e. everything excluding position) was output from the vertex shader
		// will be interpolated across the triangle and given as input to the fragment shader
		in float4 i_color : COLOR,

		// Output
		//=======

		// Whatever color value is output from the fragment shader
		// will determine the color of the corresponding pixel on the screen
		out float4 o_color : SV_TARGET

		)
	{
		// For now set the fragment as the interpolated color
		o_color = i_color;
	}
#elif defined( EAE6320_PLATFORM_GL )
	// Constants
	//==========

	layout( std140, binding = 0 ) uniform constantBuffer_frameCall
	{
		mat4 g_transform_worldToCamera;
		mat4 g_transform_cameraToScreen;
		float g_elapsedSecondCount_total;
	};

	// Input
	//======

	// Whatever arbitrary data (i.e. everything excluding position) was output from the vertex shader
	// will be interpolated across the triangle and given as input to the fragment shader

	layout( location = 0 ) in vec4 i_color;

	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out vec4 o_color;

	// Entry Point
	//============

	void main()
	{
		// For now set the fragment as the interpolated color
		o_color = i_color;
	}
