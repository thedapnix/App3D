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

//Deferred renderer requires pixel shader to output more than just a color
struct PixelShaderOutput
{
    float4 position : SV_Target0;
    float4 colour : SV_Target1; //Rename to diffuse instead of colour?
    float4 normal : SV_Target2;
};

PixelShaderOutput main(PixelShaderInput input) //: SV_TARGET //previously float4 main
{
    //float3 tex = tex2D.Sample(samplerState, input.uv).rgb;
    //return (tex, 1.0f);
    PixelShaderOutput output;
    output.position = input.worldPosition;
    output.colour = tex2D.Sample(samplerState, input.uv); //input.col; //sample from texture here if you want
    output.normal = input.nor;
    
    return output;
}