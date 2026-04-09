
#ifndef BACKEND
#define BACKEND

#include <cglm/cglm.h>
#include <cglm/util.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <malloc.h>
#include "../Model/model.h"
#include "../Camera/camera.h"

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

int backendInit(WindowHints *hints);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursorCallback(GLFWwindow* window, double xpos, double ypos);


int backendInit(WindowHints *hints) {
    
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, hints->majorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, hints->minorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    g_window = glfwCreateWindow(hints->width, hints->height, (const char*) hints->title, hints->monitor, hints->share);
    if (g_window == NULL) {
        fprintf(stderr, "glfwCreateWindow() failed.\n");
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(g_window);
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
