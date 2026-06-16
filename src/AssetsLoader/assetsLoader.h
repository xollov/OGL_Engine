#ifndef ASSETS_LOADER
#define ASSETS_LOADER

#include <cglm/cglm.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "../Shader/shader.h"
#include "../Common/objects.h"
#include "../Model/model.h"
#include "../Common/utility.h"
#include "GLFW/glfw3.h"


typedef struct {
    GLuint vao, vbo, ebo, count;
} Object;

typedef struct {
    GLuint type, shaderProgram;
    vec3 position, scale, rotation;
} GameObject;

typedef struct {
    vec4 diffuse, specular;
    unsigned int shininess;
}Material;

typedef struct {
    vec4 position, ambient, diffuse, specular;
}Light;

Model* g_models;
Object* g_objects;
GameObject* g_gameObjects;
GLuint* g_shaders;
GLuint g_cubeShader, gbufferShader, gbufferRenderShader, gbufferShowShader, fboShader ;

Material* g_materials;
GLuint g_materialsBuffer;

GameObject* g_lightRenderer;
Light* g_lights;
GLuint g_lightsBuffer;

GLuint gbuffer, gbuffer_tex[4];
GLuint fbo, fbo_texture, fbo_depth;

extern GLFWwindow* g_window;
// TODO: this is bad so refactor the defines. Not flexable
#define MODEL_COUNT     1
#define SHADER_COUNT    6
#define MATERIAL_COUNT 23
#define LIGHT_COUNT 100 
#define OBJECT_COUNT    4 

#define GAME_OBJECT_COUNT 100 

enum ObjectType {
    CUBE = 0,
    PLANE,
    QUAD,
    SKYBOX,
};
enum ModelType {
    BACKPACK = OBJECT_COUNT,
    DRAGON,
    NINJA,
};
enum MetarialType {
    BRONZE
};


void loadFramebuffers();
void loadGameObjects();
void loadAssets();
void loadMaterialValues();
void loadShaders();
void loadMaterials();

void sceneSetup();

void parseMaterials(Material* material);
void checkFramebuffer(GLenum status);

void cleanGameObjects();
void cleanAssets();
void cleanMaterial();
void cleanLights();
void cleanShaders();
void deleteBuffers();

GLuint g_textures[3];
enum Shaders{
    SHADER_MATERIAL = 0,
    SHADER_MODEL = 1,
    SHADER_LIGHT_SOURCE = 2,
    SHADER_GBUFFER = 3,
    SHADER_GBUFFER_VIS = 4,
    SHADER_GBUFFER_RENDER = 5,
};

GLuint debug_texture;
void loadShaders() {
    g_shaders = malloc(sizeof(GLuint) * SHADER_COUNT);
    char pathes[2][64] = {
        "NULL",
        "NULL",
    };

    strncpy(pathes[0], "Shaders/Material/Material.vert", 64);
    strncpy(pathes[1], "Shaders/Material/Material.frag", 64);
    g_shaders[SHADER_MATERIAL] = createProgram(pathes, 2);

    strncpy(pathes[0], "Shaders/Model/Model.vert", 64);
    strncpy(pathes[1], "Shaders/Model/Model.frag", 64);
    g_shaders[SHADER_MODEL] = createProgram(pathes, 2);

    strncpy(pathes[0], "Shaders/LightSource/LightSource.vert", 64);
    strncpy(pathes[1], "Shaders/LightSource/LightSource.frag", 64);
    g_shaders[SHADER_LIGHT_SOURCE] = createProgram(pathes, 2);


    strncpy(pathes[0], "Shaders/Cube/Cube.vert", 64);
    strncpy(pathes[1], "Shaders/Cube/Cube.frag", 64);
    g_cubeShader = createProgram(pathes, 2);

    // debug
    strncpy(pathes[0], "Shaders/GBuffer/GBuffer.vert", 64);
    strncpy(pathes[1], "Shaders/GBuffer/GBuffer.frag", 64);
    gbufferShader = createProgram(pathes, 2);
    
    strncpy(pathes[0], "Shaders/GBuffer/GBufferRender.vert", 64);
    strncpy(pathes[1], "Shaders/GBuffer/GBufferShow.frag", 64);
    gbufferShowShader = createProgram(pathes, 2);
    
    strncpy(pathes[0], "Shaders/GBuffer/GBufferRender.vert", 64);
    strncpy(pathes[1], "Shaders/GBuffer/GBufferRender.frag", 64);
    gbufferRenderShader = createProgram(pathes, 2);

    strncpy(pathes[0], "Shaders/GBuffer/GBufferRender.vert", 64);
    strncpy(pathes[1], "Shaders/FramebufferShow.frag", 64);
    fboShader = createProgram(pathes, 2);

    // textures
    g_textures[0] = loadTexture("Assets/Backpack/diffuse.jpg");
    g_textures[1] = loadTexture("Assets/Backpack/specular.jpg");
    g_textures[2] = loadTexture("Assets/stanford_dragon_pbr/textures/DefaultMaterial_baseColor.jpg");
    debug_texture = loadTexture("Assets/uv_grid_opengl.jpg");
}

