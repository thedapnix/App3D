
cbuffer CAMERA_CONSTANT_BUFFER : register(b0)
{
    matrix view;
    matrix proj;
    float3 cameraPosition;
};

struct VertexShaderInput
{
    float3 localPosition : POS;
    float2 uv : UV;
    float3 localNormal : NOR;
    
    //Per-instance data
    matrix world : WORLD;
    uint InstanceId : SV_InstanceID;
};

float4 main(VertexShaderInput input) : SV_POSITION
{
    float4 pos = float4(input.localPosition, 1.0f);
    
    pos = mul(pos, input.world);
    
    pos = mul(pos, view);
    pos = mul(pos, proj);
    
    return pos;
}