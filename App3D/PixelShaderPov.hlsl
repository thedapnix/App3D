
cbuffer CAMERA_CONSTANT_BUFFER : register(b0)
{
    matrix view;
    matrix proj;
    float3 cameraPosition;
};

cbuffer SHININESS_CONSTANT_BUFFER : register(b1)
{
    float shininess;
};

Texture2D ambientTexture : register(t0);
Texture2D diffuseTexture : register(t1);
Texture2D specularTexture : register(t2);
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
    
    float fov;
};

StructuredBuffer<SpotLight> spotlights : register(t3);

Texture2DArray<float> shadowMaps : register(t4);
sampler shadowMapSampler : register(s1);


float4 main(PixelShaderInput input) : SV_TARGET
{
    float3 ambient = 0.25f;
    float4 base = diffuseTexture.Sample(samplerState, input.uv);
    float3 specularAlbedo = specularTexture.Sample(samplerState, input.uv).xyz;
    float specularPower = shininess;
    
    float3 finalColor = base.xyz * ambient;
    float3 lighting = 0.0f;
    
    //Allow for multiple spotlights
    //https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/sm5-object-structuredbuffer-getdimensions
    uint count;
    uint stride; //I don't really care about the stride but I need to pass in two values to the GetDimensions() function in order to get the element count
    spotlights.GetDimensions(count, stride);
    for (int i = 0; i < count; i++)
    {
        //Shadow stuff
        //So the cookbook gives us the formula: float3 shadowMapUV = float3(ndcSpace.x * 0.5f + 0.5f, ndcSpace.y * -0.5f + 0.5f, lightIndex);
        //To get coordinates in ndc space, we do the perspective divide (w-component) on the transformed position
        bool isInShadow = false;
        
        float4 ndcPos = mul(input.worldPosition, spotlights[i].view);
        ndcPos = mul(ndcPos, spotlights[i].proj);
        ndcPos.xyz /= ndcPos.w;
        
        float3 shadowMapUV = float3(ndcPos.x * 0.5f + 0.5f, ndcPos.y * -0.5f + 0.5f, i); //So this should be correct yes?
        float3 shadowMapSample = shadowMaps.SampleLevel(shadowMapSampler, shadowMapUV, 0.0f); //MSDN: First argument is the sampler state, second is the texture coordinates, third is lod (if the value is = 0, the biggest mipmap is used)
        
        //"If the sampled value is lesser than the calculated value then this means that some other object lies in the path, and thus the fragment should be considered to be in shadow"
        //"As several of the fragments we are currently viewing might share a texel in the shadow map (as it does not have infinite precision) 
        //you may want to add a very small value to the sampled depth to try and avoid self shadowing issues"
        if (shadowMapSample.x + 0.00001f < ndcPos.z)
        {
            isInShadow = true;
        }
        
        //Page 504 of the book "Practical Rendering and Computation with Direct3D11"
        //Calculate the diffuse term
        float3 L = 0;
        float attenuation = 1.0f;
        
        if (spotlights[i].fov != 0.0f) //If it doesn't have an fov, I'll treat it as a directional light
        {
            L = spotlights[i].origin - input.worldPosition.xyz; //Base the light vector on the light position
            //Calculate attenuation based on distance from the light source
            float dist = length(L);
            attenuation = max(0.0f, 1.0f - (dist / 40.0f)); //What I write as "40.0f" here is what the book refers to as LightRange.x, so some arbitrary value representing how far the light reaches
            L /= dist;
            
            //Modify attenuation by making the light weaker the further we get away from the center
            float outerCone = cos(spotlights[i].fov / 2.0f);
            float innerCone = cos(spotlights[i].fov / 4.0f);
            float alpha = dot(-L, spotlights[i].direction);
            attenuation *= smoothstep(outerCone, innerCone, alpha);
        }
        else
        {
            L = -spotlights[i].direction;
        }

        float nDotL = saturate(dot(input.nor.xyz, L));
        float3 diffuse = nDotL * spotlights[i].colour * base.xyz;
        
        //Calculate the specular term
        float3 V = cameraPosition - input.worldPosition.xyz;
        float3 H = normalize(L + V);
        float3 specular = pow(dot(input.nor.xyz, H), shininess) * spotlights[i].colour * specularAlbedo * nDotL; //removed: saturate
        
        lighting += (diffuse + specular) * attenuation;
        //if (!isInShadow)
        //{
        //    lighting += (diffuse + specular) * attenuation;
        //}
    }
    finalColor += lighting;
    
    return float4(finalColor, 1.0f);
}