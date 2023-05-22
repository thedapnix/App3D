
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    matrix scale;
    matrix rotate;
    matrix translate;
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
    float3 normal : TEXCOORD1;
};

VertexShaderOutput main( VertexShaderInput input ) //standard vertex shader, actually thinking i don't need this new vertex shader at all.
{
    VertexShaderOutput output = (VertexShaderOutput) 0;
    
    float4 pos = float4(input.localPosition, 1.0f);
    
    pos = mul(pos, scale);
    pos = mul(pos, rotate);
    pos = mul(pos, translate);
    output.worldPos = pos;
    
    pos = mul(pos, view);
    pos = mul(pos, proj);
    output.clipPos = pos;
    
    float4 nor = float4(input.localNormal, 0.0f);
    nor = mul(nor, scale);
    nor = mul(nor, rotate);
    nor = mul(nor, translate);
    output.normal = normalize(nor);
    
    return output;
}