/*
	This is an example of a vertex shader
*/

// The version of GLSL to use must come first
#version 420

// Constants
//==========

layout( std140, binding = 0 ) uniform constantBuffer_frameCall
{
	mat4 g_transform_worldToCamera;
	mat4 g_transform_cameraToScreen;
	float g_elapsedSecondCount_total;
};
layout( std140, binding = 1 ) uniform constantBuffer_drawCall
{
	mat4 g_transform_localToWorld;
};

// Input
//======

// The locations assigned are arbitrary
// but must match the C calls to glVertexAttribPointer()

// These values come from one of the sVertex that we filled the vertex buffer with in C code
layout( location = 0 ) in vec3 i_vertexPosition_local;
layout( location = 1 ) in vec4 i_color;

// Output
//=======

// None; the vertex shader must always output a position value,
// but unlike HLSL where the value is explicit
// GLSL has an implicit required variable called "gl_Position"

// Any other data is optional; the GPU doesn't know or care what it is,
// and will merely interpolate it across the triangle
// and give us the resulting interpolated value in a fragment shader.
// It is then up to us to use it however we want to.
// The locations are used to match the vertex shader outputs
// with the fragment shader inputs
// (note that Direct3D uses arbitrarily assignable "semantics").
layout( location = 0 ) out vec4 o_color;

// Entry Point
//============

void main()
{
	// Calculate the position of this vertex on screen
	{
		// The position stored in the vertex is in "local space",
		// meaning that it is relative to the center (or "origin", or "pivot")
		// of the mesh.
		// The graphics hardware needs the position of the vertex
		// in normalized device coordinates,
		// meaning where the position of the vertex should be drawn
		// on the screen.
		// This position that we need to output, then,
		// is the result of taking the original vertex in local space
		// and transforming it into "screen space".

		// Any matrix transformations that include translation
		// will operate on a vec4 position,
		// which _must_ have 1 for the w value
		vec4 vertexPosition_local = vec4( i_vertexPosition_local, 1.0 );
		// Transform the vertex from local space into world space
		vec4 vertexPosition_world = vertexPosition_local * g_transform_localToWorld;
		// Transform the vertex from world space into camera space
		vec4 vertexPosition_camera = vertexPosition_world * g_transform_worldToCamera;
		// Project the vertex from camera space into screen space
		gl_Position = vertexPosition_camera * g_transform_cameraToScreen;
	}
	// Pass the input color to the fragment shader unchanged
	{
		o_color = i_color;
	}
}
