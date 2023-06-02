RWTexture2D<unorm float4> backBufferUAV : register(u0);

Texture2D<float4> positionGBuffer : register(t0);
Texture2D<float4> colourGBuffer : register(t1);
Texture2D<float4> normalGBuffer : register(t2);

//NEW///////////////////////////////////////////////////////////////////////////
cbuffer CAMERA_CONSTANT_BUFFER : register(b0)
{
    matrix view;
    matrix proj;
    float3 cameraPosition;
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

StructuredBuffer<SpotLight> spotlights : register(t3);
Texture2DArray<float> shadowMaps : register(t4);

sampler shadowMapSampler : register(s0);
////////////////////////////////////////////////////////////////////////////////

[numthreads(8, 8, 1)]                       //Think of a grid of cells reminiscent of threads in OS programming, where every thread runs the code below
                                            //In this case, I make a 8x8 grid, making sure to cover the entire view (by dispatching 100x100)
void main(uint3 DTid : SV_DispatchThreadID) //So the DTid is the index of the specific thread we're working with
{   
    //float3 position = positionGBuffer[DTid.xy].xyz;
    float4 position = positionGBuffer[DTid.xy]; //I'm not sure what the w-value will be in this case. Maybe I manually set it to 1.0f?
    float3 colour = colourGBuffer[DTid.xy].xyz;
    float3 normal = normalize(normalGBuffer[DTid.xy].xyz);
    
    float3 ambient = float3(normalGBuffer[DTid.xy].w, colourGBuffer[DTid.xy].w, 1.0f); //WHERE DOES THIS COME FROM
    //NEW///////////////////////////////////////////////////////////////////////
    float3 specularAlbedo = 1.0f;
    float specularPower = 1.0f;
    
    float3 finalColour = colour;
    float3 specular = 0.0f;
    
    //Allow for multiple spotlights
    //https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/sm5-object-structuredbuffer-getdimensions
    uint count;
    uint stride; //I don't really care about the stride but I need to pass in two values to the GetDimensions() function in order to get the element count
    spotlights.GetDimensions(count, stride);
    for (int i = 0; i < 1; i++)
    {
        //Shadow stuff
        //So the cookbook gives us the formula: float3 shadowMapUV = float3(ndcSpace.x * 0.5f + 0.5f, ndcSpace.y * -0.5f + 0.5f, lightIndex);
        //To get coordinates in ndc space, we do the perspective divide (w-component) on the transformed position
        bool isInShadow = false;
        float shadowFactor = 1.0f;
        float4x4 vp = mul(spotlights[i].view, spotlights[i].proj);
        float4 ndcPos = mul(position, vp);
        //ndcPos = mul(ndcPos, spotlights[i].proj);
        ndcPos.xyz /= ndcPos.w;
        
        //float posDiv = 1 / ndcPos.w;
        //ndcPos.x * posDiv;
        //ndcPos.y * posDiv;
        //ndcPos.z * posDiv;
        
        float3 shadowMapUV = float3(ndcPos.x * 0.5f + 0.5f, ndcPos.y * -0.5f + 0.5f, i); //So this should be correct yes?
        float3 shadowMapSample = shadowMaps.SampleLevel(shadowMapSampler, shadowMapUV, 0.0f); //MSDN: First argument is the sampler state, second is the texture coordinates, third is lod (if the value is = 0, the biggest mipmap is used)
        
        //"If the sampled value is lesser than the calculated value then this means that some other object lies in the path, and thus the fragment should be considered to be in shadow"
        //"As several of the fragments we are currently viewing might share a texel in the shadow map (as it does not have infinite precision) 
        //you may want to add a very small value to the sampled depth to try and avoid self shadowing issues"
        if (shadowMapSample.x + 0.00001f < ndcPos.z)
        {
            isInShadow = true;
            shadowFactor = 0.0f;
        }
        
        //Page 504 of the book "Practical Rendering and Computation with Direct3D11"
        //Calculate the diffuse term
        float3 L = 0;
        float attenuation = 1.0f;
        L = spotlights[i].origin - position.xyz; //Base the light vector on the light position
        
        //Calculate attenuation based on distance from the light source
        float dist = length(L);
        attenuation *= max(0.0f, 1.0f - (dist / 20.0f)); //What I write as "20.0f" here is what the book refers to as LightRange.x, so some arbitrary value representing how far the light reaches
        L /= dist;
        
        //Also add in the spotlight attenuation factor
        //float3 L2 = spotlights[i].direction;
        //float rho = dot(-L, L2); //Book shows this as "-L", but that makes the light go in the wrong direction for me (maybe I should fix the light implementation in D3D11Engine instead?)
        //float rotXY = spotlights[i].rotation.x - spotlights[i].rotation.y;
        //if (rotXY == 0.0f)
        //    rotXY = 1.0f; //Please don't divide by 0
        //attenuation *= saturate(
        //(rho - spotlights[i].rotation.y) /
        //rotXY);
        
        float minCos = cos(spotlights[i].angle);
        float maxCos = (minCos + 1.0f) / 2.0f;
        float cosAngle = dot(spotlights[i].direction, -L);
        attenuation *= smoothstep(minCos, maxCos, cosAngle);

        float nDotL = saturate(dot(normal, L));
        float3 diffuse = nDotL * spotlights[i].colour * colour;
        
        //Calculate the specular term
        float3 V = cameraPosition - position.xyz;
        float3 H = normalize(L + V);
        float3 specular = pow(saturate(dot(normal, H)), specularPower) * spotlights[i].colour * specularAlbedo * nDotL;
        
        
        //lighting += (diffuse + specular) * attenuation * shadowFactor;
        if (!isInShadow)
        {
            finalColour += (diffuse + specular) * attenuation * 1.0f;
        }
        else
        {
            finalColour += (diffuse + specular) * attenuation;
        }
        //lighting = (ambient + diffuse) * base.xyz;
    }
    
    ////////////////////////////////////////////////////////////////////////////
    finalColour += ambient;
    //finalColour += specular;
    backBufferUAV[DTid.xy] += float4(finalColour, 1.0f);
    //int mode = 0;
    
    //Depending on what mode you've selected, the effect will modify the finalColour variable
    
    //Phong Shading
    //if (mode == 0)
    //{
    //    //Ambient lighting (essentially darken the whole screen, as things will instead become bright as a result of light)
    //    float3 ambient = ambientStrength * lightColor;
    
    //    //Diffuse lighting (surfaces becomes brighter the more they're facing the light)
    //    float3 lightDir = normalize(lightPos - position);
    //    float3 diffuseStrength = max(dot(normal, lightDir), 0.0f);
    //    float3 diffuse = diffuseStrength * lightColor;
    
    //    //Specular lighting (shiny specularity depending on the position we're "looking" from)
    //    int shine = 64;
    //    float specularStrength = 0.75f;
    //    float3 cameraDir = normalize(cameraPos - position);
    //    float3 reflectDir = reflect(-lightDir, normal);
    //    float specularComponent = pow(max(dot(cameraDir, reflectDir), 0.0f), shine);
    //    float3 specular = specularStrength * specularComponent * lightColor;
    
    //    finalColour = (ambient + diffuse) * colour;
    //    finalColour += specular;
    //}
    
    ////Blinn-Phong Shading (Not working but hey one day it will. maybe.)
    //else if (mode == 1)
    //{
    //    if (length(normal) > 0.0f)
    //    {
    //        float3 lightDir = normalize(float3(1.0f, 1.0f, 1.0f));
    //        float lambertian = max(dot(lightDir, normal), 0.0f);
    //        float specular = 0.0f;
        
    //        [flatten]
    //        if (lambertian > 0.0f)
    //        {
    //            float3 viewDir = normalize(-position); //position
    //            float3 halfDir = normalize(lightDir + viewDir);
    //            float specAngle = max(dot(halfDir, normal), 0.0f);
    //            specular = pow(specAngle, 100.0f);
    //        }
        
    //        float3 colorLinear = lambertian * colour + specular * lightDir;
    //        finalColour = float4(pow(colorLinear, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f)), 1.0f);
    //    }
    //}
    
    ////Invert Colours
    //else if (mode == 2)
    //{
    //    finalColour = float3(1.0f, 1.0f, 1.0f) - colour;
    //}
    
    ////Blur
    //else if (mode == 3)
    //{
    //    float4 tempColour = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //    for (int w = -5; w < 5; ++w)
    //    {
    //        for (int h = -5; h < 5; ++h)
    //        {
    //            tempColour += colourGBuffer[DTid.xy + float2(w, h)];
    //        }

    //    }
        
    //    finalColour = tempColour.xyz / (10 * 10);
    //}
    
    ////Apply the appropriate effect
    //backBufferUAV[DTid.xy] = float4(finalColour, 1.0f);
}