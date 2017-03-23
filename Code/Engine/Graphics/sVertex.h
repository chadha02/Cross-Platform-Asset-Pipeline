#pragma once
#include <iostream>

struct sVertex
{
	// POSITION
	// 2 floats == 8 bytes
	// Offset = 0
	float x, y,z;
	float u, v;
	uint8_t R, G, B, A;
};