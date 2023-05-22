
struct PixelShaderInput
{
	float4 clipPos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : TEXCOORD1;
};

TextureCube reflectionTexture : register(t0);
sampler standardSampler : register(s0); //not SamplerState?

cbuffer CAMERA_CONSTANT_BUFFER : register(b0)
{
    matrix view;
    matrix proj;
    float3 cameraPos;
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    
    //Calculate vector that goes FROM the camera, TO the fragment being processed, in world space
    float3 incomingView = normalize(input.worldPos - cameraPos);
    
    //Use the HLSL reflect function to calculate how the incoming view vector is reflected, based on the normal of the fragment being processed
    float3 reflectedView = reflect(incomingView, input.normal);
    
    //Sample from reflectionTexture using the sampler and the reflected view vector
    float4 sampledValue = float4(reflectedView, 1.0f); //reflectionTexture.Sample(standardSampler, reflectedView);
    
    return sampledValue;
}