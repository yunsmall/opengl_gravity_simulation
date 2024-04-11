#version 330 core

layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;

out vec3 FragViewPos;
out vec3 ViewNormal;
out vec3 lightViewPos;

uniform vec3 lightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragViewPos = vec3(view *model * vec4(aPos, 1.0));
    ViewNormal =  mat3(transpose(inverse(view *model))) * aNormal;
    lightViewPos=vec3(view *vec4(lightPos, 1.0));
}