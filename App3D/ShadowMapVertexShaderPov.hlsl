
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
    float3 cameraPosition;
};

struct VertexShaderInput
{
    float3 localPosition : POS;
    float2 uv : UV;
    float3 localNormal : NOR;
};

float4 main(VertexShaderInput input) : SV_POSITION
{
    float4 pos = float4(input.localPosition, 1.0f);
    
    pos = mul(pos, scale);
    pos = mul(pos, rotate);
    pos = mul(pos, translate);
    
    //pos = mul(pos, view);
    pos = mul(pos, proj);
    
    return pos;
}