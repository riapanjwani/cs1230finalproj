#include "Cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = (param2 > 2) ? param2 : 3;
    setVertexData();
}

void Cylinder::makeTileSide(glm::vec3 topLeft,
                            glm::vec3 topRight,
                            glm::vec3 bottomLeft,
                            glm::vec3 bottomRight) {

    glm::vec3 normalTopLeft = glm::normalize(glm::vec3(topLeft[0], 0.0f, topLeft[2]));
    glm::vec3 normalTopRight = glm::normalize(glm::vec3(topRight[0], 0.0f,topRight[2]));
    glm::vec3 normalBottomLeft = glm::normalize(glm::vec3(bottomLeft[0],0.0f,bottomLeft[2]));
    glm::vec3 normalBottomRight = glm::normalize(glm::vec3(bottomRight[0],0.0f,bottomRight[2]));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalBottomLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalTopRight);
}

void Cylinder::makeTileBase(glm::vec3 topLeft,
                            glm::vec3 topRight,
                            glm::vec3 bottomLeft,
                            glm::vec3 bottomRight) {

    if(topLeft[1] == 0.5){
        glm::vec3 normalTop = {0.0f, 1.0f, 0.0f};
        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normalTop);

        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normalTop);

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normalTop);

        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normalTop);

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normalTop);

        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normalTop);
    }
    else if (topLeft[1] == -0.5){
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
}


void Cylinder::makeSide(float currTheta, float nextTheta){
    int numSegments = m_param1;
    float tileHeightDelta = 1.0f/ numSegments;

    for (int i = 0; i < numSegments; i++){
        glm::vec3 topLeft = calculateVertex(currTheta, 0.5f - (tileHeightDelta * i));
        glm::vec3 topRight = calculateVertex(nextTheta, 0.5f - (tileHeightDelta * i));
        glm::vec3 bottomLeft = calculateVertex(currTheta, 0.5f - (tileHeightDelta * (i+1)));
        glm::vec3 bottomRight = calculateVertex(nextTheta, 0.5f - (tileHeightDelta * (i+1)));

        makeTileSide(topLeft, topRight, bottomLeft, bottomRight);
    }

}

void Cylinder::makeBase(float currTheta, float nextTheta){
    int numSegments = m_param1;

    float baseTileDelta = 1.0f/ numSegments;

    for (int i = 0; i < numSegments; i++){
        glm::vec3 topLeft = calculateBaseVertex(currTheta, baseTileDelta * i, 0.5);
        glm::vec3 topRight = calculateBaseVertex(nextTheta, baseTileDelta * i, 0.5);
        glm::vec3 bottomLeft = calculateBaseVertex(currTheta, baseTileDelta * (i+1), 0.5);
        glm::vec3 bottomRight = calculateBaseVertex(nextTheta, baseTileDelta * (i+1), 0.5);

        makeTileBase(topLeft, topRight, bottomLeft, bottomRight);

        glm::vec3 bTopLeft = calculateBaseVertex(currTheta, baseTileDelta * i, -0.5);
        glm::vec3 bTopRight = calculateBaseVertex(nextTheta, baseTileDelta * i, -0.5);
        glm::vec3 bBottomLeft = calculateBaseVertex(currTheta, baseTileDelta * (i+1), -0.5);
        glm::vec3 bBottomRight = calculateBaseVertex(nextTheta, baseTileDelta * (i+1), -0.5);

        makeTileBase(bTopLeft, bTopRight, bBottomLeft, bBottomRight);
    }
}

void Cylinder::setVertexData() {

    m_vertexData.clear();
    float thetaStep = glm::radians(360.0f/ m_param2);

    for (int i = 0; i < m_param2; i++){
        float currTheta = i * thetaStep;
        float nextTheta = (i+1) * thetaStep;

        makeSide(currTheta, nextTheta);
        makeBase(currTheta, nextTheta);
    }
}

glm::vec3 Cylinder::calculateVertex(float currTheta, float height){
    float radius = 0.5;
    float x = radius * sin(currTheta);
    float y = height;
    float z = radius * cos(currTheta);

    return glm::vec3(x,y,z);
}

glm::vec3 Cylinder::calculateBaseVertex(float currTheta, float tileDelta, float z){
    float radius = 0.5 * tileDelta;
    float x = (radius * sin(currTheta));
    float y = z;
    float zed = (radius * cos(currTheta));

    return glm::vec3(x,y,zed);
}

// Inserts a glm::vec3 into a vector of floats.
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
