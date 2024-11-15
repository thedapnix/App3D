
//From camera (duh)
cbuffer CAMERA_CONSTANT_BUFFER : register(b0)
{
    matrix view;
    matrix proj;
    float3 cameraPosition;
};

//From drawables
cbuffer SHININESS_CONSTANT_BUFFER : register(b1)
{
    float shininess;
};
Texture2D ambientTexture : register(t0);
Texture2D diffuseTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D nmapTexture : register(t3);

//From engine
SamplerState samplerState : register(s0);

//Lights and shadowmaps
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
sampler shadowMapSampler : register(s1);

//From domain shader
struct PixelShaderInput
{
    float4 clipPosition : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 nor : NORMAL;
    float4 col : TEXCOORD1;
    
    float4 worldPosition : TEXCOORD2;
    
    float3 tangent : TEXCOORD3;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    //float3 ambientAlbedo = ambientTexture.Sample(samplerState, input.uv).xyz;
    float3 ambient = 0.25f;
    float4 diffuseAlbedo = diffuseTexture.Sample(samplerState, input.uv);
    float3 specularAlbedo = specularTexture.Sample(samplerState, input.uv).xyz;
    float specularPower = shininess;
    
    float4 position = input.worldPosition;
    float3 normal = normalize(input.nor.xyz);
    
    //Normalmap stuff
    float3 N = normal;
    normal = nmapTexture.Sample(samplerState, input.uv).xyz;
    normal = 2.0f * normal - 1.0f;
    float3 tangent = input.tangent;
    float3 T = normalize(tangent - dot(tangent, N) * N);
    float3 B = cross(N, T);
    float3x3 tbn = float3x3(T, B, N);
    normal = mul(normal, tbn);
    
    float3 finalColour = diffuseAlbedo.xyz * ambient; // * ambientAlbedo;
    float3 lighting = 0.0f;
    
    //float3 finalColour = 0.0f;
    
    //Allow for multiple spotlights
    //https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/sm5-object-structuredbuffer-getdimensions
    uint count;
    uint stride; //I don't really care about the stride but I need to pass in two values to the GetDimensions() function in order to get the element count
    spotlights.GetDimensions(count, stride);
    for (int i = 0; i < count; i++)
    {
        //Quick little light culling for a performance boost
        //float3 lightToCam = abs(spotlights[i].origin - cameraPosition);
        //if (lightToCam.x >= 100.0f ||
        //    lightToCam.y >= 100.0f ||
        //    lightToCam.z >= 100.0f)
        //{
        //    continue;
        //}
        
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
            if (spotlights[i].rad == 0.0f) //Directional light
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
        float3 diffuse = nDotL * spotlights[i].colour * diffuseAlbedo.xyz;
        
        //Calculate the specular term
        float3 V = cameraPosition - position.xyz;
        float3 H = normalize(L + V);
        float3 specular = pow(saturate(dot(normal, H)), shininess) * spotlights[i].colour * specularAlbedo * nDotL; //removed: saturate
        
        if (!isInShadow || isPointLight) //|| isPointLight
        {
            //finalColour += (diffuse + specular) * attenuation;
            lighting += (diffuse + specular) * attenuation;

        }
    }
    //finalColour += base * 0.25f;
    //finalColour += ambientAlbedo * 0.25f;
    
    //Apply the lighting to the base color
    finalColour += lighting;
    
    return float4(finalColour, 1.0f);
    
    //return base;
}












/*
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
    //float3 ambient = 0.25f;
    float3 ambientAlbedo = ambientTexture.Sample(samplerState, input.uv);
    float4 base = diffuseTexture.Sample(samplerState, input.uv);
    float3 specularAlbedo = specularTexture.Sample(samplerState, input.uv).xyz;
    float specularPower = shininess;
    
    //float3 finalColor = base.xyz * ambient;
    //float3 lighting = 0.0f;
    
    float3 finalColour = 0.0f;
    
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
        if(shadowMapSample.x + 0.00001f < ndcPos.z)
        {
            isInShadow = true;
        }
        
        //Page 504 of the book "Practical Rendering and Computation with Direct3D11"
        //Calculate the diffuse term
        float3 L = 0;
        float attenuation = 1.0f;
        
        if(spotlights[i].fov != 0.0f) //If it doesn't have an fov, I'll treat it as a directional light
        {
            L = spotlights[i].origin - input.worldPosition.xyz; //Base the light vector on the light position
            //Calculate attenuation based on distance from the light source
            float dist = length(L);
            attenuation = max(0.0f, 1.0f - (dist / 40.0f)); //What I write as "40.0f" here is what the book refers to as LightRange.x, so some arbitrary value representing how far the light reaches
            L /= dist;
            
            //Also add in the spotlight attenuation factor
            //float3 L2 = spotlights[i].direction;
            //float rho = dot(L, L2); //Book shows this as "-L", but that makes the light go in the wrong direction for me (maybe I should fix the light implementation in D3D11Engine instead?)
            //float rotXY = spotlights[i].rotation.x - spotlights[i].rotation.y;
            //if (rotXY == 0.0f)
            //    rotXY = 1.0f; //Please don't divide by 0
            //attenuation *= saturate(
            //(rho - spotlights[i].rotation.y) /
            //rotXY);
            
            //The formula above doesn't take into account the fov of the light, essentially making it more like a directional light, which I think is kinda weird of the book to do
            //https://learn.microsoft.com/en-us/windows/win32/direct3d9/light-types#spotlight
            //Attenuation of 1.0f means we're in full light, 0.0f means we're not in the light. We want to interpolate between 0 and 1 (using, for instance, the hlsl function smoothstep())
            //Msdn states that spotlights have an inner cone and an outer cone, based off of an angle, so outer cone = fov, inner cone = fov / 2
            //"Because the attenuation occurs as the vertex becomes more distant from the center of illumination (rather than across the total cone angle), the runtime divides these cone angles in half before calculating their cosines."
            //Then we check to see if the projected vector of the lights direction onto the pixel (L dot D, effectively the angle) lies within the range of these cones, interpolating thusly
            float outerCone = cos(spotlights[i].fov / 2.0f);
            float innerCone = cos(spotlights[i].fov / 4.0f);
            float alpha = dot(-L, spotlights[i].direction); //I need to do -L here instead of just L
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
        
        if(!isInShadow)
        {
            //lighting += (diffuse + specular) * attenuation;
            finalColour += (diffuse + specular) * attenuation;
        }
    }
    //finalColour += base * 0.25f;
    finalColour += ambientAlbedo * 0.25f;
    
    return float4(finalColour, 1.0f);
}
*/