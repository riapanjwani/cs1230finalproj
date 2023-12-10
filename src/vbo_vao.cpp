#include "vbo_vao.h"
#include <iostream>

vbo_vao::vbo_vao(GLuint &vao) {
    // Generate VBO and then VAO
    m_vao = &vao;
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, m_vao);
}

int vbo_vao::fillArray(PrimitiveType currShape, int param1, int param2){
    std::vector<GLfloat> vertices;

    // Generate vertex data based on current shape
    if (currShape == PrimitiveType::PRIMITIVE_CONE) {
        m_cone = new Cone;
        m_cone->updateParams(param1, param2);
        vertices = m_cone->generateShape();
        delete(m_cone);
    } else if (currShape == PrimitiveType::PRIMITIVE_CUBE) {
        m_cube = new Cube;
        m_cube->updateParams(param1);
        vertices = m_cube->generateShape();
        delete(m_cube);
    } else if (currShape == PrimitiveType::PRIMITIVE_SPHERE) {
        m_sphere = new Sphere();
        m_sphere->updateParams(param1, param2);
        vertices = m_sphere->generateShape();
        delete(m_sphere);
    } else if (currShape == PrimitiveType::PRIMITIVE_CYLINDER) {
        m_cylinder = new Cylinder();
        m_cylinder->updateParams(param1, param2);
        vertices = m_cylinder->generateShape();
        delete(m_cylinder);
    }

    int numTriangles = int(vertices.size()) / 6;
    // Bind and send data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    // Bind VAO
    glBindVertexArray(*m_vao);

    // Enable and define attribute 0 and 1 to store vertex positions and normals
    // contiguously
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    // Clean-up bindings
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return numTriangles;
}

void vbo_vao::draw(int numTriangles, GLuint &vao){
    // Bind VAO & Draw Command
    m_vao = &vao;
    glBindVertexArray(*m_vao);
    glDrawArrays(GL_TRIANGLES, 0, numTriangles);
    glBindVertexArray(0);
}

// destructor
void vbo_vao::deleteData() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, m_vao);
}

