#version 330 core

// layout variables representing position & UV coordinate
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 m_uvCoords;

// an "out" variable representing the UV coordinate
out vec2 uvCoords;

void main() {
    // assign the UV layout variable to the UV "out" variable
    uvCoords = m_uvCoords;

    gl_Position = vec4(position, 1.0);
}
