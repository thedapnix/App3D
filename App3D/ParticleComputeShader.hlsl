//cbuffer PARTICLE_CONSTANT_BUFFER : register(b0)
//{
//    float angle;
//}

struct Particle //same as the one in the vertex shader
{
    float3 pos;
};

RWStructuredBuffer<Particle> Particles : register(u0);

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    Particle current = Particles[DTid.x];
    //process the particle in some way, like make it spin around in a circular manner? let the constant buffer send in an angle of rotation?
    Particles[DTid.x] = current;
}