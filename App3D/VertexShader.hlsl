
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    matrix world;
    matrix invWorld;
    
    bool hasNormalMap;
    float3 aTangent;
    float3 aBitangent;
    float3 bTangent;
    float3 bBitangent;
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
    
    //New: Calculate TBN vectors while we still have access to the objects local space positions, BIIISMILLAH
    /*if(hasNormalMap)
    {
        //For tomorrow: Pay attention to page 502 of the frank de luna book, which goes through an example of how to do the ridiculous calculations we did on the CPU side but much smarter and better on the GPU side
        float4 T = normalize(mul(float4(aTangent, 0.0f), world));
        float4 B = normalize(mul(float4(aBitangent, 0.0f), world));
        float4 N = normalize(mul(float4(input.localNormal, 0.0f), world));
    
        output.tbn = float3x3(T.xyz, B.xyz, N.xyz);
    }*/
    
    pos = mul(pos, world);
    nor = mul(nor, world);

    output.worldPosition = pos;
    output.uv = input.uv;
    output.nor = normalize(nor);
    
    return output;
}