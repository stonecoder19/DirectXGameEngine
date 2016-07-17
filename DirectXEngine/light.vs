cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float padding;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};


PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

	//change the position vector to be 4 units
	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)worldMatrix);


	//normalize vector
	output.normal = normalize(output.normal);

	//calculate positon of vector in world
	worldPosition = mul(input.position, worldMatrix);

	//determine viewing direction based on position of camera and position of vertex in world
	output.viewDirection  = cameraPosition.xyz - worldPosition.xyz;

	//normalize	the viewing direction vector
	output.viewDirection = normalize(output.viewDirection);

	return output;

}