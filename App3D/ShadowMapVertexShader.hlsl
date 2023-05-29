
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    //matrix world;
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

struct VertexShaderOutput
{
    //float4 clipPosition : SV_POSITION;
    float4 worldPosition : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 nor : NORMAL;
    //float4 col : TEXCOORD1;
};

//struct VertexShaderOutput
//{
//    float4 worldPosition : SV_POSITION;
//};

float4 main(VertexShaderInput input) : SV_POSITION
{
    //VertexShaderOutput output;
    float4 pos = float4(input.localPosition, 1.0f);
    
    pos = mul(pos, scale);
    pos = mul(pos, rotate);
    pos = mul(pos, translate);
    
    pos = mul(pos, view);
    pos = mul(pos, proj);
    
    //output.worldPosition = float4(input.localPosition, 1.0f);
    return pos;
}