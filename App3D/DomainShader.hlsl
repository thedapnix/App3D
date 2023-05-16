//Pass camera information in through cbuffer
//cbuffer Camera : register(b0)
//{
//	matrix viewProjection;
//}


//Get our data from the output of the hullshader by mimicking structs here, as per usual
struct HullShaderOutput 
{
	float4 clipPosition : CLIP_POS;
	float2 uv : UV;
	float4 nor : NORMAL;
	float4 col : COLOR;

	float4 worldPosition : WORLD_POS;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]	: SV_TessFactor;
	float InsideTessFactor	: SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

//Hmm
struct DomainShaderOutput
{
	float4 clipPosition : SV_POSITION; //Return of the SV_POSITION semantic? Question is, is this clip space or no? Neither the base example or the recipe confirms this
	float2 uv : UV;
	float4 nor : NORMAL;
	float4 col : COLOR;

	float4 worldPosition : WORLD_POS;
};

[domain("tri")]
DomainShaderOutput main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 barycentric : SV_DomainLocation, //VS example called this variable "domain", but barycentric is way more descriptive, cheers
	const OutputPatch<HullShaderOutput, NUM_CONTROL_POINTS> patch)
{
	DomainShaderOutput output = (DomainShaderOutput)0;

	//Unbelievably cursed
	output.worldPosition = 
		patch[0].worldPosition * barycentric.x + 
		patch[1].worldPosition * barycentric.y + 
		patch[2].worldPosition * barycentric.z;

	output.uv = 
		patch[0].uv * barycentric.x + 
		patch[1].uv * barycentric.y + 
		patch[2].uv * barycentric.z;

	output.nor = normalize(
		patch[0].nor * barycentric.x +
		patch[1].nor * barycentric.y +
		patch[2].nor * barycentric.z);

	output.col = patch[0].col; //Praydge

	output.clipPosition = output.worldPosition; //mul(output.worldPosition, viewProjection) but just to ignore errors for now

	return output;
}

struct DS_OUTPUT
{
	float4 vPosition  : SV_POSITION;
};

struct HS_CONTROL_POINT_OUTPUT
{
	float3 vPosition : WORLDPOS; 
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	Output.vPosition = float4(
		patch[0].vPosition*domain.x+patch[1].vPosition*domain.y+patch[2].vPosition*domain.z,1);

	return Output;
}
