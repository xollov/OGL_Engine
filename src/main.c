
#include <cglm/vec3.h>
#include <string.h>
#include <cglm/cglm.h>

#include "AssetsLoader/assetsLoader.h"
#include "BackEnd/backend.h"
#include "Camera/camera.h"
#include "Common/utility.h"
#include "Render/render.h"

int SCR_WIDTH = 1920;
int SCR_HEIGHT = 1080;

// Note(): setting glfw_resizable to false or floating to true makes  widnow shrink into a line
int main() {
    

    WindowHints hints;
    hints.majorVersion = 4;
    hints.minorVersion = 3;
    hints.height = SCR_HEIGHT;
    hints.width  = SCR_WIDTH;
    strcpy(hints.title, "Window");
    hints.monitor = NULL;
    hints.share = NULL;

    if (!backendInit(&hints)) {
        return -1;
    }
   
    cameraSetup();
    shadersSetup();
    materialSetup();
    lightsSetup();
    loadAssets();
    gameObjectsSetup();

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(g_window)) {

        glClear(GL_DEPTH_BUFFER_BIT);
        glClearBufferfv(GL_COLOR, 0, colors[0]);

        update();
        render();

        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

    // TODO: cleanup models, objects, shader programs etc.
    cleanGameObjects();
    cleanAssets();
    cleanMaterial();
    cleanShaders();
    deleteBuffers();
    glfwTerminate();
    return 0;
}

