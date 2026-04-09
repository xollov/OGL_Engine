#ifndef ASSETS_LOADER
#define ASSETS_LOADER

#include <cglm/cglm.h>
#include <cglm/vec3.h>
#include <glad/glad.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "../Common/objects.h"
#include "../Model/model.h"

typedef struct {
    GLuint vao, vbo, ebo, count;
} Object;

typedef struct {
    GLuint type, shaderProgram;
    vec3 position, scale, rotation;
} GameObject;

typedef struct {
    vec4 diffuse, specular;
    float shininess;
}Material;

typedef struct {
    vec4 position, ambient, diffuse, specular;
}Light;

Model* g_models;
Object* g_objects;
GameObject* g_gameObjects;
GLuint* g_shaders;

Material* g_materials;
GLuint g_materialsBuffer;

GameObject* g_lightRenderer;
Light* g_lights;
GLuint g_lightsBuffer;

#define MODEL_COUNT     1
#define SHADER_COUNT    3
#define MATERIAL_COUNT 23
#define LIGHT_COUNT 2 
#define OBJECT_COUNT    4 

#define GAME_OBJECT_COUNT 3 

enum ObjectType {
    CUBE = 0,
    PLANE,
    QUAD,
    SKYBOX,
};
enum ModelType {
    BACKPACK = OBJECT_COUNT,
    DRAGON,
};
enum MetarialType {
    BRONZE
};

void shadersSetup();
void gameObjectsSetup();
void loadAssets();
void materialSetup();
void loadMaterialValues();
void parseMaterials(Material* material);

void cleanGameObjects();
void cleanAssets();
void cleanMaterial();
void cleanShaders();
void deleteBuffers();

GLuint g_textures[3];
void shadersSetup() {

    g_shaders = malloc(sizeof(GLuint) * SHADER_COUNT);
    char pathes[2][64] = {
        "Shaders/Material.vert",
        "Shaders/Material.frag",
    };
    g_shaders[0] = createProgram(pathes, 2);

    strncpy(pathes[0], "Shaders/Model.vert", 64);
    strncpy(pathes[1], "Shaders/Model.frag", 64);
    g_shaders[1] = createProgram(pathes, 2);

    strncpy(pathes[0], "Shaders/LightSource.vert", 64);
    strncpy(pathes[1], "Shaders/LightSource.frag", 64);
    g_shaders[2] = createProgram(pathes, 2);


    g_textures[0] = loadTexture("Assets/Backpack/diffuse.jpg");
    g_textures[1] = loadTexture("Assets/Backpack/specular.jpg");
    g_textures[2] = loadTexture("Assets/Backpack/normal.png");
}

void gameObjectsSetup() {

    g_gameObjects = malloc(sizeof(GameObject) * GAME_OBJECT_COUNT);
    if (!g_gameObjects) {
        fprintf(stderr, "ERROR: coudn't locate memory for gameObjects\n");
    }

    int i = 0;
    GameObject* gameObject = &g_gameObjects[i];
    gameObject->type = BACKPACK;
    gameObject->shaderProgram = g_shaders[1];
    gameObject->position[0] = 0;
    gameObject->position[1] = 0;
    gameObject->position[2] = -5;

    gameObject->rotation[0] = 0;
    gameObject->rotation[1] = 0;
    gameObject->rotation[2] = 0;

    gameObject->scale[0] = 0.6;
    gameObject->scale[1] = 0.6;
    gameObject->scale[2] = 0.6;
    i++;

    for (int i = 1; i < GAME_OBJECT_COUNT; i++) {
        memcpy(&g_gameObjects[i], &g_gameObjects[0], sizeof(GameObject));
        g_gameObjects[i].position[0] = i * 2;
    }

    g_lightRenderer = malloc(sizeof(GameObject) * LIGHT_COUNT);
    if (!g_lightRenderer) {
        fprintf(stderr, "ERROR: coudn't locate memory for light renderer\n");
    }
    for (i = 0; i < LIGHT_COUNT; i++) {
        Light* light = &g_lights[i];
        gameObject = &g_lightRenderer[i];
        gameObject->type = CUBE;
        glm_vec3_copy(light->position, gameObject->position);
        gameObject->shaderProgram = g_shaders[2];
        glm_vec3_copy(GLM_VEC3_ZERO, gameObject->rotation);
        glm_vec3_copy(GLM_VEC3_ONE, gameObject->scale);
    }
}

