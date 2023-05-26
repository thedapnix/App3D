
struct VertexShaderInput
{
    float3 localPosition : POS;
};

struct VertexShaderOutput
{
    float4 worldPosition : SV_POSITION;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.worldPosition = float4(input.localPosition, 1.0f);
    return output;
}