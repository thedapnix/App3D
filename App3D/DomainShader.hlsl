
cbuffer CAMERA_CONSTANT_BUFFER : register(b0)
{
    matrix view;
    matrix proj;
    float3 cameraPosition;
};

struct HullShaderOutput 
{
    float4 worldPosition : WORLD_POS;
	float2 uv : UV;
	float4 nor : NORMAL;
	
    matrix world : WORLD;
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
    float3 tangent : TEXCOORD3; //Perchance?
};


float3 ProjectPointOntoPlane(float3 toProject, float3 planePoint, float3 planeNor)
{
    //Formula for projection of point onto a plane	:	p' = p - (n dot (p - o)) * n
	return toProject - (dot(planeNor, toProject - planePoint) * planeNor);
}

[domain("tri")]
DomainShaderOutput main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 barycentric : SV_DomainLocation, //VS example called this variable "domain", but barycentric is way more descriptive, cheers
	const OutputPatch<HullShaderOutput, NUM_CONTROL_POINTS> patch)
{
	DomainShaderOutput output = (DomainShaderOutput)0;
	
	//Phong tessellation:
	//http://www.klayge.org/material/4_0/PhongTess/PhongTessellation.pdf
	//1. Compute the linear tessellation, then
	//2. project the resulting point orthogonally onto the three tangent planes defined by the triangle vertices, and finally
	//3. Compute the barycentric interpolation of these three projections
	
	//We've already computed the tessellation in our hull shader, so now we do orthogonal projection onto plane
    float3 worldPos =
		patch[0].worldPosition * barycentric.x +
		patch[1].worldPosition * barycentric.y +
		patch[2].worldPosition * barycentric.z;
	
    float3 b0 = ProjectPointOntoPlane(worldPos, patch[0].worldPosition.xyz, patch[0].nor.xyz);
    float3 b1 = ProjectPointOntoPlane(worldPos, patch[1].worldPosition.xyz, patch[1].nor.xyz);
    float3 b2 = ProjectPointOntoPlane(worldPos, patch[2].worldPosition.xyz, patch[2].nor.xyz);
	
    //float3 wp1 = mul(patch[0].worldPosition, patch[0].world);
    //float3 wp2 = mul(patch[1].worldPosition, patch[0].world);
    //float3 wp3 = mul(patch[2].worldPosition, patch[0].world);
	
    //float3 b0 = ProjectPointOntoPlane(worldPos, wp1, patch[0].nor.xyz);
    //float3 b1 = ProjectPointOntoPlane(worldPos, wp2, patch[1].nor.xyz);
    //float3 b2 = ProjectPointOntoPlane(worldPos, wp3, patch[2].nor.xyz);
	
	//And barycentric(again), + interpolation
    float3 baryPos =
		b0 * barycentric.x +
		b1 * barycentric.y +
		b2 * barycentric.z;
	
    float4 pos = float4(worldPos, 1.0f);
    pos = mul(pos, patch[0].world);
    //output.worldPosition = float4(lerp(worldPos, baryPos, (3.0f / 4.0f)), 1.0f); //"..a shape factor A can be used to interpolate between linear and phong tessellation" and "In our experiments, we fix A = 3/4 globally"
	
	//For now, the linear interpolation between coordinates doesn't really work with instancing
	//Might have to ask for help, thing to keep in note is that we need float4's with a w of 1.0f, so we can multiply properly with the instanced world matrix
    output.worldPosition = pos; 
	
	output.uv = 
		patch[0].uv * barycentric.x + 
		patch[1].uv * barycentric.y + 
		patch[2].uv * barycentric.z;
	
    output.nor = normalize(
		patch[0].nor * barycentric.x +
		patch[1].nor * barycentric.y +
		patch[2].nor * barycentric.z);

	output.col = float4(1.0f, 0.0f, 0.0f, 1.0f); //Pretty sure this is redundant, old code from when I wasn't using textures and I colored objects manually on the GPU

    //View and proj
    pos = mul(pos, view);
    pos = mul(pos, proj);
    output.clipPosition = pos;
	
	//Tangent calculations, making use of the fact that we have access to triangles here
    float3 pos1 = patch[0].worldPosition.xyz;
    float3 pos2 = patch[1].worldPosition.xyz;
    float3 pos3 = patch[2].worldPosition.xyz;
	
    float3 uv1 = float3(patch[0].uv, 0.0f);
    float3 uv2 = float3(patch[1].uv, 0.0f);
    float3 uv3 = float3(patch[2].uv, 0.0f);
	
    float3 edge1 = pos2 - pos1;
    float3 edge2 = pos3 - pos1;
    float3 deltaUv1 = uv2 - uv1;
    float3 deltaUv2 = uv3 - uv1;
	
    float frac = 1.0f / (deltaUv1.x * deltaUv2.y - deltaUv2.x * deltaUv1.y);
	
    float3 tangent = float3(
		frac * (deltaUv2.y * edge1.x - deltaUv1.y * edge2.x),
		frac * (deltaUv2.y * edge1.y - deltaUv1.y * edge2.y),
		frac * (deltaUv2.y * edge1.z - deltaUv1.y * edge2.z)
	);
	
    output.tangent = tangent;
	
	return output;
}