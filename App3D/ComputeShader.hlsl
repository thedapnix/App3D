RWTexture2D<unorm float4> backBufferUAV : register(u0);

Texture2D<float4> positionGBuffer : register(t0);
Texture2D<float4> colourGBuffer : register(t1);
Texture2D<float4> normalGBuffer : register(t2);

[numthreads(8, 8, 1)]                       //Think of a grid of cells reminiscent of threads in OS programming, where every thread runs the code below
                                            //In this case, I make a 8x8 grid, making sure to cover the entire view (by dispatching 100x100)
void main(uint3 DTid : SV_DispatchThreadID) //So the DTid is the index of the specific thread we're working with
{
    //Lighting variables here instead of in the pixel shader
    //float3 lightPos = { 0.5f, 0.5f, -2.5f }; //Center of the screen (approximately)
    //float3 lightColor = { 1.0f, 1.0f, 1.0f }; //Pure white
    //float3 ambientStrength = { 0.2f, 0.2f, 0.2f }; //Dark
    //float3 cameraPos = { 0.5f, 0.5f, -2.0f }; //Slightly in front of the position of the light
    
    float3 position = positionGBuffer[DTid.xy].xyz;
    float3 colour = colourGBuffer[DTid.xy].xyz;
    float3 normal = normalize(normalGBuffer[DTid.xy].xyz);
    
    float3 finalColour = colour;
    backBufferUAV[DTid.xy] = float4(finalColour, 1.0f);
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