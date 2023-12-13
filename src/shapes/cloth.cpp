#include "cloth.h"

Cloth::Cloth() { // fill vertexData with world positions
    pp1 = {-0.425, 0.8, -0.6495};
    pp2 = {-0.1, 0.8, -0.6495};
    pp3 = {0.225, 0.8, -0.6495};
    pp4 = {-.425, 0.35, -0.6495};
    pp5 = {-0.1, 0.35, -0.6495};
    pp6 = {0.225, 0.35, -0.6495};
    pp7 = {-0.425, -0.1, -0.6495};
    pp8 = {-0.1, -0.1, -0.6495};
    pp9 = {0.225, -0.1, -0.6495};
    p1.oldPosition = pp1;
    p1.position = pp1;
    p2.oldPosition = pp2;
    p2.position = pp2;
    p3.oldPosition = pp3;
    p3.position = pp3;
    p4.oldPosition = pp4;
    p4.position = pp4;
    p5.oldPosition = pp5;
    p5.position = pp5;
    p6.oldPosition = pp6;
    p6.position = pp6;
    p7.oldPosition = pp7;
    p7.position = pp7;
    p8.oldPosition = pp8;
    p8.position = pp8;
    p9.oldPosition = pp9;
    p9.position = pp9;

    setVertexData();
}


//change this, so that it takes in all the new positions to set, and then calls set vertex data
void Cloth::updateParams(std::vector<float> newPositions) {
    glm::vec3 p2new = {newPositions[0], newPositions[1], newPositions[2]};
    glm::vec3 p4new = {newPositions[3], newPositions[4], newPositions[5]};
    glm::vec3 p5new = {newPositions[6], newPositions[7], newPositions[8]};
    glm::vec3 p6new = {newPositions[9], newPositions[10], newPositions[11]};
    glm::vec3 p7new = {newPositions[12], newPositions[13], newPositions[14]};
    glm::vec3 p8new = {newPositions[15], newPositions[16], newPositions[17]};
    glm::vec3 p9new = {newPositions[18], newPositions[19], newPositions[20]};
    updateParticle2(p2new);
    updateParticle4(p4new);
    updateParticle5(p5new);
    updateParticle6(p6new);
    updateParticle7(p7new);
    updateParticle8(p8new);
    updateParticle9(p9new);
    setVertexData();
}


void Cloth::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight, int counter) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    //should be 4 normals bc 4 given vertices
    glm::vec3 normalTopLeft = glm::normalize(glm::cross(bottomLeft - topLeft, bottomRight - topLeft));
    glm::vec3 normalTopLeft2 = glm::normalize(glm::cross(bottomRight - topLeft, topRight - topLeft));
    glm::vec3 normalBottomRight = glm::normalize(glm::cross(topLeft - bottomRight, bottomLeft - bottomRight));
    glm::vec3 normalBottomRight2 = glm::normalize(glm::cross(topRight - bottomRight, topLeft - bottomRight));
    //testing to see if above are the same, but never prints

    glm::vec3 normalBottomLeft = glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft-bottomLeft));
    glm::vec3 normalTopRight = glm::normalize(glm::cross(topLeft- topRight, bottomRight - topRight));


    if(counter == 0){
        insertVec3(m_vertexData, p1.position);
        insertVec3(m_vertexData, normalTopLeft);
        insertVec3(m_vertexData, p4.position);
        insertVec3(m_vertexData, normalBottomLeft);
        insertVec3(m_vertexData, p5.position);
        insertVec3(m_vertexData, normalBottomRight);

        insertVec3(m_vertexData, p1.position);
        insertVec3(m_vertexData, normalTopLeft2);
        insertVec3(m_vertexData, p5.position);
        insertVec3(m_vertexData, normalBottomRight2);
        insertVec3(m_vertexData, p2.position);
        insertVec3(m_vertexData, normalTopRight);
    }

    if(counter == 1){
        insertVec3(m_vertexData, p4.position);
        insertVec3(m_vertexData, normalTopLeft);
        insertVec3(m_vertexData, p7.position);
        insertVec3(m_vertexData, normalBottomLeft);
        insertVec3(m_vertexData, p8.position);
        insertVec3(m_vertexData, normalBottomRight);

        insertVec3(m_vertexData, p4.position);
        insertVec3(m_vertexData, normalTopLeft2);
        insertVec3(m_vertexData, p8.position);
        insertVec3(m_vertexData, normalBottomRight2);
        insertVec3(m_vertexData, p5.position);
        insertVec3(m_vertexData, normalTopRight);
    }

    if(counter == 2){
        insertVec3(m_vertexData, p2.position);
        insertVec3(m_vertexData, normalTopLeft);
        insertVec3(m_vertexData, p5.position);
        insertVec3(m_vertexData, normalBottomLeft);
        insertVec3(m_vertexData, p6.position);
        insertVec3(m_vertexData, normalBottomRight);

        insertVec3(m_vertexData, p2.position);
        insertVec3(m_vertexData, normalTopLeft2);
        insertVec3(m_vertexData, p6.position);
        insertVec3(m_vertexData, normalBottomRight2);
        insertVec3(m_vertexData, p3.position);
        insertVec3(m_vertexData, normalTopRight);
    }
    if(counter ==3){
        insertVec3(m_vertexData, p5.position);
        insertVec3(m_vertexData, normalTopLeft);
        insertVec3(m_vertexData, p8.position);
        insertVec3(m_vertexData, normalBottomLeft);
        insertVec3(m_vertexData, p9.position);
        insertVec3(m_vertexData, normalBottomRight);

        insertVec3(m_vertexData, p5.position);
        insertVec3(m_vertexData, normalTopLeft2);
        insertVec3(m_vertexData, p9.position);
        insertVec3(m_vertexData, normalBottomRight2);
        insertVec3(m_vertexData, p6.position);
        insertVec3(m_vertexData, normalTopRight);
    }

}

