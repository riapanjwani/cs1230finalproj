#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Clean up VBO and VAO memory
    for(int i = 0; i < allShapeTypes.size(); i++){
        m_shapesRenderInfo[i].vaoObject->deleteData();
    }

    m_fboObject->deleteData();

    glDeleteProgram(m_shader);
    glDeleteProgram(m_texture_shader);

    delete(m_fboObject);

    this->doneCurrent();
}

/**
 * @brief Realtime::setUpVBOsVAOs sets up the VBOs and VAOs,
 *                                storing their IDs for each
 *                                shape.
 */
void Realtime::genVBOsVAOs(){

    for(int i = 0; i < allShapeTypes.size(); i++){
        // push back first then access the element, and fill out element's feilds
        glShapeInfo currShapeInfo;
        m_shapesRenderInfo.push_back(currShapeInfo);
        GLuint *vao;
        m_shapesRenderInfo[i].vao = *vao;
        m_shapesRenderInfo[i].shape = static_cast<PrimitiveType>(i);
        m_shapesRenderInfo[i].vaoObject = new vbo_vao(m_shapesRenderInfo[i].vao);
    }

}

void Realtime::fillVAOs(){

    for(glShapeInfo &shapeInfoObj : m_shapesRenderInfo) {

        for(RenderShapeData &shape : m_metaData.shapes) {

            if(shape.primitive.type == shapeInfoObj.shape){
                shapeInfoObj.numTriangles = shapeInfoObj.vaoObject->fillArray(shapeInfoObj.shape,
                                                                              settings.shapeParameter1, settings.shapeParameter2,
                                                                              m_newPositions, false);
                break;
            }

        }

    }

}


void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_defaultFBO;

    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    glClearColor(0.73f, 0.9f, 0.95f, 1.0f); // set the clear color (sky blue)

    // set up shaders
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert", ":/resources/shaders/texture.frag");

    m_newPositions = {0.0f};

    // generate VAOs for each type of shape
    genVBOsVAOs();

    // generate vbo and vao for fullscreen quad data
    m_fboObject = new fbo(m_fbo_texture, m_fbo_renderbuffer, m_fbo, m_defaultFBO, m_fbo_width, m_fbo_height);
    m_fboObject->generateScreenQuadData(m_fullscreen_vao, m_fullscreen_vbo);

    m_fboObject->makeFBO();

}

void Realtime::paintTexture(bool togglePerPixel, bool toggleKernelFilter, bool toggleExtraCredit1, bool toggleExtraCredit2){
    glUseProgram(m_texture_shader);

    // Set bool uniforms on whether or not to filter the texture drawn
    auto perPixelBool  = glGetUniformLocation(m_texture_shader, "m_doFilterPerPixel");
    glUniform1i(perPixelBool, togglePerPixel);
    auto kernelBoolLoc  = glGetUniformLocation(m_texture_shader, "m_doKernelFilter");
    glUniform1i(kernelBoolLoc, toggleKernelFilter);
    auto extraCredit1Loc  = glGetUniformLocation(m_texture_shader, "m_doExtraCredit1");
    glUniform1i(extraCredit1Loc, toggleExtraCredit1);
    auto extraCredit2Loc  = glGetUniformLocation(m_texture_shader, "m_doExtraCredit2");
    glUniform1i(extraCredit2Loc, toggleExtraCredit2);

    // Send scene width and height as uniforms
    auto widthLoc = glGetUniformLocation(m_texture_shader, "m_scene_width");
    glUniform1i(widthLoc, m_fbo_width);
    auto heightLoc  = glGetUniformLocation(m_texture_shader, "m_scene_height");
    glUniform1i(heightLoc, m_fbo_height);

    glBindVertexArray(m_fullscreen_vao);
    // Bind "texture" to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    glUseProgram(0);
}


void Realtime::drawShape(PrimitiveType shapeType){

    for(glShapeInfo &shapeInfoObj : m_shapesRenderInfo) {
        if (shapeType == shapeInfoObj.shape){
            shapeInfoObj.vaoObject->draw(shapeInfoObj.numTriangles, shapeInfoObj.vao);
            break;
        }
    }

}

