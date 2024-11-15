
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
	
	//AAAAAAAAAAAA
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
    //https://stackoverflow.com/questions/9605556/how-to-project-a-point-onto-a-plane-in-3d giving the formula: p' = p - (n dot (p - o)) * n , big ups
	return toProject - (dot(planeNor, toProject - planePoint) * planeNor);
}

[domain("tri")]
DomainShaderOutput main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 barycentric : SV_DomainLocation,
	const OutputPatch<HullShaderOutput, NUM_CONTROL_POINTS> patch)
{
    DomainShaderOutput output = (DomainShaderOutput) 0;

    float3 pos =
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
	
	//Transform the pos using the instance stuff
    float4 clipPos = float4(pos, 1.0f);
    clipPos = mul(clipPos, patch[0].world); //Keep if instanced, comment out if non-instanced
    output.worldPosition = clipPos;
    clipPos = mul(clipPos, view);
    clipPos = mul(clipPos, proj);
    output.clipPosition = clipPos;
	
	//Tangent calculations
    float3 pos1 = patch[0].worldPosition.xyz;
    float3 pos2 = patch[1].worldPosition.xyz;
    float3 pos3 = patch[2].worldPosition.xyz;
	
    pos1 = mul(pos1, (float3x3) patch[0].world);
    pos2 = mul(pos2, (float3x3) patch[1].world);
    pos3 = mul(pos3, (float3x3) patch[2].world);
	
    float3 uv1 = float3(patch[0].uv, 0.0f);
    float3 uv2 = float3(patch[1].uv, 0.0f);
    float3 uv3 = float3(patch[2].uv, 0.0f);
	
    //uv1 = mul(uv1, (float3x3)patch[0].world);
    //uv1 = mul(uv2, (float3x3)patch[1].world);
    //uv1 = mul(uv3, (float3x3)patch[2].world);
	
    float3 edge1 = pos2 - pos1;
    float3 edge2 = pos3 - pos1;
    float3 deltaUv1 = uv2 - uv1;
    float3 deltaUv2 = uv3 - uv1;
	
    //edge1 = mul(edge1, (float3x3) patch[0].world);
	
    float frac = 1.0f / (deltaUv1.x * deltaUv2.y - deltaUv2.x * deltaUv1.y);
	
	//Hermans, but this works the same way mine does
    float2x2 inverseDUV = { deltaUv2.y, -deltaUv1.y, -deltaUv2.x, deltaUv1.x };
    float2x3 directions = { edge1, edge2 };
    float2x3 TB = mul(inverseDUV, directions);
    float3 tangent = TB[0];
	
 //   float3 tangent = float3(
	//	frac * (deltaUv2.y * edge1.x - deltaUv1.y * edge2.x),
	//	frac * (deltaUv2.y * edge1.y - deltaUv1.y * edge2.y),
	//	frac * (deltaUv2.y * edge1.z - deltaUv1.y * edge2.z)
	//);
	
    //tangent = mul(tangent, (float3x3)patch[0].world);
	
    output.tangent = tangent;
	
    return output;
}

/*
[domain("tri")]
DomainShaderOutput main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 barycentric : SV_DomainLocation, //VS example called this variable "domain", but barycentric is way more descriptive, cheers
	const OutputPatch<HullShaderOutput, NUM_CONTROL_POINTS> patch)
{
	DomainShaderOutput output = (DomainShaderOutput)0;
	
	//Oh god it's getting even worse
	//http://www.klayge.org/material/4_0/PhongTess/PhongTessellation.pdf
	//1. Compute the linear tessellation, then
	//2. project the resulting point orthogonally onto the three tangent planes defined by the triangle vertices, and finally
	//3. Compute the barycentric interpolation of these three projections
	//So we've already computed the tessellation in our hull shader. So now we do orthogonal projection onto plane (straight-up take triangle positions x, y, and z to make a plane)
    float3 worldPos =
		patch[0].worldPosition * barycentric.x +
		patch[1].worldPosition * barycentric.y +
		patch[2].worldPosition * barycentric.z;
	
	//Formula for orthogonal projection of point onto plane: p' = p - (n . p + d) * n , courtesy of google
    float3 b0 = ProjectPointOntoPlane(worldPos, patch[0].worldPosition.xyz, patch[0].nor.xyz);
    float3 b1 = ProjectPointOntoPlane(worldPos, patch[1].worldPosition.xyz, patch[1].nor.xyz);
    float3 b2 = ProjectPointOntoPlane(worldPos, patch[2].worldPosition.xyz, patch[2].nor.xyz);
	
    //worldPos = mul(worldPos, (float3x3) patch[0].world);
	
	//And barycentric(again), + interpolation
    float3 baryPos =
		b0 * barycentric.x +
		b1 * barycentric.y +
		b2 * barycentric.z;
	
    float4 pos = float4(worldPos, 1.0f);
    pos = mul(pos, patch[0].world);
    output.worldPosition = float4(lerp(worldPos, baryPos, (3.0f / 4.0f)), 1.0f); //"..a shape factor A can be used to interpolate between linear and phong tessellation" and "In our experiments, we fix A = 3/4 globally"
    //output.worldPosition = float4(lerp(pos.xyz, baryPos, (3.0f / 4.0f)), 1.0f);
	//float4 posW = float4(lerp(worldPos, baryPos, (3.0f / 4.0f)), 1.0f);
    //output.worldPosition = mul(posW, patch[0].world); //World transform on positions delayed from vertex shader until now
	
	output.uv = 
		patch[0].uv * barycentric.x + 
		patch[1].uv * barycentric.y + 
		patch[2].uv * barycentric.z;

	//World transform on normals delayed from vertex shader until now
 //   float4 nor =
	//patch[0].nor * barycentric.x +
	//patch[1].nor * barycentric.y +
	//patch[2].nor * barycentric.z;
	
 //   nor = mul(nor, patch[0].world);
 //   output.nor = normalize(nor);
	
    output.nor = normalize(
		patch[0].nor * barycentric.x +
		patch[1].nor * barycentric.y +
		patch[2].nor * barycentric.z);

	output.col = float4(1.0f, 0.0f, 0.0f, 1.0f); //ahaaa praydge

    //float4 pos = mul(output.worldPosition, view);
    //float4 pos = mul(output.worldPosition, patch[0].world);
    //pos = mul(pos, view);
    //pos = mul(pos, proj);
	
    //output.worldPosition = pos;
    pos = mul(pos, view);
    pos = mul(pos, proj);
    output.clipPosition = pos;
	
	//Is this a good place to do tangent calculations? I have access to triangles after all?
    float3 pos1 = patch[0].worldPosition.xyz;
    float3 pos2 = patch[1].worldPosition.xyz;
    float3 pos3 = patch[2].worldPosition.xyz;
	
    pos1 = mul(pos1, (float3x3)patch[0].world);
    pos2 = mul(pos2, (float3x3)patch[1].world);
    pos3 = mul(pos3, (float3x3)patch[2].world);
	
    //pos1 = mul(pos1, barycentric.x);
    //pos1 = mul(pos1, barycentric.y);
    //pos1 = mul(pos1, barycentric.z);
	
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
	
    tangent = mul(tangent, (float3x3)patch[0].world);
	
    output.tangent = tangent;
	
	return output;
}
*/