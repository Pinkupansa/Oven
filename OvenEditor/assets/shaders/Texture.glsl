#type vertex
#version 330 core 

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexUV;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in vec2 a_TilingFactor;
layout(location = 5) in int a_EntityID;


uniform mat4 u_ViewProjection;

out vec2 v_TexUV;
out vec4 v_Color;
out vec2 v_TilingFactor;
out float v_TexIndex;
flat out int v_EntityID;

void main(){
    v_Color = a_Color;
    v_TexUV = a_TexUV;
    v_TexIndex = a_TexIndex;
    v_TilingFactor = a_TilingFactor;
    v_EntityID = a_EntityID;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core 

layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;

in vec2 v_TexUV;
in vec4 v_Color;
in float v_TexIndex;
in vec2 v_TilingFactor;
flat in int v_EntityID;

uniform vec4 u_Color;
uniform sampler2D u_Textures[16];

void main(){
    //color = texture(u_Texture, v_TexUV * u_TilingFactor) * u_Color;
    color = texture(u_Textures[int(v_TexIndex)], v_TexUV * v_TilingFactor) * v_Color;
    //color = vec4(vec3(v_EntityID), 1.0f);
    entityID = v_EntityID;
}