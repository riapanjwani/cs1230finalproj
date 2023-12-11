#include "cone.h"
#include <iostream>

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = (param1 > 1) ? param1 : 2;
    m_param2 = (param2 > 2) ? param2 : 3;
    setVertexData();
}

void Cone::makeSideTriangle(glm::vec3 top,
                            glm::vec3 left,
                            glm::vec3 right) {

    glm::vec3 normal = glm::normalize(glm::cross(left - top, right - top));
    glm::vec3 origin = {0.0f, 0.0f, -0.5f};
    glm::vec3 normalLeft = glm::normalize(glm::cross(left - top, right- origin));
    glm::vec3 normalRight = glm::normalize(glm::cross(top - right, origin -right));

    insertVec3(m_vertexData, top);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, left);
    insertVec3(m_vertexData, normalLeft);

    insertVec3(m_vertexData, right);
    insertVec3(m_vertexData, normalRight);

}

void Cone::makeTileBase(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight) {

    glm::vec3 normalBottom = {0.0f, -1.0f, 0.0f};
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalBottom);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottom);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalBottom);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalBottom);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalBottom);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottom);
}

void Cone::makeTileSide(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight) {

    glm::vec3 normalTopLeft = {2.0f * topLeft[0], 0.5f * (0.5f - topLeft[1]), 2.0f* topLeft[2]};
    glm::vec3 normalTopRight = {2.0f * topRight[0], 0.5f * (0.5f - topRight[1]), 2.0f* topRight[2]};
    glm::vec3 normalBottomLeft = {2.0f * bottomLeft[0], 0.5f * (0.5f - bottomLeft[1]), 2.0f* bottomLeft[2]};
    glm::vec3 normalBottomRight = {2.0f * bottomRight[0], 0.5f * (0.5f - bottomRight[1]), 2.0f* bottomRight[2]};


    insertVec3(m_vertexData, topLeft);
    if (topLeft[0]==0.0f && topLeft[1]==0.5f && topLeft[2] ==0.0f){
        glm::vec3 tipNormal = {2.0f * topLeft[0], 0.5f, 2.0f* topLeft[2]};
        insertVec3(m_vertexData,glm::normalize(tipNormal));
    }
    else{
        insertVec3(m_vertexData, glm::normalize(normalTopLeft));
    }

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(normalBottomLeft));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(normalBottomRight));

    insertVec3(m_vertexData, topLeft);
    if (topLeft[0]==0.0f && topLeft[1]==0.5f && topLeft[2] ==0.0f){
        glm::vec3 tipNormal = {2.0f * topLeft[0], 0.5f, 2.0f* topLeft[2]};
        insertVec3(m_vertexData,glm::normalize(tipNormal));
        // average of bottom normals
    }
    else {
        insertVec3(m_vertexData, glm::normalize(normalTopLeft));
    }

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(normalBottomRight));

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(normalTopRight));
}

void Cone::makeBase(float currTheta, float nextTheta) {
    int numSegments = m_param1;
    float baseTileDelta = 1.0f/ numSegments;

    for (int i = 0; i < numSegments; i++){
        glm::vec3 bTopLeft = calculateBaseVertex(currTheta, baseTileDelta * i, -0.5);
        glm::vec3 bTopRight = calculateBaseVertex(nextTheta, baseTileDelta * i, -0.5);
        glm::vec3 bBottomLeft = calculateBaseVertex(currTheta, baseTileDelta * (i+1), -0.5);
        glm::vec3 bBottomRight = calculateBaseVertex(nextTheta, baseTileDelta * (i+1), -0.5);

        makeTileBase(bTopLeft, bTopRight, bBottomLeft, bBottomRight);
    }
}

void Cone::makeSide(float currTheta, float nextTheta){

    int numSegments = m_param1;
    float tileDelta = 1.0f/ numSegments;

    for (int i = 0; i < numSegments; i++){
        glm::vec3 topLeft = calculateSideVertex(currTheta, tileDelta, 0.5 - (tileDelta * i), numSegments);
        glm::vec3 topRight = calculateSideVertex(nextTheta, tileDelta, 0.5 - (tileDelta * i), numSegments);
        glm::vec3 bottomLeft = calculateSideVertex(currTheta, tileDelta,  0.5f - tileDelta * (i+1), numSegments);
        glm::vec3 bottomRight = calculateSideVertex(nextTheta, tileDelta, 0.5f - tileDelta * (i+1), numSegments);

        makeTileSide(topLeft, topRight, bottomLeft, bottomRight);
    }
}

void Cone::setVertexData() {
    m_vertexData.clear();

    float thetaStep = glm::radians(360.0f/ m_param2);
    for (int i = 0; i < m_param2; i++){
        float currTheta = i * thetaStep;
        float nextTheta = (i+1) * thetaStep;

        makeSide(currTheta, nextTheta);
        makeBase(currTheta, nextTheta);
    }
}

glm::vec3 Cone::calculateBaseVertex(float currTheta, float tileDelta, float z){
    float radius = m_radius * tileDelta;
    float x = (radius * sin(currTheta));
    float y = z;
    float zed = (radius * cos(currTheta));

    return glm::vec3(x,y,zed);
}

glm::vec3 Cone::calculateSideVertex(float currTheta, float tileDelta, float height, float numSegments){
    float radius = 0.5 * (0.5 - height);

    float x = (radius * sin(currTheta));
    float y = height;
    float z = (radius* cos(currTheta));

    return glm::vec3(x,y,z);
}

// Inserts a glm::vec3 into a vector of floats.
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