void Realtime::setUpGlobalUniforms(){
    // pass in view and projection matrices as uniforms
    GLint viewLocation = glGetUniformLocation(m_shader, "m_view");
    glm::mat4 view = m_camera.getViewMatrix();
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
    glm::mat4 projection = m_camera.getProjectionMatrix();
    GLint projLocation = glGetUniformLocation(m_shader, "m_proj");
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

    // pass in camera position as a uniform for the frag shader
    GLint cameraPosLocation = glGetUniformLocation(m_shader, "m_cameraWorldPos");
    glm::vec4 camWorldPos = m_camera.getPosVec();
    glUniform4fv(cameraPosLocation, 1, &camWorldPos[0]);

    // pass in global Scene data as uniforms
    GLint kaLocation = glGetUniformLocation(m_shader, "m_ka");
    float ka = m_metaData.globalData.ka;
    glUniform1f(kaLocation, ka);

    GLint kdLocation = glGetUniformLocation(m_shader, "m_kd");
    float kd = m_metaData.globalData.kd;
    glUniform1f(kdLocation, kd);

    GLint ksLocation = glGetUniformLocation(m_shader, "m_ks");
    float ks = m_metaData.globalData.ks;
    glUniform1f(ksLocation, ks);

    // for each light pass in their properties
    for (int i = 0; i < m_metaData.lights.size(); i++) {
        std::string typeName = "lights[" + std::to_string(i) + "]" + ".type";
        GLint typeLoc = glGetUniformLocation(m_shader, &typeName[0]);
        int intLightType = static_cast<int>(m_metaData.lights[i].type);
        glUniform1i(typeLoc, intLightType);

        std::string colorName = "lights[" + std::to_string(i) + "]" + ".color";
        GLint colorLoc = glGetUniformLocation(m_shader, &colorName[0]);
        glm::vec4 lightColor = m_metaData.lights[i].color;
        glUniform4fv(colorLoc, 1, &lightColor[0]);

        std::string attName = "lights[" + std::to_string(i) + "]"  + ".attenuation";
        GLint attFunctionLoc = glGetUniformLocation(m_shader, &attName[0]);
        glm::vec3 attFunction = m_metaData.lights[i].function;
        glUniform3fv(attFunctionLoc, 1, &attFunction[0]);

        std::string posName = "lights[" + std::to_string(i) + "]" + ".pos";
        GLint posLoc = glGetUniformLocation(m_shader, &posName[0]);
        glm::vec4 lightPos = m_metaData.lights[i].pos;
        glUniform4fv(posLoc, 1, &lightPos[0]);

        std::string dirName = "lights[" + std::to_string(i) + "]" + ".dir";
        GLint dirLoc = glGetUniformLocation(m_shader, &dirName[0]);
        glm::vec4 lightDir = m_metaData.lights[i].dir;
        glUniform4fv(dirLoc, 1, &lightDir[0]);

        std::string penumbraName = "lights[" + std::to_string(i) + "]" + ".penumbra";
        GLint penumbraLoc = glGetUniformLocation(m_shader, &penumbraName[0]);
        float penumbra = m_metaData.lights[i].penumbra;
        glUniform1f(penumbraLoc, penumbra);

        std::string angleName = "lights[" + std::to_string(i) + "]" + ".angle";
        GLint angleLoc = glGetUniformLocation(m_shader, &angleName[0]);
        float angle = m_metaData.lights[i].angle;
        glUniform1f(angleLoc, angle);
    }

    // pass in number of lights as uniform
    int num_lights = m_metaData.lights.size();
    glUniform1i(glGetUniformLocation(m_shader, "num_lights"), num_lights);
}


