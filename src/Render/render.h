#ifndef RENDER
#define RENDER

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Shader/shader.h"
#include "../Model/model.h"
#include "../Camera/camera.h"
#include "../AssetsLoader/assetsLoader.h"
#include "../BackEnd/backend.h"


void update();
void render();
void drawGBufferScene();
void drawForwardScene();

void renderModel(GameObject* gameObject);
void renderObject(GameObject* gameObject);
void applyMVP(GameObject* gameObject);

extern GLuint vis_mode;
extern GLuint renderMode;
void render() {
    GLuint query;
    glGenQueries(1, &query);
    glBeginQuery(GL_TIME_ELAPSED, query);

    if (renderMode) {
        drawForwardScene();
    } else {
        drawGBufferScene();
    }

    glEndQuery(GL_TIME_ELAPSED);
    GLuint64 time;
    glGetQueryObjectui64v(query, GL_QUERY_RESULT, &time);
    //printf("Time taken: %lu\n", time);
}

void drawGBufferScene() {
    const GLfloat one[] = {1, 1, 1, 1};
    int width, height;
    int id;
    glfwGetWindowSize(g_window, &width, &height);

    // first pass
    glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);
    glEnable(GL_DEPTH_TEST); 
    glViewport(0,0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint program = gbufferShader;
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_textures[0]);

    /* 
    // Early z for heavy fragment computation 
     
    // Disable color writes, enable depth writes
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    renderModel(&g_gameObjects[0]);
    
    // Enable color writes
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_FALSE); // depth already written
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    */
    renderModel(&g_gameObjects[0]);
    
    // second pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0, width, height);
    glDisable(GL_DEPTH_TEST); 
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT);

    program = gbufferRenderShader;
    glUseProgram(program);

    // uniforms
    glUniform1ui(glGetUniformLocation(program, "lightCount"), 2);
    glUniform1ui(glGetUniformLocation(program, "matID"), 10);
    glUniform3fv(glGetUniformLocation(program, "viewPosition"),3, g_camera.position);

    //ssbo
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g_materialsBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, g_lightsBuffer);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gbuffer_tex[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gbuffer_tex[1]);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); 
    
    // clean attachment
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawForwardScene() {
    const GLfloat one[] = {1, 1, 1, 1};
    int width, height;
    int id;
    glfwGetWindowSize(g_window, &width, &height);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0, width, height);
    glEnable(GL_DEPTH_TEST); 
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint program = g_shaders[SHADER_MODEL];
    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_textures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_textures[1]);

    // uniforms
    glUniform1ui(glGetUniformLocation(program, "lightCount"), 2);
    glUniform1ui(glGetUniformLocation(program, "matID"), 10);
    glUniform3fv(glGetUniformLocation(program, "viewPosition"),3, g_camera.position);

    //ssbo
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g_materialsBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, g_lightsBuffer);

    renderModel(&g_gameObjects[0]);
    
    // clean attachment
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void renderModel(GameObject* gameObject) {
    applyMVP(gameObject);
    Model model = g_models[gameObject->type - BACKPACK];
    drawModel(&model, gameObject->shaderProgram);
}
void renderObject(GameObject* gameObject) {
    applyMVP(gameObject);
    Object obj = g_objects[gameObject->type];
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

    glm_translate(model, gameObject->position);
    glm_scale(model, gameObject->scale);

    mat4 vp = GLM_MAT4_IDENTITY_INIT;
    glm_mat4_mul(projection, view, vp);

    s_setMatrix4fv(gameObject->shaderProgram, "vp", 1, GL_FALSE, vp[0]);
    s_setMatrix4fv(gameObject->shaderProgram, "model", 1, GL_FALSE, model[0]);
}

void update() {
    static float lastFrame = 0;
    float deltaTime = glfwGetTime() - lastFrame;
    cameraMove(g_window, &g_camera, deltaTime);
    lastFrame = glfwGetTime();
    
    if (options.showFPS) {
        printf("FPS: %f\n", 1 / deltaTime); 
    }
}

#endif // RENDER
