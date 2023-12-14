#version 330 core
layout (location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 uvCoords;

uniform mat4 m_view;
uniform mat4 m_proj;

void main()
{
    uvCoords = position;
    gl_Position = m_proj * m_view * vec4(position, 1.0f);
}
