
#include "lighthelper.fx"

//////////////
// TYPEDEFS //
//////////////
cbuffer cbPerFrame{
	Light	gLight;
	int		gLightType;
	float3	gEyePosW;
};

cbuffer cbPerObject{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

struct VertexInputType
{
    float3 position : POSITION;
	float3 parentPosition : PARENTPOSITION;
	float2 radii : RADIUS;	//2 radii values - x is the radius of the object, y is the radius of the parent
};

struct PixelInputType
{
    float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float4 normal	 : NORMAL;
    float4 color	 : COLOR0;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
VertexInputType TreeVertexShader(VertexInputType input)
{
	VertexInputType output;
    // Calculate the position of the vertex against the world, view, and projection matrices
	output.position = input.position;
	output.parentPosition = input.parentPosition;
    output.radii	= input.radii;

    return output;
}

//declare geometry shader statics
const static float PI = 3.14159265f;	
const static int mNumberOfSegments = 10;
const static float rDeltaSegAngle = (2.0f * PI / mNumberOfSegments);

////////////////////////////////////////////////////////////////////////////////
// Geometry Shader
////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(22)]
void TreeGeometryShader(point VertexInputType input[1],inout TriangleStream<PixelInputType> triStream)
{	
	PixelInputType output;
	//draw in reverse to fix winding order
	for (int currSegment = mNumberOfSegments+2; currSegment > 0; currSegment--){
		float x0 = input[0].radii.y * sin(currSegment * rDeltaSegAngle);
		float z0 = input[0].radii.y * cos(currSegment * rDeltaSegAngle);
		output.positionH = float4(x0 + input[0].parentPosition.x,input[0].parentPosition.y,z0 + input[0].parentPosition.z,1);
		output.normal = float4(x0,0.0f,z0,0.0f);
		output.color = float4(0.5f,0.4f,0.3f,0.0f);

		output.normal	 = mul(output.normal,worldMatrix);
		output.positionH = mul(output.positionH, worldMatrix);
		output.positionH = mul(output.positionH, viewMatrix);
		output.positionH = mul(output.positionH, projectionMatrix);
		output.positionW = mul(output.positionH, worldMatrix);
		triStream.Append(output);

		x0 = input[0].radii.x * sin(currSegment * rDeltaSegAngle);
		z0 = input[0].radii.x * cos(currSegment * rDeltaSegAngle);
		output.positionH = float4(x0 + input[0].position.x,input[0].position.y,z0 + input[0].position.z,1);
		output.normal = float4(x0,0.0f,z0,0.0f);
		output.color = float4(0.5f,0.4f,0.3f,1.0f);

		output.normal	 = mul(output.normal,worldMatrix);
		output.positionH = mul(output.positionH, worldMatrix);
		output.positionH = mul(output.positionH, viewMatrix);
		output.positionH = mul(output.positionH, projectionMatrix);
		output.positionW = mul(output.positionH, worldMatrix);
		triStream.Append(output);
		
	}	
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TreePixelShader(PixelInputType input) : SV_Target
{
	// Interpolating normal can make it not be of unit length so normalize it.
    float3 normalW = normalize(input.normal);

	// Compute the lit color for this pixel.
	SurfaceInfo v = {input.positionW, normalW, input.color, input.color};
	float3 litColor = ParallelLight(v, gLight, gEyePosW);

	return float4(litColor, 1.0f);	
}

technique10 TreeTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, TreeVertexShader()));
		SetGeometryShader(CompileShader(gs_4_0, TreeGeometryShader()));
        SetPixelShader(CompileShader(ps_4_0, TreePixelShader()));
		
    }
}
