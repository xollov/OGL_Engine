#include "assetsLoader.h"

GameObject* g_gameObjects;
void loadGameObjects() {
    g_gameObjects = malloc(sizeof(GameObject) * GAME_OBJECT_COUNT);
    if (!g_gameObjects) {
        fprintf(stderr, "ERROR: coudn't locate memory for gameObjects\n");
    }
}

void sceneSetup() {
    int boxSize = ceil(sqrt(options.gameObjectCount));
    int gap = 2;
    float offsetX = (boxSize - 1) * gap * 0.5f;
    for (int i = 0; i < GAME_OBJECT_COUNT; i++) {
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
}
void lightsSetup() {

    g_lights = malloc(sizeof(Light) * LIGHT_COUNT);

    if (g_lights == NULL) {
        fprintf(stderr, "ERROR: coudn't allocate memory for lights!\n");
    }

    g_lightRenderer = malloc(sizeof(GameObject) * LIGHT_COUNT);
    if (g_lightRenderer == NULL) {
        fprintf(stderr, "ERROR: coudn't locate memory for light renderer\n");
    }

    int boxSize = ceil(sqrt(LIGHT_COUNT));
    int gap = 2;
    float offsetX = (boxSize - 1) * gap * 0.5f;

    for (int i = 0; i < LIGHT_COUNT; i++) {
        Light* light = &g_lights[i];

        light->position[0] = (i % boxSize) * gap - offsetX;
        light->position[1] = 3;
        light->position[2] = -(i / boxSize) * gap;;
        light->position[3] = 0;

        light->ambient[0] = 0.1f; 
        light->ambient[1] = 0.1f; 
        light->ambient[2] = 0.1f; 
        light->ambient[3] = 0; 

        light->diffuse[0] = 0.5f; 
        light->diffuse[1] = 0.5f; 
        light->diffuse[2] = 0.5f; 
        light->diffuse[3] = 0; 

        light->specular[0] = 1; 
        light->specular[1] = 1; 
        light->specular[2] = 1; 
        light->specular[3] = 0; 
    }

    for (int i = 0; i < LIGHT_COUNT; i++) {
        Light* light = &g_lights[i];
        GameObject* gameObject = &g_lightRenderer[i];
        gameObject->type = CUBE;
        glm_vec3_copy(light->position, gameObject->position);
        gameObject->shaderProgram = g_shaders[SHADER_LIGHT_SOURCE];
        glm_vec3_copy(GLM_VEC3_ZERO, gameObject->rotation);
        glm_vec3_copy(GLM_VEC3_ONE, gameObject->scale);
    }
    // light ssbo
    glGenBuffers(1, &g_lightsBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_lightsBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Light) * LIGHT_COUNT, g_lights, GL_STATIC_DRAW);
}

void cleanGameObjects() {
    free(g_gameObjects);
}