void Cloth::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    int numTiles = 2;

    //for the z values all the same, meaning it is the top and bottom face
    if ((topLeft[2] == 0.5 || topLeft[2] == -0.5)
        && (topRight[2] == 0.5 || topRight[2] == -0.5)
        && (bottomLeft[2] == 0.5 || bottomLeft[2] == -0.5)
        && (bottomRight[2] == 0.5 || bottomRight[2] == -0.5)){

        float stepX = (topRight[0] - topLeft[0])/numTiles;
        float stepY = (bottomLeft[1] - topLeft[1])/numTiles;

        int counter = 0;
        for(int i = 0; i< numTiles; i++){
            for (int j = 0; j<numTiles; j++){
                glm::vec3 tileTopLeft = topLeft + glm::vec3(stepX * i, stepY * j, 0.0f);
                glm::vec3 tileTopRight = tileTopLeft + glm::vec3(stepX, 0.0f, 0.0f);
                glm::vec3 tileBottomLeft = tileTopLeft + glm::vec3(0.0f, stepY, 0.0f);
                glm::vec3 tileBottomRight = tileTopRight + glm::vec3(0.0f, stepY, 0.0f);
                makeTile(tileTopLeft, tileTopRight, tileBottomLeft, tileBottomRight, counter);
                counter += 1;
            }
        }
    }

}

void Cloth::setVertexData() {
    m_vertexData.clear();

    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f));

}

// Inserts a glm::vec3 into a vector of floats.
void Cloth::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}


Cloth::Particle Cloth::getParticle1(){
    return p1;
}

void Cloth::updateParticle2(glm::vec3 newP2pos){
    p2.oldPosition = p2.position;
    p2.position = newP2pos;
}

Cloth::Particle Cloth::getParticle2(){
    return p2;
}

Cloth::Particle Cloth::getParticle3(){
    return p3;
}

void Cloth::updateParticle4(glm::vec3 newP4pos){
    p4.oldPosition = p4.position;
    p4.position = newP4pos;
}

Cloth::Particle Cloth::getParticle4(){
    return p4;
}

void Cloth::updateParticle5(glm::vec3 newP5pos){
    p5.oldPosition = p5.position;
    p5.position = newP5pos;
}

Cloth::Particle Cloth::getParticle5(){
    return p5;
}

void Cloth::updateParticle6(glm::vec3 newP6pos){
    p6.oldPosition = p6.position;
    p6.position = newP6pos;
}


Cloth::Particle Cloth::getParticle6(){
    return p6;
}

void Cloth::updateParticle7(glm::vec3 newP7pos){
    p7.oldPosition = p7.position;
    p7.position = newP7pos;
}

Cloth::Particle Cloth::getParticle7(){
    return p7;
}

void Cloth::updateParticle8(glm::vec3 newP8pos){
    p8.oldPosition = p8.position;
    p8.position = newP8pos;
}

Cloth::Particle Cloth::getParticle8(){
    return p8;
}

void Cloth::updateParticle9(glm::vec3 newP9pos){
    p9.oldPosition = p9.position;
    p9.position = newP9pos;
}

Cloth::Particle Cloth::getParticle9(){
    return p9;
}

