#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cylinder
{
public:
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();
    void makeBase(float currTheta, float nextTheta);
    void makeTileSide(glm::vec3 topLeft,
                                glm::vec3 topRight,
                                glm::vec3 bottomLeft,
                      glm::vec3 bottomRight);
    void makeBaseTriangle(glm::vec3 top,
                                    glm::vec3 left,
                                    glm::vec3 right);
    void makeTileBase(glm::vec3 topLeft,
                                glm::vec3 topRight,
                                glm::vec3 bottomLeft,
                                glm::vec3 bottomRight);
    void makeSide(float currTheta, float nextTheta);
    glm::vec3 calculateVertex(float currTheta, float height);
    glm::vec3 calculateBaseVertex(float currTheta, float tileDelta, float z);

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
};
