
cbuffer CAMERA_CONSTANT_BUFFER : register(b0)
{
    matrix view;
    matrix proj;
    float3 cameraPosition;
};

struct VertexShaderOutput //Mimic the VertexShaderOutput from our vertex shader, but with different semantics (only the CLIP_POS semantic is apparently important, the others mimic)
{
    //float4 clipPosition : SV_POSITION;
    float4 worldPosition : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 nor : NORMAL;
    //float4 col : TEXCOORD1;
};

//So firstly we want to make what's known as a "patch constant function", which has the purpose of determining the tessellation factors for the patch we're currently processing
struct HS_CONSTANT_DATA_OUTPUT //Triangle patches are defined by their 3 edges and their insides
{
    float EdgeTessFactor[3] : SV_TessFactor;
    float InsideTessFactor	: SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3 //Better than hardcoding, since we use this in both our function calls that have the InputPatch

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VertexShaderOutput, NUM_CONTROL_POINTS> ip)
{
	//Recipe example differs from VS example in that it doesn't have "uint PatchID : SV_PrimitiveID" in the function call, and that the output values get set to 32 rather than 15
	//I assume that this is fine, since PatchID isn't used anyway (could be in the future), and the output values probably determine some sort of min- or max-lod
    HS_CONSTANT_DATA_OUTPUT output = (HS_CONSTANT_DATA_OUTPUT) 0;

	//Constants
    float maxDist = 20.0f;
    float minDist = 2.0f;
    float maxTess = 16.0f;
    float minTess = 1.0f;
    
    //Store triangle centerpoint as well as distance from camera
    float3 center = (ip[0].worldPosition + ip[1].worldPosition + ip[2].worldPosition) / 3.0f; //Triangle centerpoint formula: (Ax + Bx + Cx)/3
    float dist = distance(center, cameraPosition);
	
	//Calculate tessellation
    float tess;
    if (dist >= maxDist) //This is the point where we're sufficiently far away for us to only need to tessellate once
    {
        tess = minTess;
    }
    else
    {
        tess = (maxTess * (maxDist - dist) / maxDist);
    }
    
    output.EdgeTessFactor[0] = output.EdgeTessFactor[1] = output.EdgeTessFactor[2] = tess;
    output.InsideTessFactor = (output.EdgeTessFactor[0] + output.EdgeTessFactor[1] + output.EdgeTessFactor[2]) / 3.0f;
    
    return output;
}

//Now on to what this shader will actually output to the tesselation stage, this will run per control point in the input patch
struct HullShaderOutput
{
	//As per routine when we want shaders to interact with eachother, mimic the structs passing data
    float4 worldPosition : WORLD_POS;
	float2 uv : UV;
	float4 nor : NORMAL;
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
	HullShaderOutput output = (HullShaderOutput)0; //Aaaand zero the memory

	//Basically just copy over the values from out vertex shader to the hull shader
    output.worldPosition = ip[i].worldPosition;
	output.uv = ip[i].uv;
	output.nor = ip[i].nor;

	return output;
}
