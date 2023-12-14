#version 330 core

layout (location = 0) in vec3 m_worldPos;
layout (location = 1) in vec3 m_ObjNormal;
layout (location = 2) in vec2 m_uvCoords;
layout (location = 3) in vec3 m_Tangent;
layout (location = 4) in vec3 m_Bitangent;

out vec2 uvCoords;
out mat3 TBN; // Tangent space to world space matrix

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D normalMap;

void main()
{
gl_Position = m_worldPos;

// Sample normal from the normal map
vec3 normalTex = texture(normalMap, m_uvCoords).rgb * 2.0 - 1.0;

// Create TBN matrix
vec3 T = normalize(vec3(model * vec4(m_Tangent,   0.0)));
vec3 B = normalize(vec3(model * vec4(m_Bitangent, 0.0)));
vec3 N = normalize(vec3(model * vec4(m_ObjNormal,    0.0)));
mat3 TBN = mat3(T, B, N);
// mat3 TBNMatrix = mat3(aTangent, aBitangent, aNormal);

// Transform normal to world space
vec3 normal = normalize(TBN * normalTex);

// Pass data to the fragment shader
uvCoords = m_uvCoords;
TBN = transpose(inverse(TBN));

}
