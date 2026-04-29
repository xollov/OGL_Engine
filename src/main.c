
#include <cglm/vec3.h>
#include <string.h>
#include <cglm/cglm.h>

#include "AssetsLoader/assetsLoader.h"
#include "BackEnd/backend.h"
#include "Render/render.h"


int SCR_WIDTH = 1920;
int SCR_HEIGHT = 1080;

// Note(): setting glfw_resizable to false or floating to true makes  widnow shrink into a line
int main(int argc, char** argv) {
    
    for (int i  = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            printf("--show-fps to show fps in console.\n");
            return 0;
        }
        if (!strcmp(argv[i], "--show-fps")) {
            options.showFPS = 1;
        }
    }

    // EngineInit
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

    while (!glfwWindowShouldClose(g_window)) {
        update();

        // frame begin()
        render();
        // frame end()

        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

    backendCleanup();
    return 0;
}

