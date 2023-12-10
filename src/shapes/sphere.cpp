#include "sphere.h"

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = (param1 > 1) ? param1 : 2;
    m_param2 = (param2 > 1) ? param2 : 2;
    setVertexData();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    glm::vec3 normalTopLeft = glm::normalize(topLeft - glm::vec3(0.0f,0.0f,0.0f));
    glm::vec3 normalTopRight = glm::normalize(topRight - glm::vec3(0.0f,0.0f,0.0f));
    glm::vec3 normalBottomLeft = glm::normalize(bottomLeft - glm::vec3(0.0f,0.0f,0.0f));
    glm::vec3 normalBottomRight = glm::normalize(bottomRight- glm::vec3(0.0f,0.0f,0.0f));

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

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    // Creates a single wedge of the sphere using the
    //         makeTile() function.

    // param 1 = number of segments (lat division)
    // param 2 = number of wedges (long division)

    int numSegments = m_param1;
    //bc 180 is half vertically, divide by # segments to get diff in how many segments
    float angleIncrement = glm::radians(180.f / numSegments);

    for (int i= 0; i< numSegments; i++){
        //get the first angle increment - the first longitude angle
        float phi1 = i * angleIncrement;
        //then get the angle after - double of the last to get the rest
        float phi2 = (i+1) * angleIncrement;

        //this is to loop thru the longitude angles, and get the one before and after
        //by looping thru to see what num segmenet of current there where at * the size of the current wedge, didvided by the total
        for (int j = 0; j< m_param2; j++){
            float theta1 = currentTheta +j *(nextTheta -currentTheta)/ m_param2;
            float theta2 = currentTheta + (j+1) * (nextTheta - currentTheta) / m_param2;

            // sphere has a radius of 0.5, so use that for r calculation
            glm::vec3 topLeft = glm::vec3 (
                0.5f * glm::sin(phi1) * sin(theta1),
                0.5f * glm::cos(phi1),
                0.5f * glm::sin(phi1) * cos(theta1)
                );

            glm::vec3 topRight = glm::vec3 (
                0.5f * glm::sin(phi1) * glm::sin(theta2),
                0.5f * glm::cos(phi1),
                0.5f * glm::sin(phi1) * glm::cos(theta2)
                );

            glm::vec3 bottomLeft = glm::vec3 (
                0.5f * glm::sin(phi2) * glm::sin(theta1),
                0.5f * glm::cos(phi2),
                0.5f * glm::sin(phi2) * glm::cos(theta1)
                );

            glm::vec3 bottomRight = glm::vec3(
                0.5f * glm::sin(phi2) * glm::sin(theta2),
                0.5f * glm::cos(phi2),
                0.5f * glm::sin(phi2) * glm::cos(theta2)
                );


            makeTile(topLeft, topRight, bottomLeft, bottomRight);
        }



    }
}

void Sphere::makeSphere() {
    // Creates a full sphere using the makeWedge() function
    float thetaIncrement = glm::radians(360.f / m_param2);

    for (int i = 0; i < m_param2; i++) {
        float currentTheta = i * thetaIncrement;
        float nextTheta = (i + 1) * thetaIncrement;

        makeWedge(currentTheta, nextTheta);
    }
}

void Sphere::setVertexData() {

    makeSphere();
    // add to VBO?
}

// Inserts a glm::vec3 into a vector of floats.
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
