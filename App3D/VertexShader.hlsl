
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    matrix world;
    matrix invWorld;
    
    bool hasNormalMap;
};

struct VertexShaderInput
{
    float3 localPosition : POS;
    float2 uv : UV;
    float3 localNormal : NOR;
    
    //We're now going to take in every world matrix through here (Input Layout) instead of the per-object constant buffer
    //row_major float4x4 world : WORLD;
    matrix world : WORLD;
    uint InstanceId : SV_InstanceID;
};

struct VertexShaderOutput
{
    float4 worldPosition : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 nor : NORMAL;
    
    //New: Instancing stuff
    matrix world : WORLD;
};

VertexShaderOutput main(VertexShaderInput input)
{   
    VertexShaderOutput output = (VertexShaderOutput)0;
    
    float4 pos = float4(input.localPosition, 1.0f);
    float4 nor = float4(input.localNormal, 0.0f);
    
    //pos = mul(pos, world);
    //nor = mul(nor, world); //invWorld?
    
    //New: Multiply by the per-instance world matrix from input layout
    //pos = mul(pos, input.world);
    nor = mul(pos, input.world);

    output.worldPosition = pos;
    output.uv = input.uv;
    output.nor = normalize(nor);
    
    //Forgor
    output.world = input.world;
    
    return output;
}