//Texture2D tex2D : register(t0);
//SamplerState samplerState : register(s0);

struct PixelShaderInput
{
    float4 clipPosition : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 nor : NORMAL;
    float4 col : TEXCOORD1;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    //float3 tex = tex2D.Sample(samplerState, input.uv).rgb;
    //return (tex, 1.0f);
    return float4(input.uv, 1.0f, 1.0f);  //input.col;
}