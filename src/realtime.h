#pragma once
#include "fbo.h"

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

#include <utils/sceneparser.h>
#include <vbo_vao.h>
#include <camera/camera.h>

#include "settings.h"
#include <utils/scenedata.h>
#include "src/ShaderLoader.h"

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    int sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    void genVBOsVAOs();
    void fillVAOs();
    void drawShape(PrimitiveType shapeType);

    void setUpGlobalUniforms();

    struct glShapeInfo{
        PrimitiveType shape;
        int numTriangles;
        GLuint vao;
        vbo_vao *vaoObject;
    };

    std::vector<glShapeInfo> m_shapesRenderInfo;

    std::vector<PrimitiveType> allShapeTypes = {PrimitiveType::PRIMITIVE_CONE,
                                                 PrimitiveType::PRIMITIVE_CUBE,
                                                 PrimitiveType::PRIMITIVE_CYLINDER,
                                                PrimitiveType::PRIMITIVE_SPHERE};

    RenderData m_metaData; // scene data
    // Stores id of shader program
    GLuint m_shader;

    void paintTexture(bool togglePerPixel, bool toggleKernelFilter, bool toggleExtraCredit1, bool toggleExtraCredit2);

    fbo *m_fboObject;
    GLuint m_fullscreen_vao;
    GLuint m_fullscreen_vbo;
    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_defaultFBO = 2;
    GLuint m_fbo_renderbuffer;

    int m_fbo_width;
    int m_fbo_height;

    GLuint m_texture_shader;

    int m_width, m_height;
    Camera m_camera;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;
    int m_screen_width;
    int m_screen_height;

};
