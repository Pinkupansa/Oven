#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexUV;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in vec2 a_TilingFactor;
layout(location = 5) in int a_EntityID;

layout(std140, binding = 0) uniform Camera {
    mat4 u_ViewProjection;
} u_Camera;

struct VertexOutput {
    vec4 Color;
    vec2 TexUV;
    vec2 TilingFactor;
    float TexIndex;
};

layout(location = 0) out VertexOutput v_Output;
layout(location = 4) flat out int v_EntityID;

void main() {
    v_Output.Color = a_Color;
    v_Output.TexUV = a_TexUV;
    v_Output.TexIndex = a_TexIndex;
    v_Output.TilingFactor = a_TilingFactor;
    v_EntityID = a_EntityID;
    gl_Position = u_Camera.u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

struct VertexOutput {
    vec4 Color;
    vec2 TexUV;
    vec2 TilingFactor;
    float TexIndex;
};

layout(location = 0) in VertexOutput v_Output;
layout(location = 4) flat in int v_EntityID;

layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;

layout(binding = 0) uniform sampler2D u_Textures[16];

void main() {
    color = texture(u_Textures[int(v_Output.TexIndex)], v_Output.TexUV * v_Output.TilingFactor) * v_Output.Color;
    entityID = v_EntityID;
}