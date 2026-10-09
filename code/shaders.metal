#include <metal_stdlib>

using namespace metal;

struct vertex_in
{
    float3 Position [[attribute(0)]];
    float2 UV [[attribute(1)]];
};

struct object_matrices
{
    float4x4 Rotation;
    float4x4 Translation;
};

struct global_matrices
{
    float4x4 View;
    float4x4 Perspective;
};

struct vertex_out
{
    float4 Position [[position]];
    float2 UV;
};

struct fragment_out
{
    half4 Color [[color(0)]];
};

[[vertex]] vertex_out
VertexFunction(vertex_in Vertex [[stage_in]], 
               constant object_matrices *ObjectMatrices [[buffer(1)]],
               constant global_matrices &GlobalMatrices [[buffer(2)]],
               uint InstanceID [[instance_id]])
{
    vertex_out Result;
    
    object_matrices ObjectMatrices = ObjectMatrices[InstanceID];
    
    Result.Position = GlobalMatrices.Perspective * GlobalMatrices.View * ObjectMatrices.Translation * ObjectMatrices.Rotation * float4(Vertex.Position, 1.0f);
    Result.UV = Vertex.UV;
    
    return Result;
}

[[fragment]] fragment_out
FragmentFunction(vertex_out Fragment [[stage_in]],
                 texture2d<half> Texture [[texture(0)]],
                 sampler Sampler [[sampler(0)]])
{
    fragment_out Result;
    
    //Result.Color = half4(half2(Fragment.UV), 0.0h, 1.0h);
    Result.Color = Texture.sample(Sampler, Fragment.UV);
    
    return Result;
}

#endif