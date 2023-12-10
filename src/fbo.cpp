#include "fbo.h"
#include <vector>

fbo::fbo(GLuint &fboTexture, GLuint &fboRenderbuffer, GLuint &fbo, GLuint &defaultFBO, int width, int height){
    m_FBO_texture = &fboTexture;
    m_FBO_renderbuffer = &fboRenderbuffer;
    m_FBO = &fbo;
    m_defaultFBO = &defaultFBO;

    m_fbo_width = width;
    m_fbo_height = height;
}

void fbo::makeFBO(){
    // Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1, m_FBO_texture);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, *m_FBO_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, m_FBO_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, *m_FBO_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Generate and bind an FBO
    glGenFramebuffers(1, m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, *m_FBO);

    // Add the texture as a color attachment, and the renderbuffer as a depth+stencil attachment, to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *m_FBO_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *m_FBO_renderbuffer);

    // Unbind FBO
    glBindFramebuffer(GL_FRAMEBUFFER, *m_defaultFBO);

}

void fbo::generateScreenQuadData(GLuint &vao, GLuint &vbo) {

    std::vector<GLfloat> fullscreen_quad_data = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };

    m_fullscreenVAO = &vao;
    m_fullscreenVBO = &vbo;

    glGenBuffers(1, m_fullscreenVBO);
    glBindBuffer(GL_ARRAY_BUFFER, *m_fullscreenVBO);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat),
                 fullscreen_quad_data.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, m_fullscreenVAO);
    glBindVertexArray(*m_fullscreenVAO);

    // enable attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    //clean up bindings
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void fbo::deleteData(){
    glDeleteFramebuffers(1, m_FBO);
    glDeleteRenderbuffers(1, m_FBO_renderbuffer);
    glDeleteTextures(1, m_FBO_texture);
}
