#version 330 core
out vec4 FragColor;

in vec3 uvCoords;

uniform samplerCube m_skybox;

void main()
{
    FragColor = texture(m_skybox, uvCoords);
}
