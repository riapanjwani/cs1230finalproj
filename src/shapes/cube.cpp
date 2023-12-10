#include "cube.h"

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Creates a tile (i.e. 2 triangles) using the 4 given points.
    glm::vec3 normalTopLeft = glm::normalize(glm::cross(bottomLeft - topLeft, bottomRight - topLeft));
    glm::vec3 normalTopLeft2 = glm::normalize(glm::cross(bottomRight - topLeft, topRight - topLeft));
    glm::vec3 normalBottomRight = glm::normalize(glm::cross(topLeft - bottomRight, bottomLeft - bottomRight));
    glm::vec3 normalBottomRight2 = glm::normalize(glm::cross(topRight - bottomRight, topLeft - bottomRight));
    glm::vec3 normalBottomLeft = glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft-bottomLeft));
    glm::vec3 normalTopRight = glm::normalize(glm::cross(topLeft- topRight, bottomRight - topRight));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalBottomLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft2);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight2);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalTopRight);
}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // creates a single side of the cube out of the 4
    //         given points and makeTile()
    int numTiles = m_param1;

    //for the x faces, for the left and right faces
    if ((topLeft[0] == 0.5 || topLeft[0] == -0.5)
        && (topRight[0] == 0.5 || topRight[0] == -0.5)
        && (bottomLeft[0] == 0.5 || bottomLeft[0] == -0.5)
        && (bottomRight[0] == 0.5 || bottomRight[0] == -0.5)){

        float incrementX =  (topRight[1] - topLeft[1])/numTiles;
        float incrementY = (bottomLeft[2] - topLeft[2])/numTiles;

        for (int i = 0; i < numTiles; i++){
            for (int j = 0; j < numTiles; j++){
                glm::vec3 tileTopLeft = topLeft + glm::vec3(0.0f, incrementX * i, incrementY * j);
                glm::vec3 tileTopRight = tileTopLeft + glm::vec3(0.0f, incrementX, 0.0f);
                glm::vec3 tileBottomLeft = tileTopLeft + glm::vec3 (0.0f, 0.0f, incrementY);
                glm::vec3 tileBottomRight = tileTopRight + glm::vec3(0.0f, 0.0f, incrementY);

                makeTile(tileTopLeft, tileTopRight, tileBottomLeft, tileBottomRight);
            }
        }
    }

    //the y faces, top and bottom
    if ((topLeft[1] == 0.5 || topLeft[1] == -0.5)
        && (topRight[1] == 0.5 || topRight[1] == -0.5)
        && (bottomLeft[1] == 0.5 || bottomLeft[1] == -0.5)
        && (bottomRight[1] == 0.5 || bottomRight[1] == -0.5)){

        float stepX = (topRight[0] - topLeft[0])/numTiles;
        float stepY = (bottomLeft[2] - topLeft[2])/numTiles;

        for (int i = 0; i < numTiles; i++){
            for (int j = 0; j < numTiles; j++){
                glm::vec3 tileTopLeft = topLeft + glm::vec3(stepX * i, 0.0f, stepY * j);
                glm::vec3 tileTopRight = tileTopLeft + glm::vec3(stepX, 0.0f,0.0f);
                glm::vec3 tileBottomLeft = tileTopLeft + glm::vec3(0.0f, 0.0f, stepY);
                glm::vec3 tileBottomRight = tileTopRight + glm::vec3(0.0f, 0.0f, stepY);

                makeTile(tileTopLeft, tileTopRight, tileBottomLeft, tileBottomRight);
            }
        }
    }


    // for the z values
    if ((topLeft[2] == 0.5 || topLeft[2] == -0.5)
        && (topRight[2] == 0.5 || topRight[2] == -0.5)
        && (bottomLeft[2] == 0.5 || bottomLeft[2] == -0.5)
        && (bottomRight[2] == 0.5 || bottomRight[2] == -0.5)){

        float stepX = (topRight[0] - topLeft[0])/numTiles;
        float stepY = (bottomLeft[1] - topLeft[1])/numTiles;

        for(int i = 0; i< numTiles; i++){
            for (int j = 0; j<numTiles; j++){
                glm::vec3 tileTopLeft = topLeft + glm::vec3(stepX * i, stepY * j, 0.0f);
                glm::vec3 tileTopRight = tileTopLeft + glm::vec3(stepX, 0.0f, 0.0f);
                glm::vec3 tileBottomLeft = tileTopLeft + glm::vec3(0.0f, stepY, 0.0f);
                glm::vec3 tileBottomRight = tileTopRight + glm::vec3(0.0f, stepY, 0.0f);

                makeTile(tileTopLeft, tileTopRight, tileBottomLeft, tileBottomRight);
            }
        }
    }


}

void Cube::setVertexData() {
    m_vertexData = std::vector<float>();

    //
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f));

    // Back face (z = -0.5)
    makeFace(
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3( 0.5f, -0.5f, -0.5f),

        glm::vec3(-0.5f,  0.5f, -0.5f),
        glm::vec3( 0.5f,  0.5f, -0.5f)

        );

    // Left face (x = -0.5)
    makeFace(
        glm::vec3(-0.5f, 0.5f, 0.5f),
        glm::vec3(-0.5f, -0.5f,  0.5f),
        glm::vec3(-0.5f, 0.5f,  -0.5f),
        glm::vec3(-0.5f, - 0.5f, -0.5f)

        );

    // Top face (y = -.5)
    makeFace(glm::vec3(-0.5f,  -0.5f,  0.5f),
             glm::vec3(0.5f,  -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f,  -0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f));

    // Right face (x = .5)
    makeFace(glm::vec3(0.5f, -0.5f, 0.5f),
             glm::vec3(0.5f, 0.5f, 0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f),
             glm::vec3(0.5f, 0.5f, -0.5f));

    // Bottom face (y = 0.5)
    makeFace(glm::vec3(0.5f, 0.5f, 0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3(0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, -0.5f));

}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
