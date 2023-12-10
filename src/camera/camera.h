#pragma once
#include <utils/scenedata.h>

class Camera
{
public:
    Camera();

    Camera(const SceneCameraData &cameraData, int& width, int& height);
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    // Returns the inverse view matrix for the current camera settings.
    glm::mat4 getInverseViewMatrix() const;

    glm::vec4 getPosVec();

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    static void createProjectionMatrix();

    static void updateProjectionMatrix(int newWidth, int newHeight);

    static void updateViewMatrix();

    static void keyMovement(float translationDistance, bool leftRight, bool look, bool world);

    static void translateCameraPos(float transX, float transY, float transZ, float translationDistance);

    static void rotateAround(float angleDeg, glm::vec3 rotationAxis, bool isYRotation);

};



