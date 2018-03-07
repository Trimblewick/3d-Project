#pragma once
typedef union
{
	struct { float x, y, z, w; };
	struct { float r, g, b, a; };
} float4;

typedef union
{
	struct { float u; float v; };
	struct { float x; float y; };
} float2;