#type vertex
#version 330 core 

layout(location = 0) in vec3 a_Position;
uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

void main(){
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core 

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 color2;

uniform vec4 u_Color;
void main(){
    color = u_Color;
    color2 = vec4(0.1f, 0.2f, 0.7f, 1.0f)
}