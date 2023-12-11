#pragma once
#include "GL/glew.h"

class fbo
{
public:
    fbo(GLuint &fboTexture, GLuint &fboRenderbuffer, GLuint &fbo, GLuint &defaultFBO, int width, int height);

    void makeFBO();

    void deleteData();

    void generateScreenQuadData(GLuint &vao, GLuint &vbo);

private:
    GLuint *m_FBO;
    GLuint *m_FBO_texture;
    GLuint *m_defaultFBO;
    GLuint *m_FBO_renderbuffer;

    GLuint *m_fullscreenVAO;
    GLuint *m_fullscreenVBO;

    int m_fbo_width;
    int m_fbo_height;
};
