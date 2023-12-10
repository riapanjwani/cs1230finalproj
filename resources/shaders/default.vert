#version 330 core

layout(location = 0) in vec3 m_objSpacePosition;
layout(location = 1) in vec3 m_objSpaceNormal;

// `out` variables for the world-space position and normal
out vec3 m_world_position;
out vec3 m_world_normal;

uniform mat4 m_model;
uniform mat4 m_view;
uniform mat4 m_proj;

void main() {

    vec4 worldPosition = m_model * vec4(m_objSpacePosition, 1.0f);

    mat3 inverseTransposeMat = transpose(inverse(mat3(m_model)));
    vec3 worldNormal =  normalize(inverseTransposeMat * m_objSpaceNormal);

    m_world_position = vec3(worldPosition);
    m_world_normal = worldNormal;

    // set gl_Position to world space position transformed to clip space
    gl_Position = m_proj * (m_view * vec4(m_world_position, 1.f));
}
