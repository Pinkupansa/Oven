#version 410

struct VertexOutput
{
    vec4 Color;
    vec2 TexUV;
    vec2 TilingFactor;
    float TexIndex;
};

uniform sampler2D u_Textures[16];

layout(location = 0) out vec4 color;
layout(location = 0) in VertexOutput v_Output;
layout(location = 1) out int entityID;
layout(location = 4) flat in int v_EntityID;

void main()
{
    color = texture(u_Textures[int(v_Output.TexIndex)], v_Output.TexUV * v_Output.TilingFactor) * v_Output.Color;
    entityID = v_EntityID;
}

