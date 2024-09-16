
TextureCube reflectionTexture : register(t0);
SamplerState standardSampler : register(s0);

cbuffer CAMERA_CONSTANT_BUFFER : register(b0)
{
    matrix view;
    matrix proj;
    float3 cameraPos;
};

struct PixelShaderInput
{
    float4 clipPos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float2 uv : TEXCOORD1;
    float3 normal : TEXCOORD2;
};

float4 main(PixelShaderInput input) : SV_TARGET
{   
    //float3 normal = normalize(input.normal);
    
    //Calculate vector that goes FROM the camera, TO the fragment being processed, in world space
    float3 incomingView = normalize(input.worldPos - cameraPos);
    
    //Use the HLSL reflect function to calculate how the incoming view vector is reflected, based on the normal of the fragment being processed
    float3 reflectedView = reflect(incomingView, input.normal);
    
    //Sample from reflectionTexture using the sampler and the reflected view vector
    float4 sampledValue = reflectionTexture.Sample(standardSampler, reflectedView);
    
    //Set an arbitrary value that we can access in the compute shader to be able to modify specifically this object
    //IMPORTANT: the w-components of my gBuffers are either normals (ranges between -1 to +1) or the shininess value from .mtl files (typically I have these set to 1.0f)
        //Because of thishis, you HAVE TO MAKE SURE that this value isn't one of those
        //Otherwise the dispatch-call on the compute shader will overwrite anything else with a w-component with the same value (Resulting in only this object being rendered)
    sampledValue.w = 69.0f;
    
    return sampledValue;
}