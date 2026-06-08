
#include <cglm/vec3.h>
#include <ctype.h>
#include <string.h>
#include <cglm/cglm.h>

#include "AssetsLoader/assetsLoader.h"
#include "BackEnd/backend.h"
#include "Common/utility.h"
#include "Render/render.h"


int setOptions(int argc, char** argv);

int main(int argc, char** argv) {
    
    if (!setOptions(argc,argv)) {
        return -1;
    }

    // EngineInit
    WindowHints hints;
    hints.majorVersion = 4;
    hints.minorVersion = 3;
    hints.height = options.height;
    hints.width = options.width;
    strcpy(hints.title, "Window");
    hints.monitor = NULL;
    hints.share = NULL;

    if (!backendInit(&hints)) {
        return -1;
    }

    sceneSetup();

    while (!glfwWindowShouldClose(g_window)) {
        update();
        render();
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

    backendCleanup();
    return 0;
}

int setOptions(int argc, char** argv) {
    options.width = 1920;
    options.height = 1080;
    options.lightCount = 5;
    options.renderLights = 0;
    options.gameObjectCount = 1;
    options.renderMode = FORWARD;
    for (int i  = 0; i < argc; i++) {
        if (!strcmp(argv[i], "--help")) {
            printf("--show-fps to show fps in console.\n");
            return 0;
        }
        if (!strcmp(argv[i], "--show-fps")) {
            options.showFPS = 1;
        }
        if (!strcmp(argv[i], "-rl")) {
            options.renderLights = 1;
        }
        if (!strcmp(argv[i], "-l")) {
            if ( isdigit(argv[i+1][0]) ) {
                char* endptr;
                int count = strtol(argv[i+1], &endptr, 10);
                if (argv[i+1] == endptr) {
                    printf("Invalid value.\n usage: -l <lights count>.\n");
                    return 0;
                }
                options.lightCount = count;
                i++;
            }
        }
        if (!strcmp(argv[i], "-g")) {
            if ( isdigit(argv[i+1][0]) ) {
                char* endptr;
                int count = strtol(argv[i+1], &endptr, 10);
                if (argv[i+1] == endptr) {
                    printf("Invalid value.\n usage: -g <gameObjects count>.\n");
                    return 0;
                }
                options.gameObjectCount = count;
                i++;
            }
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
    return 1;
}
