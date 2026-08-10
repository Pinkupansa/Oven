#type vertex
#version 330 core 

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexUV;
uniform mat4 u_ViewProjection;

out vec2 v_TexUV;
out vec4 v_Color;

void main(){
    v_Color = a_Color;
    v_TexUV = a_TexUV;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core 

layout(location = 0) out vec4 color;

in vec2 v_TexUV;
in vec4 v_Color;

uniform vec4 u_Color;
uniform vec2 u_TilingFactor;
uniform sampler2D u_Texture;

void main(){
    //color = texture(u_Texture, v_TexUV * u_TilingFactor) * u_Color;
    color = v_Color;
}