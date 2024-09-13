
RWTexture2D<unorm float4> backBufferUAV : register(u0);

cbuffer CAMERA_CONSTANT_BUFFER : register(b0)
{
    matrix view;
    matrix proj;
    float3 cameraPosition;
};

Texture2D<float4> positionGBuffer : register(t0);
Texture2D<float4> ambientGBuffer : register(t1);
Texture2D<float4> diffuseGBuffer : register(t2);
Texture2D<float4> specularGBuffer : register(t3);

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

StructuredBuffer<SpotLight> spotlights : register(t4);
Texture2DArray<float> shadowMaps : register(t5);
sampler shadowMapSampler : register(s0);

[numthreads(8, 8, 1)]                       //Think of a grid of cells reminiscent of threads in OS programming, where every thread runs the code below
                                            //In this case, I make a 8x8 grid, making sure to cover the entire view (by dispatching 100x100)
void main(uint3 DTid : SV_DispatchThreadID) //So the DTid is the index of the specific thread we're working with
{    
    float4 position = float4(positionGBuffer[DTid.xy].xyz, 1.0f);
    float3 ambientAlbedo = ambientGBuffer[DTid.xy].xyz;
    float3 diffuseAlbedo = diffuseGBuffer[DTid.xy].xyz;
    float3 specularAlbedo = specularGBuffer[DTid.xy].xyz;
    float3 normal = normalize(float3(ambientGBuffer[DTid.xy].w, diffuseGBuffer[DTid.xy].w, specularGBuffer[DTid.xy].w));
    float shininess = positionGBuffer[DTid.xy].w;
    
    float3 finalColour = 0.0f;
    
    uint count;
    uint stride;
    spotlights.GetDimensions(count, stride);
    for (int i = 0; i < count; i++)
    {
        bool isInShadow = false;
        float4 ndcPos = mul(position, spotlights[i].view);
        ndcPos = mul(ndcPos, spotlights[i].proj);
        ndcPos.xyz /= ndcPos.w;
        
        float3 shadowMapUV = float3(ndcPos.x * 0.5f + 0.5f, ndcPos.y * -0.5f + 0.5f, i);
        float3 shadowMapSample = shadowMaps.SampleLevel(shadowMapSampler, shadowMapUV, 0.0f);
        
        if (shadowMapSample.x + 0.00001f < ndcPos.z)
        {
            isInShadow = true;
        }
        
        float3 L = 0;
        float attenuation = 1.0f;
        
        if (spotlights[i].fov != 0.0f)
        {
            L = spotlights[i].origin - position.xyz;
            float dist = length(L);
            attenuation = max(0.0f, 1.0f - (dist / 40.0f));
            L /= dist;
            
            float outerCone = cos(spotlights[i].fov / 2.0f);
            float innerCone = cos(spotlights[i].fov / 4.0f);
            float alpha = dot(-L, spotlights[i].direction);
            attenuation *= smoothstep(outerCone, innerCone, alpha);
        }
        else
        {
            L = -spotlights[i].direction;
        }

        float nDotL = saturate(dot(normal, L));
        float3 diffuse = nDotL * spotlights[i].colour * diffuseAlbedo;
        
        //Calculate the specular term
        float3 V = cameraPosition - position.xyz;
        float3 H = normalize(L + V);
        float3 specular = pow(saturate(dot(normal, H)), shininess) * spotlights[i].colour * specularAlbedo * nDotL; //removed: saturate
        
        if (!isInShadow)
        {
            finalColour += (diffuse + specular) * attenuation;
        }
    }
    
    finalColour += ambientAlbedo * 0.25f;
    
    //Apply the appropriate effect
    backBufferUAV[DTid.xy] = float4(finalColour, 1.0f);
}