void loadFramebuffers() {
    GLuint framebufferStatus;
    // fbo 
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // color buffer
    glGenTextures(1, &fbo_texture);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1920, 1080);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // depth buffer
    glGenTextures(1, &fbo_depth);
    glBindTexture(GL_TEXTURE_2D, fbo_depth);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 1920, 1080);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fbo_texture, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fbo_depth, 0);

    static const GLenum fbo_draw_buffer[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(1, fbo_draw_buffer);

    framebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        printf("fbo status:\t");
        checkFramebuffer(framebufferStatus);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // G buffer
    //GLuint width = 1920, height = 1080;
    GLuint width = options.width, height = options.height;
    glGenFramebuffers(1, &gbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);

    glGenTextures(4, gbuffer_tex);
    glBindTexture(GL_TEXTURE_2D, gbuffer_tex[0]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32UI, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glBindTexture(GL_TEXTURE_2D, gbuffer_tex[1]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, gbuffer_tex[2]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8UI, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, gbuffer_tex[3]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, width, height);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gbuffer_tex[0], 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, gbuffer_tex[1], 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, gbuffer_tex[2], 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, gbuffer_tex[3], 0);

    static const GLenum draw_buffer[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, draw_buffer);

    framebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        printf("gbuffer status:\t");
        checkFramebuffer(framebufferStatus);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
 }

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

void loadAssets() {
    g_models = malloc(sizeof(Model) * MODEL_COUNT);
    loadModel(&g_models[0], "Assets/Backpack/backpack.obj", "Assets/Backpack/");
    //loadModel(&g_models[1], "Assets/stanford_dragon_pbr/dragon.glb", "Assets/stanford_dragon_pbr/textures/");
    //loadModel(&g_models[2], "Assets/Ninja/Ninja.fbx", "Assets/Ninja/");
    
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

void loadMaterials() {
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
        mat->shininess =  v[9] * 128; 
        //printf("%d.%s %f %f %f %f %f %f %f\n", i, name, mat->diffuse[0], mat->diffuse[1], mat->diffuse[2], mat->specular[0], mat->specular[1], mat->specular[2], mat->shininess);
        i++;
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

void checkFramebuffer(GLenum status) {
    switch (status) {
        case GL_FRAMEBUFFER_UNDEFINED:
            printf("Framebuffer is UNDEFINED!!!!\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            printf("Framebuffer has INCOMPLETE ATTACHMENT!!!!\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            printf("Framebuffer has INCOMPLETE DRAW BUFFER!!!!\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            printf("Framebuffer has INCOMPLETE LAYER TARGETS!!!!\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            printf("Framebuffer has MISSING ATTACHMENT!!!!\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            printf("Framebuffer has INCOMPLETE MULTISAMPLE!!!!\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            printf("Framebuffer has incomplete READ BUFFER!!!!\n");
            break;
        default:
            break;
    }
}
void cleanGameObjects() {
    free(g_gameObjects);
}
void cleanAssets() {

    for (int i = 0; i < MODEL_COUNT; i++) {
        deleteModel(&g_models[i]);
    }
    free(g_models);
    free(g_objects);
    objectsDelete();
}
void cleanMaterial() {
    free(g_materials);
}
void cleanLights() {
    free(g_lights);
    free(g_lightRenderer);
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
