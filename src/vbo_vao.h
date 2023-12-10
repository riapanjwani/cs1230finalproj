#pragma once
#include "GL/glew.h"
#include <shapes/cube.h>
#include <shapes/sphere.h>
#include <shapes/Cylinder.h>
#include <shapes/cone.h>
#include <utils/scenedata.h>

class vbo_vao
{
public:
    vbo_vao(GLuint &vao); // generates VBO and VAO
    ~vbo_vao();

    int fillArray(PrimitiveType currShape, int param1, int param2);
    void draw(int numTriangles, GLuint &vao);
    void deleteData();

private:
    GLuint m_vbo; // Stores id of vbo
    GLuint *m_vao;
    Cube *m_cube;
    Sphere *m_sphere;
    Cylinder *m_cylinder;
    Cone *m_cone;
};
