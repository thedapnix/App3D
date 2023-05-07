cbuffer CAMERA_CONSTANT_BUFFER : register(b0)
{
    matrix view;
    matrix proj;
    /*matrix viewProjection;
    float3 cameraPosition;
    float padding;*/
};

cbuffer OBJECT_CONSTANT_BUFFER : register(b1)
{
    //matrix world;
    matrix scale;
    matrix rotate;
    matrix translate;
};

struct VertexShaderInput
{
    float3 localPosition : POS;
    float2 uv : UV;
    float3 localNormal : NOR;
};

struct VertexShaderOutput
{
    float4 clipPosition : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 nor : NORMAL;
    float4 col : TEXCOORD1;
    
    float4 worldPosition : TEXCOORD2;
};

VertexShaderOutput main(VertexShaderInput input)
{   
    VertexShaderOutput output = (VertexShaderOutput)0; //zero the memory
    
    float4 pos = float4(input.localPosition, 1.0f);
    //pos = mul(pos, world);
    pos = mul(pos, scale);
    pos = mul(pos, rotate);
    pos = mul(pos, translate);
    output.worldPosition = pos;
    pos = mul(pos, view);
    pos = mul(pos, proj);
    output.clipPosition = pos;
    
    output.uv = input.uv; // float2(input.uv.x, input.uv.y); //input.uv;
    
    float4 nor = float4(input.localNormal, 0.0f);
    //nor = mul(nor, world);
    nor = mul(nor, scale); //redundant? normals should stay unit length
    nor = mul(nor, rotate);
    nor = mul(nor, translate);
    output.nor = nor;
    
    output.col = float4(1.0f, 0.0f, 0.0f, 1.0f);
    
    return output;
}