void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_fbo_width, m_fbo_height);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind the shader
    glUseProgram(m_shader);

    setUpGlobalUniforms();

    // for each shape, bind VAO, pass in shape-specific uniforms,s
    // and then draw with shaders, unbind VAO
    for(RenderShapeData &shape : m_metaData.shapes){

        glm::mat4 model = shape.ctm;
        // pass in the model (ctm) of this shape as a uniform into the shader program
        GLint modelLocation = glGetUniformLocation(m_shader, "m_model");
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);

        // pass in Shape material attributes as uniforms
        glm::vec4 ambientColor = shape.primitive.material.cAmbient;
        glUniform4fv(glGetUniformLocation(m_shader, "material_cAmbient"), 1, &ambientColor[0]);
        glm::vec4 diffuseColor = shape.primitive.material.cDiffuse;
        glUniform4fv(glGetUniformLocation(m_shader, "material_cDiffuse"), 1, &diffuseColor[0]);
        glm::vec4 specularColor = shape.primitive.material.cSpecular;
        glUniform4fv(glGetUniformLocation(m_shader, "material_cSpecular"), 1, &specularColor[0]);
        float shininess = shape.primitive.material.shininess;
        glUniform1f(glGetUniformLocation(m_shader, "material_shininess"), shininess);
        glm::vec4 reflectiveColor = shape.primitive.material.cReflective;
        glUniform4fv(glGetUniformLocation(m_shader, "material_cReflective"), 1, &reflectiveColor[0]);

        // Bind VAO & Draw Command
        drawShape(shape.primitive.type);
    }

    // Unbind the shader
    glUseProgram(0);

    // Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0, 0, m_fbo_width, m_fbo_height);

    // draw FBO color attachment texture
    paintTexture(settings.perPixelFilter, settings.kernelBasedFilter, settings.extraCredit1, settings.extraCredit2);
}

void Realtime::resizeGL(int w, int h) {
    // Delete Texture, Renderbuffer, and Framebuffer memory
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    m_width = settings.width;
    m_height = settings.height;
    m_camera = Camera(m_metaData.cameraData, m_width, m_height);

    // Delete Texture, Renderbuffer, and Framebuffer memory
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;

    // Regenerate FBOs
    m_fboObject = new fbo(m_fbo_texture, m_fbo_renderbuffer, m_fbo, m_defaultFBO, m_fbo_width, m_fbo_height);
    m_fboObject->generateScreenQuadData(m_fullscreen_vao, m_fullscreen_vbo);
    m_fboObject->makeFBO();
}

int Realtime::sceneChanged() {
    RenderData renderData;
    bool success = SceneParser::parse(settings.sceneFilePath, renderData);
    m_metaData = renderData;

    if (!success) {
        std::cerr << "Error loading scene: \"" << settings.sceneFilePath << "\"" << std::endl;
        return 1;
    }

    m_width = settings.width;
    m_height = settings.height;

    m_camera = Camera(m_metaData.cameraData, m_width, m_height);
    fillVAOs();

    update(); // asks for a PaintGL() call to occur
    return 0;
}

void Realtime::settingsChanged() {
    fillVAOs();
    m_camera.updateProjectionMatrix(m_width, m_height);
    update(); // asks for a PaintGL() call to occur
}

// ================== Mesh Cloth Functions


void Realtime::updateClothWithForce(){
    for(glShapeInfo &shapeInfoObj : m_shapesRenderInfo) {

        if(shapeInfoObj.shape == PrimitiveType::PRIMITIVE_CLOTH){
            shapeInfoObj.numTriangles = shapeInfoObj.vaoObject->fillArray(shapeInfoObj.shape,
                                                                          settings.shapeParameter1, settings.shapeParameter2, m_newPositions, true);
            break;
        }

    }
}


//makes sense that this is identical because applied universally to each particle
//this is assuming that y is up!!! which i believe is true here
glm::vec3 Realtime::calculateGravity(){
    m_mass = 1.0f;
    glm::vec3 gravityForce = {0.0f, -m_gravity* m_mass, 0.0f};
    return gravityForce;
}

/**
 * @brief This is the force acting on a particle with a given spring at p&q with stiffness K, and rest length
 * //is there an appropriate stiffness, rest length etc, to use?
 * @return
 */
