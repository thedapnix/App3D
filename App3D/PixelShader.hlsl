Texture2D tex2D : register(t0);
SamplerState samplerState : register(s0);

struct PixelShaderInput
{
    float4 clipPosition : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 nor : NORMAL;
    float4 col : TEXCOORD1;
    
    float4 worldPosition : TEXCOORD2;
};

struct SpotLight
{
    matrix view;
    matrix proj;
    float3 colour;
    float3 origin;
    float3 direction;
};

StructuredBuffer<SpotLight> spotlights : register(t1);

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 ambient = float4(0.25f, 0.25f, 0.25f, 0.25f);
    
    //Page 504 of the book "Practical Rendering and Computation with Direct3D11", 
    
    float4 base = tex2D.Sample(samplerState, input.uv);
    float4 result = base * ambient;
    return result;
}