
#include <cglm/vec3.h>
#include <ctype.h>
#include <string.h>
#include <cglm/cglm.h>

#include "AssetsLoader/assetsLoader.h"
#include "BackEnd/backend.h"
#include "Common/utility.h"
#include "Render/render.h"


int SCR_WIDTH = 1920;
int SCR_HEIGHT = 1080;

// Note(): setting glfw_resizable to false or floating to true makes  widnow shrink into a line
int main(int argc, char** argv) {
    
    options.width = 1920;
    options.height = 1080;
    for (int i  = 0; i < argc; i++) {
        if (!strcmp(argv[i], "--help")) {
            printf("--show-fps to show fps in console.\n");
            return 0;
        }
        if (!strcmp(argv[i], "--show-fps")) {
            options.showFPS = 1;
        }
        if (!strcmp(argv[i], "-w")) {
            if ( isdigit(argv[i+1][0]) ) {
                char* endptr;
                int width = strtol(argv[i+1], &endptr, 10);
                if (argv[i+1] == endptr) {
                    printf("Invalid value.\n usage: -w <integer value>.\n");
                    return 0;
                }
                options.width = width;
                i++;
            }
            else {
                printf("-w <integer value>.\n");
                return 0;
            }
        }
        if (!strcmp(argv[i], "-h")) {
            if ( isdigit(argv[i+1][0]) ) {
                char* endptr;
                int height = strtol(argv[i+1], &endptr, 10);
                if (argv[i+1] == endptr) {
                    printf("Invalid value.\n usage: -h <integer value>.\n");
                    return 0;
                }
                options.height = height;
                i++;
            }
            else {
                printf("-h <integer value>.\n");
                return 0;
            }
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

