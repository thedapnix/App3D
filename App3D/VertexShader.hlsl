
struct VertexShaderInput
{
    float3 localPosition : POS;
    float2 uv : UV;
    float3 localNormal : NOR;
    
    //Per-instance data
    matrix world : WORLD;
    uint InstanceId : SV_InstanceID;
};

struct VertexShaderOutput
{
    float4 worldPosition : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 nor : NORMAL;
    
    //Pass this along, since we can't perform world transformations on positions already (Tessellation needs to happen before we transform, or things get wacky)
    matrix world : WORLD;
};

//Instanced Version
VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output = (VertexShaderOutput) 0;
    
    float4 pos = float4(input.localPosition, 1.0f);
    float4 nor = float4(input.localNormal, 0.0f);
    
    //Postpone position transformations, only do normals for now
    nor = mul(nor, input.world);

    output.worldPosition = pos;
    output.uv = input.uv;
    output.nor = normalize(nor);
    
    output.world = input.world;
    
    return output;
}