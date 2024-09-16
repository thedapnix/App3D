
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    matrix world;
};

cbuffer CAMERA_CONSTANT_BUFFER : register(b1)
{
    matrix view;
    matrix proj;
    float3 cameraPos;
}

struct VertexShaderInput
{
    float3 localPosition : POS;
    float2 uv : UV;
    float3 localNormal : NOR;
};

struct VertexShaderOutput
{
    float4 clipPos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float2 uv : TEXCOORD1;
    float3 normal : TEXCOORD2;
};

VertexShaderOutput main( VertexShaderInput input ) //I'm not even sure that this vertex shader is needed, as it's identical to the regular one
{
    VertexShaderOutput output = (VertexShaderOutput) 0;
    
    float4 pos = float4(input.localPosition, 1.0f);
    
    pos = mul(pos, world);
    output.worldPos = pos;
    
    pos = mul(pos, view);
    pos = mul(pos, proj);
    output.clipPos = pos;
    
    output.uv = input.uv;
    
    float4 nor = float4(input.localNormal, 0.0f);
    nor = mul(nor, world); 
    output.normal = normalize(nor);
    
    return output;
}