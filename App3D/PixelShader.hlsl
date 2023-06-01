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
    float3 ambient = 0.25f;
    float4 base = tex2D.Sample(samplerState, input.uv);
    float3 result = 0.0f;
    float3 shininess = 32.0f;      //Hardcoding these because uhh
    float specularPower = 0.5f; //Pretty sure this would need me to rewrite hundreds of lines to do materials with different levels of specularity
    
    //Allow for multiple spotlights
    //https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/sm5-object-structuredbuffer-getdimensions
    uint count;
    uint stride; //I don't really care about the stride but I need to pass in two values to the GetDimensions() function in order to get the element count
    spotlights.GetDimensions(count, stride);
    for (int i = 0; i < count; i++)
    {
        //Page 504 of the book "Practical Rendering and Computation with Direct3D11"
        //Calculate the diffuse term
        float3 L = 0;
        float attenuation = 1.0f;
        L = spotlights[i].origin - input.clipPosition.xyz; //Base the light vector on the light position
        
        //Calculate attenuation based on distance from the light source
        float dist = length(L);
        attenuation = max(0.0f, 1.0f - (dist / 20.0f)); //What I write as "20.0f" here is what the book refers to as LightRange.x, so some arbitrary value representing how far the light reaches
        L /= dist;
        
        //Also add in the spotlight attenuation factor (rotX and rotY in spotlight class?)
        //float3 L2 = spotlights[i].direction;
        //float rho = dot(-L, L2);
        //attenuation *= saturate(
        //(rho - spotlights[i].angleY) /
        //(spotlights[i].angleX - spotlights[i].angleY));

        float nDotL = saturate(dot(input.nor.xyz, L));
        float3 diffuse = nDotL * spotlights[i].colour * base.xyz;
        
        //Calculate the specular term
        float3 V = float3(0.0f, 0.0f, -5.0f); //Camera position, pass in cb
        float3 H = normalize(L + V);
        float3 specular = pow(saturate(dot(input.nor.xyz, H)), specularPower) * spotlights[i].colour * shininess * nDotL;
        
        result += (diffuse + specular) * attenuation;
    }
    result += ambient; //Apply ambient lighting to the scene
    
    return float4(result, 1.0f);
}