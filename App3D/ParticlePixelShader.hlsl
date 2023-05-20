//I don't want to do anything fancy here, just color the particles green
//Might need to make it compatible with the gbuffer (deferred rendering)?
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}