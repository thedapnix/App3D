
Texture2D ambientTexture : register(t0);
Texture2D diffuseTexture : register(t1);
Texture2D specularTexture : register(t2);

//Todo: Add new Texture2D and sample from it, but only if a normal map is available for the mesh (So we have to pass that in through the constant buffer
Texture2D nmapTexture : register(t3);

SamplerState samplerState : register(s0);

//Return of the per-object constant buffer, need to know if the object has a normalmap or nah
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    matrix world;
    matrix invWorld;
    
    bool hasNormalMap;
};

cbuffer SHININESS_CONSTANT_BUFFER : register(b1)
{
    float shininess;
};

struct PixelShaderInput
{
    float4 clipPosition : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 nor : NORMAL;
    float4 col : TEXCOORD1;
    
    float4 worldPosition : TEXCOORD2;
    
    float3 tangent : TEXCOORD3;
};

//Deferred renderer requires pixel shader to output more than just a color
struct PixelShaderOutput
{
    float4 position : SV_Target0;
    float4 ambient : SV_Target1;
    float4 diffuse : SV_Target2;
    float4 specular : SV_Target3;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output = (PixelShaderOutput) 0;
    
    //Assignment states that *at least* the following data needs to be stored:
    //Position, normal, ambient, diffuse, specular, shininess
    //6 different containers, 16 values in total
    //The cookbook gives us the tip that we can store values separately into 4 4-dimensional containers instead (which saves on memory and read-operations)
    
    //So first I can store the shininess as the 4th component in the position container
    output.position = float4(input.worldPosition.xyz, shininess);
    
    float3 ambient = ambientTexture.Sample(samplerState, input.uv).xyz;
    float3 diffuse = diffuseTexture.Sample(samplerState, input.uv).xyz;
    float3 specular = specularTexture.Sample(samplerState, input.uv).xyz;
    
    //And then pack the normal into the 4th component of the other texture containers (New: Fetch normals from the normalmapped texture instead of the regular normal, if it exists)
    float3 normal = input.nor.xyz;
    
    if (hasNormalMap)
    {
        //Store N for the upcoming TBN matrix before we operate on the normal-variable
        float3 N = normal;
        
        //Sample new normal from normal map
        normal = nmapTexture.Sample(samplerState, input.uv).xyz;
        normal = 2.0f * normal - 1.0f; //Uncompress components from (0, 1) to (-1, 1)
        
        //float3 tangent = float3(2.0f, 0.0f, 0.0f);
        float3 tangent = input.tangent;
        
        //Calculate TBN matrix
        float3 T = normalize(tangent - dot(tangent, N) * N); //Re-orthogonalize the basis vectors, Gram-Schmidt process
        float3 B = cross(N, T);
        float3x3 tbn = float3x3(T, B, N);
        
        //Apply the TBN matrix transformation to bring the normals from tangent space into the world
        normal = mul(normal, tbn);
    }
    
    output.ambient = float4(ambient, normal.x);
    output.diffuse = float4(diffuse, normal.y);
    output.specular = float4(specular, normal.z);
    
    return output;
}