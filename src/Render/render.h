
#ifndef RENDER
#define RENDER

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "../Shader/shader.h"
#include "../Model/model.h"
#include "../Camera/camera.h"
#include "../AssetsLoader/assetsLoader.h"
#include "../BackEnd/backend.h"


void update();
void render();
void renderModel(GameObject* gameObject);
void renderObject(GameObject* gameObject);
void applyMVP(GameObject* gameObject);

int texID;
void render() {

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g_materialsBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, g_lightsBuffer);
    
    int counter = 0;
    // draw objects
    for (int i = 0; i < GAME_OBJECT_COUNT; i++) {
        GameObject gameObject = g_gameObjects[i];

        s_setVec3farr(gameObject.shaderProgram, "viewPosition", g_camera.position);
        texID = i;
        if (gameObject.type >= BACKPACK) {
            renderModel(&gameObject);
        }
        else {
            renderObject(&gameObject);
        }
    }
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g_lightsBuffer);
    for (int i = 0; i < LIGHT_COUNT; i++) {
        GameObject light = g_lightRenderer[i];
        s_setInt(light.shaderProgram, "lightID", i);
        if (light.type >= BACKPACK) {
            renderModel(&light);
        }
        else {
            renderObject(&light);
        }
    }
}

void renderModel(GameObject* gameObject) {
    glUseProgram(gameObject->shaderProgram);
    applyMVP(gameObject);
    Model model = g_models[gameObject->type - BACKPACK];
    drawModel(&model, gameObject->shaderProgram);
}
void renderObject(GameObject* gameObject) {
    glUseProgram(gameObject->shaderProgram);
    applyMVP(gameObject);

    Object obj = g_objects[gameObject->type];
    //printf("VAO: %u, count: %d\n", obj.vao, obj.count);
    glBindVertexArray(obj.vao);
    glDrawArrays(GL_TRIANGLES, 0, obj.count);
}
void applyMVP(GameObject* gameObject) {

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    mat4 projection = GLM_MAT4_IDENTITY_INIT;
    mat4 view = GLM_MAT4_IDENTITY_INIT;

    vec3 front;
    glm_vec3_add(g_camera.position, g_camera.front, front);
    glm_lookat(g_camera.position, front, g_camera.up, view);
    glm_perspective(glm_rad(45), 1, 0.1f, 100.0f, projection);

    // glm_rotate() ...
    glm_translate(model, gameObject->position);
    glm_scale(model, gameObject->scale);

    mat4 vp = GLM_MAT4_IDENTITY_INIT;
    glm_mat4_mul(projection, view, vp);

    glUseProgram(gameObject->shaderProgram);
    s_setMatrix4fv(gameObject->shaderProgram, "vp", 1, GL_FALSE, vp[0]);
    s_setMatrix4fv(gameObject->shaderProgram, "model", 1, GL_FALSE, model[0]);
}
void update() {
    static float lastFrame = 0;
    float deltaTime = glfwGetTime() - lastFrame;
    cameraMove(g_window, &g_camera, deltaTime);
    lastFrame = glfwGetTime();
}

#endif // RENDER
