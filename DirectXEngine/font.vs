

cbuffer PerFrameBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position: POSITION;
	float2 tex: TEXCOORD0;
};

PixelInputType FontVertexShader(VertexInputType in)
{
	PixelInputType out;


	out.position = mul(in.position,worldMatrix);
	out.position = mul(out.position,viewMatrix);
	out.position = mul(out.position,porjectionMatrix);

	out.tex = in.tex;

	return out;
}

