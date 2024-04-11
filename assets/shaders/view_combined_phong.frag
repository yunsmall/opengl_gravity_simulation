#version 330 core

in vec3 FragViewPos;
in vec3 ViewNormal;
in vec3 lightViewPos;

out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
    float ambientStrength=0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(ViewNormal);
    vec3 lightDir = normalize(lightViewPos-FragViewPos);

    float diff=max(dot(norm,lightDir),0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(- FragViewPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}