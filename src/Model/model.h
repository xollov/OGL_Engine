#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdio.h>
#include <cglm/cglm.h>
#include <glad/glad.h>

#include "../Shader/shader.h"

#ifndef MODEL
#define MODEL

typedef struct {
    vec3 position;
    vec3 normals;
    vec2 texCoords;
} Vertex;

#define MAX_TEXTURES_TYPES 13
enum TextureType {
    none = 0,
    diffuse = 1,
    specular = 2,
    ambient = 3,
    emissive = 4,
    height = 5,
    normals = 6,
    shininess = 7,
    opacity = 8,
    lightmap = 9,
    reflection = 10,
    base_color = 11,
    normal_camera = 12,
    metalness = 13
};

typedef struct {
    enum TextureType type;
    unsigned int id;
    char path[256];
} Texture;

typedef struct {

    Vertex* vertices;
    unsigned int* indices;
    Texture* textures;
    int verticesSize, indicesSize, texturesSize;
    unsigned int VAO,VBO,EBO;
} Mesh;

typedef struct {
    int meshesSize;
    Mesh* meshes;
} Model;

void loadModel(Model* model, const char* modelPath, const char* absolutePath);
void drawModel(Model* model, GLuint shader);
void drawMesh(Mesh* mesh, GLuint shader);
void deleteModel(Model* model);
#endif // MODEL
