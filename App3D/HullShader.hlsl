
cbuffer CAMERA_CONSTANT_BUFFER : register(b0)
{
    matrix view;
    matrix proj;
    float3 cameraPosition;
};

//Per control point in the input patch
struct VertexShaderOutput
{
    float4 worldPosition : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 nor : NORMAL;

    matrix world : WORLD;
};

struct HS_CONSTANT_DATA_OUTPUT //Triangle patches are defined by their 3 edges and the area within those points
{
    float EdgeTessFactor[3] : SV_TessFactor;
    float InsideTessFactor	: SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3 //Better than hardcoding, since we use this in both our function calls that have the InputPatch

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VertexShaderOutput, NUM_CONTROL_POINTS> ip)
{
    HS_CONSTANT_DATA_OUTPUT output = (HS_CONSTANT_DATA_OUTPUT) 0;

	//Constants
    float maxDist = 20.0f;
    float minDist = 2.0f;
    float maxTess = 16.0f;
    float minTess = 1.0f;
    
    //Store triangle centerpoint as well as distance from camera
    float3 center = (ip[0].worldPosition + ip[1].worldPosition + ip[2].worldPosition) / 3.0f; //Triangle centerpoint formula: (Ax + Bx + Cx)/3
    center = mul(float4(center, 1.0f), ip[0].world); //Since we skip the transform in the vertex shader, do it here to make sure we get the center of individual triangles
    float dist = distance(center, cameraPosition);
	
	//Calculate tessellation based off of distance
    float tess = 1.0f;
    if (dist >= maxDist) //This is the point where we're sufficiently far away for us to only need to tessellate once
    {
        tess = minTess;
    }
    else
    {
        //tess = (maxTess * (maxDist - dist) / maxDist);
        tess = maxTess * saturate((maxDist - dist) / (maxDist - minDist));
    }
    
    output.EdgeTessFactor[0] = output.EdgeTessFactor[1] = output.EdgeTessFactor[2] = tess;
    output.InsideTessFactor = tess; //(output.EdgeTessFactor[0] + output.EdgeTessFactor[1] + output.EdgeTessFactor[2]) / 3.0f; (Wait why tf have I been doing (tess+tess+tess)/3?)
    
    return output;
}

struct HullShaderOutput
{
    float4 worldPosition : WORLD_POS;
	float2 uv : UV;
	float4 nor : NORMAL;
    
    matrix world : WORLD;
};

[domain("tri")]								//Tesselate as triangles
[partitioning("fractional_odd")]			//
[outputtopology("triangle_cw")]				//Clockwise triangles, match our topology
[outputcontrolpoints(NUM_CONTROL_POINTS)]	//
[patchconstantfunc("CalcHSPatchConstants")]	//Function we're passing in to define our patches
HullShaderOutput main( 
	InputPatch<VertexShaderOutput, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID) //Note that we're again ignoring the "uint PatchID : SV_PrimitiveID" in this function call
{
	HullShaderOutput output = (HullShaderOutput)0;

    output.worldPosition = ip[i].worldPosition;
	output.uv = ip[i].uv;
    output.nor = normalize(ip[i].nor);

    //Keep passing this on because it's also needed in the domain shader
    output.world = ip[i].world;
    
	return output;
}
