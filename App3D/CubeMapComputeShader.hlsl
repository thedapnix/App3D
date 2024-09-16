
RWTexture2DArray<unorm float4> uav : register(u0);

//Like in our regular compute shader, but we only expect one g-buffer
Texture2D<float4> gBuffer : register(t0);

//This shader updates the backbuffer using a uav (which usually targets the whole image), but targets specific objects depending on an arbitrary value stored in their w-component
[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    //Get the texture from the gbuffer
    float3 color = gBuffer[DTid.xy].xyz;

    //Only render if the w-component has this value
    if (gBuffer[DTid.xy].w == 69.0f) //Another comment for further clarity (see CubeMapPixelShader.hlsl), this value has to be separate from any .mtl shininess value
    {
        uav[DTid] = float4(saturate(color), 1.0f);
    }
    
    //This might be a bad way of doing things, since for every object I want to do this with, I lock more shininess values into the "don't touch" box
    //So if you only want to render one thing to the scene, even if you're using deferred rendering as the default for the whole scene, it's probably best to forward render that specific object
}