void loadAssets() {
    g_models = malloc(sizeof(Model) * MODEL_COUNT);
    loadModel(&g_models[0], "Assets/Backpack/backpack.obj", "Assets/Backpack/");
    //loadModel(&g_models[1], "Assets/stanford_dragon_pbr/scene.gltf", "Assets/stanford_dragon_pbr/");

    // load objects
    objectsInit();
    g_objects = malloc(sizeof(Object) * OBJECT_COUNT);
    if (g_objects == NULL) {
        fprintf(stderr, "ERROR: coudn't allocate memory for materials!\n");
    }
    g_objects[0].vao = cubeVAO;
    g_objects[0].vbo = cubeVBO;
    g_objects[0].ebo = 0;
    g_objects[0].count = 36;

    g_objects[1].vao = planeVAO;
    g_objects[1].vbo = planeVBO;
    g_objects[1].ebo = 0;
    g_objects[1].count = 6;

    g_objects[2].vao = quadVAO;
    g_objects[2].vbo = quadVBO;
    g_objects[2].ebo = 0;
    g_objects[2].count = 6;

    g_objects[3].vao = skyboxVAO;
    g_objects[3].vbo = skyboxVBO;
    g_objects[3].ebo = 0;
    g_objects[3].count = 36;

}

void materialSetup() {
    g_materials = malloc(sizeof(Material) * MATERIAL_COUNT);

    if (g_materials == NULL) {
        fprintf(stderr, "ERROR: coudn't allocate memory for materials!\n");
    }
    
    loadMaterialValues();

    glGenBuffers(1, &g_materialsBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_materialsBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Material) * MATERIAL_COUNT, g_materials, GL_STATIC_DRAW);
} 

void loadMaterialValues() {
    char* filePath = "Assets/material_values.txt";
    FILE* file;
    file = fopen(filePath, "r");

    int i = 0;
    char line[256];
    while ( i < MATERIAL_COUNT && fgets(line, sizeof(line),file)) {

        Material* mat = &g_materials[i];

        float v[10];
        char name[64];
        // Read: [name (everything)] + 10 floats at the end
        sscanf(line,
                "%[^0-9.-] %f %f %f %f %f %f %f %f %f %f",
                name,
                &v[0], &v[1], &v[2], &v[3], &v[4],
                &v[5], &v[6], &v[7], &v[8], &v[9]
                );

        mat->diffuse[0] =  v[3]; 
        mat->diffuse[1] =  v[4]; 
        mat->diffuse[2] =  v[5]; 
        mat->specular[0] =  v[6]; 
        mat->specular[1] =  v[7]; 
        mat->specular[2] =  v[8]; 
        mat->shininess =  v[9]; 
        //printf("%d.%s %f %f %f %f %f %f %f\n", i, name, mat->diffuse[0], mat->diffuse[1], mat->diffuse[2], mat->specular[0], mat->specular[1], mat->specular[2], mat->shininess);
        i++;
    }
}
void lightsSetup() {

    g_lights = malloc(sizeof(Light) * LIGHT_COUNT);

    if (g_lights == NULL) {
        fprintf(stderr, "ERROR: coudn't allocate memory for lights!\n");
    }
    Light* light = &g_lights[0];
    light->position[0] = 0; 
    light->position[1] = 0; 
    light->position[2] = 4; 
    light->ambient[0] = 0.1f; 
    light->ambient[1] = 0.1f; 
    light->ambient[2] = 0.1f; 
    light->diffuse[0] = 0.8f; 
    light->diffuse[1] = 0.8f; 
    light->diffuse[2] = 0.8f; 
    light->specular[0] = 1; 
    light->specular[1] = 1; 
    light->specular[2] = 1; 

    light = &g_lights[1];
    light->position[0] = 0; 
    light->position[1] = -3; 
    light->position[2] = -9; 
    light->ambient[0] = 0.2f; 
    light->ambient[1] = 0; 
    light->ambient[2] = 0; 
    light->diffuse[0] = 0.15f; 
    light->diffuse[1] = 0.31f; 
    light->diffuse[2] = 0.6f; 
    light->specular[0] = 1; 
    light->specular[1] = 1; 
    light->specular[2] = 1; 
    
    // light ssbo
    glGenBuffers(1, &g_lightsBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_lightsBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Light) * LIGHT_COUNT, g_lights, GL_STATIC_DRAW);
}
void cleanGameObjects() {
    free(g_gameObjects);
}
void cleanAssets() {

    for (int i = 0; i < MODEL_COUNT; i++) {
        deleteModel(&g_models[i]);
    }
    free(g_models);
    free(g_lightRenderer);
    free(g_lights);
    free(g_objects);
    objectsDelete();
}
void cleanMaterial() {

    free(g_materials);
}
void cleanShaders() {
    for (int i = 0; i < SHADER_COUNT; i++) {
            glDeleteProgram(g_shaders[i]);
    }
    free(g_shaders);
}
void deleteBuffers(){
    glDeleteBuffers(1, &g_lightsBuffer);
    glDeleteBuffers(1, &g_materialsBuffer);
}
#endif //ASSETS_LOADER 
