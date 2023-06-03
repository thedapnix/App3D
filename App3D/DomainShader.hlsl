
cbuffer CAMERA_CONSTANT_BUFFER : register(b0)
{
    matrix view;
    matrix proj;
    float3 cameraPosition;
};

//Get our data from the output of the hullshader by mimicking structs here, as per usual
struct HullShaderOutput 
{
    float4 worldPosition : WORLD_POS;
	float2 uv : UV;
	float4 nor : NORMAL;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]	: SV_TessFactor;
	float InsideTessFactor	: SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

struct DomainShaderOutput
{
	float4 clipPosition : SV_POSITION; //Return of the SV_POSITION semantic? Question is, is this clip space or no? Neither the base example or the recipe confirms this
	float2 uv : TEXCOORD0;
	float4 nor : NORMAL;
	float4 col : TEXCOORD1;

	float4 worldPosition : TEXCOORD2;
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

	output.col = float4(1.0f, 0.0f, 0.0f, 1.0f); //ahaaa praydge

    float4 pos = mul(output.worldPosition, view);
    pos = mul(pos, proj);
    output.clipPosition = pos;

	return output;
}