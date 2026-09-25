#version 410

uniform sampler2D u_Textures[16];

layout(location = 0) out vec4 color;
layout(location = 3) flat in float v_TexIndex;
layout(location = 1) in vec2 v_TexUV;
layout(location = 2) in vec2 v_TilingFactor;
layout(location = 0) in vec4 v_Color;
layout(location = 1) out int entityID;
layout(location = 4) flat in int v_EntityID;

void main()
{
    color = texture(u_Textures[int(v_TexIndex)], v_TexUV * v_TilingFactor) * v_Color;
    entityID = v_EntityID;
}

