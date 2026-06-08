
#ifndef BACKEND
#define BACKEND

#include <cglm/cglm.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "../Model/model.h"
#include "../Camera/camera.h"
#include "../AssetsLoader/assetsLoader.h"

typedef struct {
    int majorVersion ;
    int minorVersion ;
    int width;
    int height;
    char title[64];
    GLFWmonitor *monitor;
    GLFWwindow *share;

} WindowHints;

GLFWwindow* g_window; 
GLuint vis_mode = 0;
GLuint renderMode = 0;

int backendInit(WindowHints *hints);
int windowInit(WindowHints *hints);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursorCallback(GLFWwindow* window, double xpos, double ypos);
void backendCleanup();

int backendInit(WindowHints *hints) {

    if (!windowInit(hints)) {
        return 0;
    }
   
    cameraSetup();

    loadShaders();
    loadAssets();
    loadGameObjects();
    loadMaterials();
    lightsSetup();
    loadFramebuffers();

    glEnable(GL_DEPTH_TEST);
    return 1;
}

void backendCleanup() {
    cleanGameObjects();
    cleanAssets();
    cleanMaterial();
    cleanLights();
    cleanShaders();
    deleteBuffers();
    glfwTerminate();
}

int windowInit(WindowHints *hints) {
    
    if (!glfwInit()) {
        return 0;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, hints->majorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, hints->minorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    g_window = glfwCreateWindow(hints->width, hints->height, (const char*) hints->title, hints->monitor, hints->share);
    if (g_window == NULL) {
        fprintf(stderr, "glfwCreateWindow() failed.\n");
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(g_window);
    glfwSwapInterval( 0 );
    glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "gladLoadGLLoader() failed.\n");
        glfwTerminate();
        return 0;
    }
    glViewport(0,0, hints->width, hints->height);

    glfwSetKeyCallback(g_window, keycallback);
    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(g_window, cursorCallback);
    return 1;
}

void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    static int linemode = 1;
    static int cursor = 1;
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, (linemode = !linemode) ? GL_FILL : GL_LINE);
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE); 
    }   

    if(key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, (cursor = !cursor) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
    if(key >= GLFW_KEY_0 && key <= GLFW_KEY_9 && action == GLFW_PRESS) {
        vis_mode = key - GLFW_KEY_0;
    }
    if(key == GLFW_KEY_R && action == GLFW_PRESS) {
        options.renderMode = FORWARD;
    }
    if(key == GLFW_KEY_T && action == GLFW_PRESS) {
        options.renderMode = DEFERRED;
    }
}

void cursorCallback(GLFWwindow* window, double xpos, double ypos) {
    cameraRotate(&g_camera, xpos, ypos);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    static int i = 0;
    glViewport(0, 0, width, height);
    printf("%d.Changing size\n", i++);
}
#endif //BACKEND
