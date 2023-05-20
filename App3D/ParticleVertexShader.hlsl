//Very basic, straightup yoinked from the cookbook
struct Particle
{
    float3 pos;
};

StructuredBuffer<Particle> Particles : register(t0);

float3 main( uint vertexID : SV_VertexID ) : POSITION
{
    return Particles[vertexID].pos;
}