glm::vec3 Realtime::calculateSpringForce(){
    glm::vec3 springForce;
    return springForce;
}

glm::vec3 Realtime::calculateDampingForce(){
    glm::vec3 dampingForce;
    return dampingForce;
}

// ================== Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }

}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
    m_mouseDown = false;
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        m_camera.rotateAround(deltaX * 0.2f, glm::vec3(0.f, 1.f, 0.f), false);
        m_camera.rotateAround(deltaY * 0.01f, glm::vec3(0.f, 1.f, 0.f), true);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    if(m_keyMap[Qt::Key_G] == true){

        Cloth::Particle p1;
        Cloth::Particle p2;
        Cloth::Particle p3;
        Cloth::Particle p4;
        Cloth::Particle p5;
        Cloth::Particle p6;
        Cloth::Particle p7;
        Cloth::Particle p8;
        Cloth::Particle p9;

        for(glShapeInfo &shapeInfoObj : m_shapesRenderInfo) {

            if(shapeInfoObj.shape == PrimitiveType::PRIMITIVE_CLOTH){ // kind confused by this bc I thought we deleted each object after filling the arrays?
                // commented out the delete call in va0_vbo for the cloth for now, see line 41 there
                p1 = shapeInfoObj.vaoObject->m_cloth->getParticle1();
                p2 = shapeInfoObj.vaoObject->m_cloth->getParticle2();
                p3 = shapeInfoObj.vaoObject->m_cloth->getParticle3();
                p4 = shapeInfoObj.vaoObject->m_cloth->getParticle4();
                p5 = shapeInfoObj.vaoObject->m_cloth->getParticle5();
                p6 = shapeInfoObj.vaoObject->m_cloth->getParticle6();
                p7 = shapeInfoObj.vaoObject->m_cloth->getParticle7();
                p8 = shapeInfoObj.vaoObject->m_cloth->getParticle8();
                p9 = shapeInfoObj.vaoObject->m_cloth->getParticle9();

                m_particles.push_back(p1);
                m_particles.push_back(p2);
                m_particles.push_back(p3);
                m_particles.push_back(p4);
                m_particles.push_back(p5);
                m_particles.push_back(p6);
                m_particles.push_back(p7);
                m_particles.push_back(p8);
                m_particles.push_back(p9);

                break;
            }

        }

        glm::vec3 acceleration = calculateGravity();


        p1.acceleration = acceleration;
        p2.acceleration = acceleration ;
        p3.acceleration = acceleration;
        p4.acceleration = acceleration ;
        p5.acceleration = acceleration ;
        p6.acceleration = acceleration ;
        p7.acceleration = acceleration ;
        p8.acceleration = acceleration ;
        p9.acceleration = acceleration ;

        float springHoriz = 0.325f;
        float springVert = 0.45f;


        m_structuralSprings.clear();
        Spring spring14;
        spring14.particle1 = &p1;
        spring14.particle2 = &p4;
        spring14.restLength = springVert;
        m_structuralSprings.push_back(spring14);
        float k = 1.0f; //elasticity
        float p_q = glm::length(spring14.particle2->position - spring14.particle1->position);
        // glm::vec3 springForce = spring14.restLength > 0.0f ? k *(spring14.restLength - p_q) * glm::normalize(spring14.particle2->position - spring14.particle1->position) : glm::vec3(0.0f);
        // springForce = springForce ;
        // p1.position += springForce;
        // p4.position += (-springForce);
        glm::vec3 difference = spring14.particle2->position - spring14.particle1->position;
        glm::vec3 correctionVector = difference * (1.0f - spring14.restLength/p_q);
        glm::vec3 correctionVectorHalf = correctionVector*0.5f;
        //p1.position += correctionVectorHalf;
        p4.position += (-correctionVectorHalf);

        Spring spring45;
        spring45.particle1 = &p4;
        spring45.particle2 = &p5;
        spring45.restLength = springHoriz;
        m_structuralSprings.push_back(spring45);
        p_q = glm::length(spring45.particle2->position - spring45.particle1->position);
        // springForce = spring45.restLength > 0.0f ? k *(spring45.restLength - p_q) * glm::normalize(spring45.particle2->position - spring45.particle1->position) : glm::vec3(0.0f);
        // p4.position += springForce;
        // p5.position += (-springForce);
        difference = spring45.particle2->position - spring45.particle1->position;
        correctionVector = difference * (1.0f - spring45.restLength/p_q);
        correctionVectorHalf = correctionVector*0.5f;
        p4.position += correctionVectorHalf;
        p5.position += (-correctionVectorHalf);

        Spring spring25;
        spring25.particle1 = &p2;
        spring25.particle2 = &p5;
        spring25.restLength = springVert;
        m_structuralSprings.push_back(spring25);
        p_q = glm::length(spring25.particle2->position - spring25.particle1->position);
        // springForce = spring25.restLength > 0.0f ? k *(spring25.restLength - p_q) * glm::normalize(spring25.particle2->position - spring25.particle1->position) : glm::vec3(0.0f);
        // p2.position += springForce;
        // p5.position += (-springForce);
        difference = spring25.particle2->position - spring25.particle1->position;
        correctionVector = difference * (1.0f - spring25.restLength/p_q);
        correctionVectorHalf = correctionVector*0.5f;
        p2.position += correctionVectorHalf;
        p5.position += (-correctionVectorHalf);

        Spring spring12;
        spring12.particle1 = &p1;
        spring12.particle2 = &p2;
        spring12.restLength = springHoriz;
        m_structuralSprings.push_back(spring12);
        p_q = glm::length(spring12.particle2->position - spring12.particle1->position);
        // springForce = spring12.restLength > 0.0f ? k *(spring12.restLength - p_q) * glm::normalize(spring12.particle2->position - spring12.particle1->position) : glm::vec3(0.0f);
        // p1.position += springForce;
        // p2.position += (-springForce);
        difference = spring12.particle2->position - spring12.particle1->position;
        correctionVector = difference * (1.0f - spring12.restLength/p_q);
        correctionVectorHalf = correctionVector*0.5f;
        //p1.position += correctionVectorHalf;
        p2.position += (-correctionVectorHalf);

        Spring spring47;
        spring47.particle1 = &p4;
        spring47.particle2 = &p7;
        spring47.restLength = springVert;
        m_structuralSprings.push_back(spring47);
        p_q = glm::length(spring47.particle2->position - spring47.particle1->position);
        // springForce = spring47.restLength > 0.0f ? k *(spring47.restLength - p_q) * glm::normalize(spring47.particle2->position - spring47.particle1->position) : glm::vec3(0.0f);
        // p4.position += springForce;
        // p7.position += (-springForce);
        difference = spring47.particle2->position - spring47.particle1->position;
        correctionVector = difference * (1.0f - spring47.restLength/p_q);
        correctionVectorHalf = correctionVector*0.5f;
        p4.position += correctionVectorHalf;
        p7.position += (-correctionVectorHalf);

        Spring spring78;
        spring78.particle1 = &p7;
        spring78.particle2 = &p8;
        spring78.restLength = springHoriz;
        m_structuralSprings.push_back(spring78);
        p_q = glm::length(spring78.particle2->position - spring78.particle1->position);
        // springForce = spring78.restLength > 0.0f ? k *(spring78.restLength - p_q) * glm::normalize(spring78.particle2->position - spring78.particle1->position) : glm::vec3(0.0f);
        // p7.position += springForce;
        // p8.position += (-springForce);
        difference = spring78.particle2->position - spring78.particle1->position;
        correctionVector = difference * (1.0f - spring78.restLength/p_q);
        correctionVectorHalf = correctionVector*0.5f;
        p7.position += correctionVectorHalf;
        p8.position += (-correctionVectorHalf);

        Spring spring58;
        spring58.particle1 = &p5;
        spring58.particle2 = &p8;
        spring58.restLength = springVert;
        m_structuralSprings.push_back(spring58);
        p_q = glm::length(spring58.particle2->position - spring58.particle1->position);
        // springForce = spring58.restLength > 0.0f ? k *(spring58.restLength - p_q) * glm::normalize(spring58.particle2->position - spring58.particle1->position) : glm::vec3(0.0f);
        // p5.position += springForce;
        // p8.position += (-springForce);
        difference = spring58.particle2->position - spring58.particle1->position;
        correctionVector = difference * (1.0f - spring58.restLength/p_q);
        correctionVectorHalf = correctionVector*0.5f;
        p5.position += correctionVectorHalf;
        p8.position += (-correctionVectorHalf);

        Spring spring56;
        spring56.particle1 = &p5;
        spring56.particle2 = &p6;
        spring56.restLength =springHoriz;
        m_structuralSprings.push_back(spring56);
        p_q = glm::length(spring56.particle2->position - spring56.particle1->position);
        // springForce = spring56.restLength > 0.0f ? k *(spring56.restLength - p_q) * glm::normalize(spring56.particle2->position - spring56.particle1->position) : glm::vec3(0.0f);
        // p5.position += springForce;
        // p6.position += (-springForce);
        difference = spring56.particle2->position - spring56.particle1->position;
        correctionVector = difference * (1.0f - spring56.restLength/p_q);
        correctionVectorHalf = correctionVector*0.5f;
        p5.position += correctionVectorHalf;
        p6.position += (-correctionVectorHalf);

        Spring spring36;
        spring36.particle1 = &p3;
        spring36.particle2 = &p6;
        spring36.restLength = springVert;
        m_structuralSprings.push_back(spring36);
        p_q = glm::length(spring36.particle2->position - spring36.particle1->position);
        // springForce = spring36.restLength > 0.0f ? k *(spring36.restLength - p_q) * glm::normalize(spring36.particle2->position - spring36.particle1->position) : glm::vec3(0.0f);
        // p3.position += springForce;
        // p6.position += (-springForce);
        difference = spring36.particle2->position - spring36.particle1->position;
        correctionVector = difference * (1.0f - spring36.restLength/p_q);
        correctionVectorHalf = correctionVector*0.5f;
        //p3.position += correctionVectorHalf;
        p6.position += (-correctionVectorHalf);

        Spring spring23;
        spring23.particle1 = &p2;
        spring23.particle2 = &p3;
        spring23.restLength = springHoriz;
        m_structuralSprings.push_back(spring23);
        p_q = glm::length(spring23.particle2->position - spring23.particle1->position);
        // springForce = spring23.restLength > 0.0f ? k *(spring23.restLength - p_q) * glm::normalize(spring23.particle2->position - spring23.particle1->position) : glm::vec3(0.0f);
        // p2.position += springForce ;
        // p3.position += (-springForce);
        difference = spring23.particle2->position - spring23.particle1->position;
        correctionVector = difference * (1.0f - spring23.restLength/p_q);
        correctionVectorHalf = correctionVector*0.5f;
        p2.position += correctionVectorHalf;
        //p3.position += (-correctionVectorHalf);

        Spring spring89;
        spring89.particle1 = &p8;
        spring89.particle2 = &p9;
        spring89.restLength = springHoriz;
        m_structuralSprings.push_back(spring89);
        p_q = glm::length(spring89.particle2->position - spring89.particle1->position);
        // springForce = spring89.restLength > 0.0f ? k *(spring89.restLength - p_q) * glm::normalize(spring89.particle2->position - spring89.particle1->position) : glm::vec3(0.0f);
        // p8.position += springForce;
        // p9.position += (-springForce);
        difference = spring89.particle2->position - spring89.particle1->position;
        correctionVector = difference * (1.0f - spring89.restLength/p_q);
        correctionVectorHalf = correctionVector*0.5f;
        p8.position += correctionVectorHalf;
        p9.position += (-correctionVectorHalf);

        Spring spring69;
        spring69.particle1 = &p6;
        spring69.particle2 = &p9;
        spring69.restLength = springVert;
        m_structuralSprings.push_back(spring69);
        p_q = glm::length(spring69.particle2->position - spring69.particle1->position);
        // springForce = spring69.restLength > 0.0f ? k *(spring69.restLength - p_q) * glm::normalize(spring69.particle2->position - spring69.particle1->position) : glm::vec3(0.0f);
        // p6.position += springForce ;
        // p9.position += (-springForce);
        difference = spring69.particle2->position - spring69.particle1->position;
        correctionVector = difference* (1.0f - spring69.restLength/p_q);
        correctionVectorHalf = correctionVector*0.5f;
        p6.position += correctionVectorHalf;
        p9.position += (-correctionVectorHalf);
        // Spring spring14;
        // spring14.particle1 = &p1;
        // spring14.particle2 = &p4;
        // spring14.restLength = springVert;
        // Spring spring45;
        // spring45.particle1 = &p4;
        // spring45.particle2 = &p5;
        // spring45.restLength = springHoriz;
        // Spring spring25;
        // spring25.particle1 = &p2;
        // spring25.particle2 = &p5;
        // spring25.restLength = springVert;
        // Spring spring12;
        // spring12.particle1 = &p1;
        // spring12.particle2 = &p2;
        // spring12.restLength = springHoriz;
        // Spring spring47;
        // spring47.particle1 = &p4;
        // spring47.particle2 = &p7;
        // spring47.restLength = springVert;
        // Spring spring78;
        // spring78.particle1 = &p7;
        // spring78.particle1 = &p8;
        // spring78.restLength = springHoriz;
        // Spring spring58;
        // spring58.particle1 = &p5;
        // spring58.particle2 = &p8;
        // spring58.restLength = springVert;
        // Spring spring56;
        // spring56.particle1 = &p5;
        // spring56.particle2 = &p6;
        // spring56.restLength =springHoriz;
        // Spring spring36;
        // spring36.particle1 = &p3;
        // spring36.particle2 = &p6;
        // spring36.restLength = springVert;
        // Spring spring23;
        // spring23.particle1 = &p2;
        // spring23.particle2 = &p3;
        // spring23.restLength = springHoriz;
        // Spring spring89;
        // spring89.particle1 = &p8;
        // spring89.particle2 = &p9;
        // spring89.restLength = springHoriz;
        // Spring spring69;
        // spring69.particle1 = &p6;
        // spring69.particle2 = &p9;
        // spring69.restLength = springVert;

        //verlet integration equation here!!!
        glm::vec3 p2newPos = {2.0f * p2.position[0] - p2.oldPosition[0] + p2.acceleration[0] * (deltaTime * deltaTime),
                              2.0f * p2.position[1] - p2.oldPosition[1] + p2.acceleration[1] * (deltaTime * deltaTime),
                              p2.position[2]};

        glm::vec3 p4newPos = {2.0f * p4.position[0] - p4.oldPosition[0] + p4.acceleration[0] * (deltaTime * deltaTime),
                              2.0f * p4.position[1] - p4.oldPosition[1] + p4.acceleration[1] * (deltaTime * deltaTime),
                              p4.position[2]};

        glm::vec3 p5newPos = {2.0f * p5.position[0] - p5.oldPosition[0]+ p5.acceleration[0] * (deltaTime * deltaTime),
                              2.0f * p5.position[1] - p5.oldPosition[1] + p5.acceleration[1] * (deltaTime * deltaTime),
                              p5.position[2]};

        glm::vec3 p6newPos = {2.0f * p6.position[0] - p6.oldPosition[0] + p6.acceleration[0] * (deltaTime * deltaTime),
                              2.0f * p6.position[1] - p6.oldPosition[1] + p6.acceleration[1] * (deltaTime * deltaTime),
                              p6.position[2]};

        glm::vec3 p7newPos = {2.0f * p7.position[0] - p7.oldPosition[0] + p7.acceleration[0] * (deltaTime * deltaTime),
                              2.0f * p7.position[1] - p7.oldPosition[1] +  p7.acceleration[1] * (deltaTime * deltaTime),
                              p7.position[2]};

        glm::vec3 p8newPos = {2.0f * p8.position[0] - p8.oldPosition[0] + p8.acceleration[0] * (deltaTime * deltaTime),
                              2.0f * p8.position[1] - p8.oldPosition[1] + p8.acceleration[1] * (deltaTime * deltaTime),
                              p8.position[2]};

        glm::vec3 p9newPos = {2.0f * p9.position[0] - p9.oldPosition[0] + p9.acceleration[0] * (deltaTime * deltaTime),
                              2.0f * p9.position[1] - p9.oldPosition[1] + p9.acceleration[1] * (deltaTime * deltaTime),
                              p9.position[2]};

        // glm::vec3 p2newPos = {2.0f * p2.position[0] - p2.position[0] + acceleration[0] * (deltaTime * deltaTime),
        //                       2.0f * p2.position[1] - p2.oldPosition[1] + acceleration[1] * (deltaTime * deltaTime),
        //                       p2.position[2]};

        // glm::vec3 p4newPos = {2.0f * p4.position[0] - p4.position[0] + acceleration[0] * (deltaTime * deltaTime),
        //                       2.0f * p4.position[1] - p4.oldPosition[1] + acceleration[1] * (deltaTime * deltaTime),
        //                       p4.position[2]};

        // glm::vec3 p5newPos = {2.0f * p5.position[0] - p5.position[0] + acceleration[0] * (deltaTime * deltaTime),
        //                       2.0f * p5.position[1] - p5.oldPosition[1] + acceleration[1] * (deltaTime * deltaTime),
        //                       p5.position[2]};

        // glm::vec3 p6newPos = {2.0f * p6.position[0] - p6.position[0] + acceleration[0] * (deltaTime * deltaTime),
        //                       2.0f * p6.position[1] - p6.oldPosition[1] + acceleration[1] * (deltaTime * deltaTime),
        //                       p6.position[2]};

        // glm::vec3 p7newPos = {2.0f * p7.position[0] - p7.position[0] + acceleration[0] * (deltaTime * deltaTime),
        //                       2.0f * p7.position[1] - p7.oldPosition[1] + acceleration[1] * (deltaTime * deltaTime),
        //                       p7.position[2]};

        // glm::vec3 p8newPos = {2.0f * p8.position[0] - p8.position[0] + acceleration[0] * (deltaTime * deltaTime),
        //                       2.0f * p8.position[1] - p8.oldPosition[1] + acceleration[1] * (deltaTime * deltaTime),
        //                       p8.position[2]};

        // glm::vec3 p9newPos = {2.0f * p9.position[0] - p9.position[0] + acceleration[0] * (deltaTime * deltaTime),
        //                       2.0f * p9.position[1] - p9.oldPosition[1] + acceleration[1] * (deltaTime * deltaTime),
        //                       p9.position[2]};

        m_newPositions.clear();
        insertVec3(m_newPositions, p2newPos);
        insertVec3(m_newPositions, p4newPos);
        insertVec3(m_newPositions, p5newPos);
        insertVec3(m_newPositions, p6newPos);
        insertVec3(m_newPositions, p7newPos);
        insertVec3(m_newPositions, p8newPos);
        insertVec3(m_newPositions, p9newPos);

        fillVAOs();
        updateClothWithForce();
        update();
    }

    // Use deltaTime and m_keyMap to move around
    if(m_keyMap[Qt::Key_W]){
        //m_camera.keyMovement(deltaTime, false, true, false);


    } else if(m_keyMap[Qt::Key_S]){
        m_camera.keyMovement(-deltaTime, false, true, false);
    } else if(m_keyMap[Qt::Key_A]){
        m_camera.keyMovement(-deltaTime, true, false, false);
    } else if(m_keyMap[Qt::Key_D]){
        m_camera.keyMovement(deltaTime, true, false, false);
    } else if(m_keyMap[Qt::Key_Space]){
        m_camera.keyMovement(deltaTime, false, false, true);
    } else if(m_keyMap[Qt::Key_Control]){
        m_camera.keyMovement(-deltaTime, false, false, true);
    }

    update(); // asks for a PaintGL() call to occur
}

void Realtime::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
