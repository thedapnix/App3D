
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    matrix world;
    
    bool hasNormalMap;
};

struct VertexShaderInput
{
    float3 localPosition : POS;
    float2 uv : UV;
    float3 localNormal : NOR;
};

struct VertexShaderOutput
{
    float4 worldPosition : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 nor : NORMAL;
};

VertexShaderOutput main(VertexShaderInput input)
{   
    VertexShaderOutput output = (VertexShaderOutput)0;
    
    float4 pos = float4(input.localPosition, 1.0f);
    float4 nor = float4(input.localNormal, 0.0f);
    
    pos = mul(pos, world);
    nor = mul(nor, world);

    output.worldPosition = pos;
    output.uv = input.uv;
    output.nor = normalize(nor);
    
    return output;
}