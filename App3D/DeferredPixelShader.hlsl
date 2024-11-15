
Texture2D ambientTexture : register(t0);
Texture2D diffuseTexture : register(t1);
Texture2D specularTexture : register(t2);

Texture2D nmapTexture : register(t3);

SamplerState samplerState : register(s0);

//Per-object constant buffer makes a return in order to determine whether or not a drawable has a normalmap
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
    
    //Here we store position-, normal-, ambient-, diffuse-, specular-, and shininess-data into our 4 g-buffers for use in the compute shader
    
    //Store position into the xyz-components and shininess into the w-component into the position g-buffer
    output.position = float4(input.worldPosition.xyz, shininess);
    
    //Store ambient-, diffuse-, and specular-data into the xyz-components of their respective g-buffers
    float3 ambient = ambientTexture.Sample(samplerState, input.uv).xyz;
    float3 diffuse = diffuseTexture.Sample(samplerState, input.uv).xyz;
    float3 specular = specularTexture.Sample(samplerState, input.uv).xyz;
    
    //Pack the normal into the w-components of the previous three g-buffers, but not before doing potential normalmap calculations
    float3 normal = input.nor.xyz;
    if (hasNormalMap)
    {
        //Store N for the upcoming TBN matrix before we operate on the normal-variable
        float3 N = normal;
        
        //Sample new normal from normalmap
        normal = nmapTexture.Sample(samplerState, input.uv).xyz;
        normal = 2.0f * normal - 1.0f; //Uncompress components from (0, 1) to (-1, 1)
        
        //Calculate TBN matrix
        float3 T = normalize(input.tangent - dot(input.tangent, N) * N); //Re-orthogonalize the basis vectors, Gram-Schmidt process
        float3 B = cross(N, T);
        float3x3 tbn = float3x3(T, B, N);
        
        //Apply the TBN matrix transformation to bring the normals from tangent space into world space
        normal = mul(normal, tbn);
    }
    
    //Finish packing the g-buffers before outputting all our data
    output.ambient = float4(ambient, normal.x);
    output.diffuse = float4(diffuse, normal.y);
    output.specular = float4(specular, normal.z);
    
    return output;
}