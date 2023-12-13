#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cloth
{
public:
    std::vector<float> generateShape() { return m_vertexData; }
    Cloth();
    void updateParams(std::vector<float> newPositions);
    std::vector<float> m_vertexData;

    struct Particle {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 oldPosition;
        glm::vec3 velocity;
        glm::vec3 acceleration;
    };

    Particle p1;
    Particle p2;
    Particle p3;
    Particle p4;
    Particle p5;
    Particle p6;
    Particle p7;
    Particle p8;
    Particle p9;

    glm::vec3 pp1;
    glm::vec3 pp2;
    glm::vec3 pp3;
    glm::vec3 pp4;
    glm::vec3 pp5;
    glm::vec3 pp6;
    glm::vec3 pp7;
    glm::vec3 pp8;
    glm::vec3 pp9;

    Particle getParticle1();

    void updateParticle2(glm::vec3 newP2pos);
    Particle getParticle2();

    Particle getParticle3();

    void updateParticle4(glm::vec3 newP4pos);
    Particle getParticle4();

    void updateParticle5(glm::vec3 newP5pos);
    Particle  getParticle5();

    void updateParticle6(glm::vec3 newP6pos);
    Particle getParticle6();

    void updateParticle7(glm::vec3 newP7pos);
    Particle  getParticle7();

    void updateParticle8(glm::vec3 newP8pos);
    Particle getParticle8();

    void updateParticle9(glm::vec3 newP9pos);
    Particle getParticle9();

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);

    void setVertexData();

    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight, int counter);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);

    void calcTangentData(glm::vec3 topLeft,
                         glm::vec3 topRight,
                         glm::vec3 bottomLeft,
                         glm::vec3 bottomRight);

    void calcUVCoords(glm::vec3 objPos);

//    int m_param1 = 2; // starting off with 2
};
