
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
    float rad;
    
    float3 padding; //Padding my beloved :))))))
};

StructuredBuffer<SpotLight> spotlights : register(t4);
Texture2DArray<float> shadowMaps : register(t5);
//sampler shadowMapSampler : register(s0);
SamplerState shadowMapSampler : register(s0);

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
        //Quick little light culling for a performance boost
        float3 lightToCam = abs(spotlights[i].origin - cameraPosition);
        if(lightToCam.x >= 100.0f || 
            lightToCam.y >= 100.0f ||
            lightToCam.z >= 100.0f)
        {
            continue;
        }
        
        //Since shadow cameras look in a specific direction, they're not compatible with point lights. Potentially I could make point lights have 4 shadow cameras but not right now
        bool isPointLight = false; //Temp to disable shadows with point lights
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
        float attenuation = 1.0f; //Set this to 0.25f instead of setting the color weaker on the C++ side? Should have the same effect, but move work from CPU to GPU
        
        if (spotlights[i].fov != 0.0f) //Spotlight
        {
            L = spotlights[i].origin - position.xyz;
            float dist = length(L);
            attenuation = max(0.0f, 1.0f - (dist / 40.0f)); //40.0f represents the range of the spotlight, consider passing this in from the structured buffer, hence letting spotlights pass in their range and point lights pass in their radius?
            L /= dist;
            
            float outerCone = cos(spotlights[i].fov / 2.0f);
            float innerCone = cos(spotlights[i].fov / 4.0f);
            float alpha = dot(-L, spotlights[i].direction);
            attenuation *= smoothstep(outerCone, innerCone, alpha);
        }
        else
        {
            if(spotlights[i].rad == 0.0f) //Directional light
            {
                L = -spotlights[i].direction;
            }
            else //Point light
            {
                isPointLight = true;
                //Same as with spotlights, get vector from light to pixel
                L = spotlights[i].origin - position.xyz;
                float dist = length(L); //Get distance of vector
                
                //Calculate attenuation using 2 * pi * rad instead of using a cone with a certain reach and fov? (nah just use radius and ignore shadow calcs)
                attenuation = max(0.0f, 1.0f - (dist / spotlights[i].rad));
                
                L /= dist; //Normalize vector
            }
        }

        float nDotL = saturate(dot(normal, L));
        float3 diffuse = nDotL * spotlights[i].colour * diffuseAlbedo;
        
        //Calculate the specular term
        float3 V = cameraPosition - position.xyz;
        float3 H = normalize(L + V);
        float3 specular = pow(saturate(dot(normal, H)), shininess) * spotlights[i].colour * specularAlbedo * nDotL; //removed: saturate
        
        if (!isInShadow || isPointLight) //|| isPointLight
        {
            finalColour += (diffuse + specular) * attenuation;
        }
    }
    
    finalColour += ambientAlbedo * 0.25f;
    
    //Apply the appropriate effect
    backBufferUAV[DTid.xy] = float4(finalColour, 1.0f);
}