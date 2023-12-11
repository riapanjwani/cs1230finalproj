#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>


/**
 * A helper function to traverse the scene graph. Applying each node's
 * transformation to the inputted CTM. As well as populating the renderData
 * struct with the primitives/shapes and lights to be rendered.
 *
 * @param node: the node from which to ascend the tree.
 * @param ctm: the cumulative transformation matrix to be calculated
 * @param renderData: the
 */
void traverseSceneGraph(const SceneNode* node, glm::mat4 ctm, RenderData &renderData) {

    glm::mat4 rotation;

    //for each transformation in the node's list of transformations
    for (SceneTransformation* transformation : node->transformations){
        switch (transformation->type){
        case TransformationType::TRANSFORMATION_TRANSLATE:
            ctm = glm::translate(ctm, transformation->translate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            ctm = glm::scale(ctm, transformation->scale);
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            rotation = glm::rotate(glm::mat4(1.0f), transformation->angle, transformation->rotate);
            ctm = ctm * rotation;
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            ctm = ctm*transformation->matrix;
            break;
        }
    }

    // construct a RenderShapeData object for each primitive type
    // and append to the list of shapes to be renders
    for (ScenePrimitive* primitive : node->primitives){
        RenderShapeData shapeData;
        shapeData.primitive = *primitive;
        shapeData.ctm = ctm;
        renderData.shapes.push_back(shapeData);
    }

    // For each light in the node's light list, creates a
    // sceneLightData object and updates it's feilds and appends
    // to the list of lights to be rendered.
    // note: pos & dir are in world space
    for (SceneLight* light : node->lights){
        SceneLightData lightData;
        lightData.type = light->type;
        lightData.angle = light->angle;
        lightData.color = light->color;
        lightData.width = light->width;
        lightData.function = light->function;
        lightData.height = light->height;
        lightData.penumbra = light->penumbra;
        lightData.id = light->id;

        lightData.dir = ctm *  light->dir ;
        lightData.pos = ctm * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        renderData.lights.push_back(lightData);
    }

    // Recusively traverses the graph
    for (SceneNode* child : node->children){
        traverseSceneGraph(child, ctm, renderData);
    }

}




/**
 * A function to parse the scene graph. Creates a file reader to read the JSON scene
 * graph. Creates an identity matrix for the CTM to be passed into the helper
 * function that traverses the graph and updates the relevant feilds.
 *
 * @param filepath: the filepath of the scenegraph representing the scene
 * @param renderData: the struct that stores the data needed to render the scene
 *
 * @returns bool: if the traversal was sucessful/the file is readable
 */
bool SceneParser::parse(std::string filepath, RenderData &renderData) {

    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    // populate renderData's list of primitives and their transforms.
    const SceneNode* rootNode = fileReader.getRootNode();
    renderData.shapes.clear();
    renderData.lights.clear();
    glm::mat4 ctm(1.0f);
    traverseSceneGraph(rootNode, ctm, renderData);

    return true;
}
