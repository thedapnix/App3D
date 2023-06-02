
cbuffer CAMERA_CONSTANT_BUFFER : register(b0)
{
    matrix view;
    matrix proj;
    float3 cameraPosition;
};

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
    float2 rotation;
    
    float angle; //hlsl pain (functions as padding)
};

StructuredBuffer<SpotLight> spotlights : register(t1);

float4 main(PixelShaderInput input) : SV_TARGET
{
    float3 ambient = 0.1f;
    float4 base = tex2D.Sample(samplerState, input.uv);
    float3 specularAlbedo = 0.5f;  //Hardcoding these because uhh
    float specularPower = 0.5f;     //Pretty sure this would need me to rewrite hundreds of lines to do materials with different levels of specularity
    
    float3 lighting = 0.0f;
    float3 specular = 0.0f;
    
    //Allow for multiple spotlights
    //https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/sm5-object-structuredbuffer-getdimensions
    uint count;
    uint stride; //I don't really care about the stride but I need to pass in two values to the GetDimensions() function in order to get the element count
    spotlights.GetDimensions(count, stride);
    for (int i = 0; i < 1; i++)
    {
        //Page 504 of the book "Practical Rendering and Computation with Direct3D11"
        //Calculate the diffuse term
        float3 L = 0;
        float attenuation = 1.0f;
        L = spotlights[i].origin - input.worldPosition.xyz; //Base the light vector on the light position
        
        //Calculate attenuation based on distance from the light source
        float dist = length(L);
        attenuation = max(0.0f, 1.0f - (dist / 10.0f)); //What I write as "20.0f" here is what the book refers to as LightRange.x, so some arbitrary value representing how far the light reaches
        L /= dist;
        
        //Also add in the spotlight attenuation factor
        float3 L2 = spotlights[i].direction;
        float rho = dot(-L, L2);
        float rotXY = spotlights[i].rotation.x - spotlights[i].rotation.y;
        if(rotXY == 0.0f) rotXY = 1.0f; //Please don't divide by 0
        attenuation *= saturate(
        (rho - spotlights[i].rotation.y) /
        rotXY);

        float nDotL = saturate(dot(input.nor.xyz, L));
        float3 diffuse = nDotL * spotlights[i].colour;
        
        //Calculate the specular term
        float3 V = cameraPosition - input.worldPosition.xyz;
        float3 H = normalize(L + V);
        specular = pow(saturate(dot(input.nor.xyz, H)), specularPower) * spotlights[i].colour * specularAlbedo * nDotL; //previously float3 specular, now gets defined higher up so we can access outside of scope
        
        //lighting += (diffuse + specular) * attenuation;
        lighting = (ambient + diffuse) * base.xyz;
    }
    //result += ambient; //Apply ambient lighting to the scene
    //float3 finalColor = (lighting * base.xyz) + specular;
    float3 finalColor = base.xyz;
    finalColor += specular;
    
    return float4(finalColor, 1.0f);
    //return float4(result, 1.0f);
}