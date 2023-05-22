
cbuffer CAMERA_CONSTANT_BUFFER : register(b0)
{
    matrix view;
    matrix proj;
    float3 cameraPos;
}

cbuffer OBJECT_CONSTANT_BUFFER : register(b1)
{
    matrix scale;
    matrix rotate;
    matrix translate;
};

struct VertexShaderInput
{
    float3 position : POS;
    float3 normal : NOR;
};

struct VertexShaderOutput
{
    float4 clipPos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : TEXCOORD1;
};

VertexShaderOutput main( VertexShaderInput input ) //standard vertex shader
{
    VertexShaderOutput output = (VertexShaderOutput) 0;
    
    float4 pos = float4(input.position, 1.0f);
    
    pos = mul(pos, scale);
    pos = mul(pos, rotate);
    pos = mul(pos, translate);
    output.worldPos = pos;
    
    pos = mul(pos, view);
    pos = mul(pos, proj);
    output.clipPos = pos;
    
    float4 nor = float4(input.normal, 0.0f);
    nor = mul(nor, scale);
    nor = mul(nor, rotate);
    nor = mul(nor, translate);
    output.normal = normalize(nor);
    
    return output;
}