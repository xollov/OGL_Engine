
#include <ctype.h>
#include <string.h>
#include <cglm/cglm.h>

#include "AssetsLoader/assetsLoader.h"
#include "BackEnd/backend.h"
#include "Common/utility.h"
#include "Render/render.h"

#include <GLFW/glfw3.h>

struct BenchmarckConf {
    int modelCount;
    int lightCount;
    enum RenderMode renderMode;
    int frameToMeasure;
};


int setOptions(int argc, char** argv);

double runBenchmark(struct BenchmarckConf* conf);
void setupScene(struct BenchmarckConf* conf);

int main(int argc, char** argv) {
    
    if (!setOptions(argc,argv)) {
        return -1;
    }
    // EngineInit
    WindowHints hints;
    hints.majorVersion = 4;
    hints.minorVersion = 3;
    hints.width  = options.width;
    hints.height = options.height;
    strcpy(hints.title, "Window");
    hints.monitor = NULL;
    hints.share = NULL;

    if (!backendInit(&hints)) {
        return -1;
    }

    int modelCount[] = {1, 25, 50, 75, 100};
    int lightCount[] = {1, 8, 16, 64, 100};

    for (int i = 0; i < sizeof(modelCount) / sizeof(int); i++) {
        for (int j = 0; j < sizeof(lightCount) / sizeof(int); j++) {
            // forward
            struct BenchmarckConf conf = {modelCount[i], lightCount[j], FORWARD, 300};
            double res = runBenchmark(&conf);
            printf("FORWARD, model: %d, lights: %d, avg time: %f\n", conf.modelCount, conf.lightCount, res);

            // deferred
            conf.renderMode = DEFERRED;
            res = runBenchmark(&conf);
            printf("DEFERRED, model: %d, lights: %d, avg time: %f\n", conf.modelCount, conf.lightCount, res);
        }
    }

    backendCleanup();
    return 0;
}
double runBenchmark(struct BenchmarckConf* conf) {
   
    setupScene(conf);

    // warm up
    for (int i = 0; i < 50; i++) {
        //update();
        render();
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    } 

    double totalTime = 0;
    for (int i = 0; i < conf->frameToMeasure; i++) {
        double start = glfwGetTime(); 

        render();
        glfwSwapBuffers(g_window);
        glfwPollEvents();
        
        double end = glfwGetTime();
        totalTime += end - start;
    } 
    return (totalTime / conf->frameToMeasure);
}

void setupScene(struct BenchmarckConf* conf) {

    int boxSize = ceil(sqrt(conf->modelCount));
    int gap = 2;
    float offsetX = (boxSize - 1) * gap * 0.5f;
    for (int i = 0; i < conf->modelCount; i++) {
        GameObject* gameObject = &g_gameObjects[i];
        gameObject->type = BACKPACK;
        gameObject->shaderProgram = g_shaders[0];

        gameObject->position[0] = (i % boxSize) * gap - offsetX;
        gameObject->position[1] = 0;
        gameObject->position[2] = -(i / boxSize) * gap;;

        gameObject->rotation[0] = 0;
        gameObject->rotation[1] = 0;
        gameObject->rotation[2] = 0;

        gameObject->scale[0] = 0.6;
        gameObject->scale[1] = 0.6;
        gameObject->scale[2] = 0.6;
    }
    options.gameObjectCount = conf->modelCount;
    options.lightCount = conf->lightCount;
    options.renderMode = conf->renderMode;
}
int setOptions(int argc, char** argv){ 
    options.width = 1920;
    options.height = 1080;
    options.lightCount = 5;
    options.gameObjectCount = 1;
    for (int i  = 0; i < argc; i++) {
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
