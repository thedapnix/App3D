
cbuffer PARTICLE_CONSTANT_BUFFER : register(b0)
{
    float angle;
}

struct Particle //same as the one in the vertex shader
{
    float3 pos;
    float padding; //hate having to do this
};

RWStructuredBuffer<Particle> Particles : register(u0);

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    Particle current = Particles[DTid.x];
    
    current.pos[0] = cos(angle * 0.001f + DTid.x);
    current.pos[1] = sin(angle * 0.001f + DTid.x);
    current.pos[2] = cos(angle * 0.003f + DTid.x);
    
    Particles[DTid.x] = current;
}