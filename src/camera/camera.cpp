#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <settings.h>
#include <iostream>

glm::vec4 m_look;
glm::vec4 m_position;
glm::vec4 m_up;
float m_heightAngle;
float m_aspectRatio;

glm::mat4 m_viewMatrix;
glm::mat4 m_inverseViewMatrix;
glm::mat4 m_perspectiveMatrix;

Camera::Camera(){
}

Camera::Camera(const SceneCameraData &cameraData, int& width, int& height){
    m_look = cameraData.look;
    m_up = cameraData.up;

    m_position = cameraData.pos;
    m_position[3] = 1.0f;

    m_heightAngle = cameraData.heightAngle;
    m_aspectRatio = static_cast<float>(width)/static_cast<float>(height);

    m_viewMatrix = glm::lookAt(glm::vec3(m_position), glm::vec3(m_look), glm::vec3(m_up));
    m_inverseViewMatrix = glm::inverse(m_viewMatrix);

    createProjectionMatrix();
}

glm::mat4 Camera::getViewMatrix() const {
    return m_viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() const {
    return m_perspectiveMatrix;
}

glm::mat4 Camera::getInverseViewMatrix() const{
    return m_inverseViewMatrix;
}

glm::vec4 Camera::getPosVec(){
    return m_position;
}

float Camera::getAspectRatio() const {
    return m_aspectRatio;
}

float Camera::getHeightAngle() const {
    return m_heightAngle;
}

// if near and/or far plane change, recalculate the projection
// matrix
void Camera::createProjectionMatrix() {
    float widthAngle = 2.0f * glm::atan(m_aspectRatio * glm::tan(m_heightAngle / 2.0f));

    float scale_x = 1.f / (settings.farPlane * glm::tan(widthAngle / 2.f));
    float scale_y = 1.f / (settings.farPlane * glm::tan(m_heightAngle / 2.f));
    float scale_z = 1.f / settings.farPlane;

    float c = -settings.nearPlane / settings.farPlane;

    // the 'unhinging' transformation matrix to map
    // the perspective to parallel (parralelization matrix)
    glm::mat4 m_pp = {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, (1.f/(1.f + c)), -1.f,
        0.f, 0.f, -(c/(1.f+c)), 0.f
    };


    glm::mat4 mapToGlZ = {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, -2.f, 0.f,
        0.f, 0.f, -1.f, 1.f
    };

    glm::mat4 pers = {
        scale_x, 0.f, 0.f, 0.f,
        0.f, scale_y, 0.f, 0.f,
        0.f, 0.f, scale_z, 0.f,
        0.f, 0.f, 0.f, 1.f
    };

    m_perspectiveMatrix = mapToGlZ * m_pp * pers;
}

void Camera::updateProjectionMatrix(int newWidth, int newHeight){
    m_aspectRatio = static_cast<float>(newWidth)/static_cast<float>(newHeight);
    createProjectionMatrix();
}

void Camera::updateViewMatrix(){
    m_viewMatrix = glm::lookAt(glm::vec3(m_position), glm::vec3(m_look + m_position), glm::vec3(m_up));
    m_inverseViewMatrix = glm::inverse(m_viewMatrix);
}

void Camera::keyMovement(float translationDistance, bool leftRight, bool look, bool world){
    float transX = 0.f;
    float transY = 0.f;
    float transZ = 0.f;

    if(leftRight){
        glm::vec3 rightVec = glm::cross(glm::vec3(m_look), glm::vec3(m_up));
        transX = rightVec.x;
        transY = rightVec.y;
        transZ = rightVec.z;
    } else if(look){
        transX = m_look.x;
        transY = m_look.y;
        transZ = m_look.z;
    } else if(world){
        transX = 0.f;
        transY = 1.f;
        transZ = 0.f;
    }

    translateCameraPos(transX, transY, transZ, translationDistance);

    updateViewMatrix();
}

void Camera::translateCameraPos(float transX, float transY, float transZ, float translationDistance){

    glm::mat4 transMatrix = glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        transX * translationDistance, transY * translationDistance, transZ * translationDistance, 1.0f
        );

    // Apply the translation to both position and look-at point
    m_position = transMatrix * m_position;
    m_look = transMatrix * m_look;
}

void Camera::rotateAround(float angleDeg, glm::vec3 rotationAxis, bool isYRotation) {
    float radians = glm::radians(angleDeg);
    float cosTheta = cos(radians);
    float sinTheta = sin(radians);
    glm::vec3 rightVec = glm::cross(glm::vec3(m_look), glm::vec3(m_up));
    if(isYRotation) rotationAxis = rightVec;

    float x1 = cosTheta + pow(rotationAxis.x, 2.f)*(1.f - cosTheta);
    float y1 = rotationAxis.x * rotationAxis.y * (1.f - cosTheta) + (rotationAxis.z*sinTheta);
    float z1 = rotationAxis.x * rotationAxis.z * (1.f - cosTheta) - (rotationAxis.y*sinTheta);

    float x2 = rotationAxis.x * rotationAxis.y * (1.f - cosTheta) - (rotationAxis.z*sinTheta);
    float y2 = cosTheta + pow(rotationAxis.y, 2.f)*(1.f - cosTheta);
    float z2 = rotationAxis.y * rotationAxis.z * (1.f - cosTheta) + (rotationAxis.x*sinTheta);

    float x3 = rotationAxis.x * rotationAxis.z * (1.f - cosTheta) + (rotationAxis.y*sinTheta);
    float y3 = rotationAxis.y * rotationAxis.z * (1.f - cosTheta) - (rotationAxis.x*sinTheta);
    float z3 = cosTheta + pow(rotationAxis.z, 2.f)*(1.f - cosTheta);

    glm::mat3 rotationMatrix = glm::mat3(
        x1, y1, z1,
        x2, y2, z2,
        x3, y3, z3
        );

//    rotationMatrix = glm::transpose(rotationMatrix);

    // Apply the rotation matrix to the camera's look-at point and up vector
    m_look = glm::vec4((rotationMatrix * glm::vec3(m_look)), 0.f);
    m_up = glm::vec4((rotationMatrix * glm::vec3(m_up)), 0.f);

    // Update the view matrix
    updateViewMatrix();
}
