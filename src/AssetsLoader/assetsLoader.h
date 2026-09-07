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

extern Model* g_models;
extern Object* g_objects;
extern GameObject* g_gameObjects;
extern GLuint* g_shaders;
extern GLuint g_cubeShader, gbufferShader, gbufferRenderShader, gbufferShowShader, fboShader ;

extern Material* g_materials;
extern GLuint g_materialsBuffer;

extern GameObject* g_lightRenderer;
extern Light* g_lights;
extern GLuint g_lightsBuffer;

extern GLuint gbuffer, gbuffer_tex[4];
extern GLuint fbo, fbo_texture, fbo_depth;

extern GLuint debug_texture;
extern GLuint g_textures[3];

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
void lightsSetup();

void parseMaterials(Material* material);
void checkFramebuffer(GLenum status);

void cleanGameObjects();
void cleanAssets();
void cleanMaterial();
void cleanLights();
void cleanShaders();
void deleteBuffers();

enum Shaders{
    SHADER_MATERIAL = 0,
    SHADER_MODEL = 1,
    SHADER_LIGHT_SOURCE = 2,
    SHADER_GBUFFER = 3,
    SHADER_GBUFFER_VIS = 4,
    SHADER_GBUFFER_RENDER = 5,
};

#endif //ASSETS_LOADER 
