#type vertex
#version 330 core 

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexUV;
uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec3 v_Position;
out vec2 v_TexUV;

void main(){
    v_Position = a_Position;
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
    v_TexUV = a_TexUV;
}

#type fragment
#version 330 core 

layout(location = 0) out vec4 color;

in vec3 v_Position;
in vec2 v_TexUV;

uniform sampler2D u_Texture;

void main(){
    color = texture(u_Texture, v_TexUV);
}