#version 410

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[1];
};

struct VertexOutput
{
    vec4 Color;
    vec2 TexUV;
    vec2 TilingFactor;
    float TexIndex;
};

layout(std140) uniform Camera
{
    mat4 u_ViewProjection;
} u_Camera;

layout(location = 0) out VertexOutput v_Output;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexUV;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in vec2 a_TilingFactor;
layout(location = 4) flat out int v_EntityID;
layout(location = 5) in int a_EntityID;
layout(location = 0) in vec3 a_Position;

void main()
{
    v_Output.Color = a_Color;
    v_Output.TexUV = a_TexUV;
    v_Output.TexIndex = a_TexIndex;
    v_Output.TilingFactor = a_TilingFactor;
    v_EntityID = a_EntityID;
    gl_Position = u_Camera.u_ViewProjection * vec4(a_Position, 1.0);
}

