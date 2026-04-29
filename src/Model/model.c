#include "model.h"
#include <assimp/material.h>

#define MAX_TEXTURES 100
static Texture loaded_textures[MAX_TEXTURES];
static int loadedTexturesCount = 0;
static const char textureTypeNames[2][10] = {"diffuse", "specular"};

static char assetPath[256];

void loadMaterialTextures(struct aiMaterial* mat, enum aiTextureType type, Texture* texture, int* textureCount) {
   
    for (int i = 0; i < aiGetMaterialTextureCount(mat, type); i++) {
        struct aiString str; 
        aiGetMaterialTexture(mat, type, i, &str, NULL,NULL,NULL,NULL,NULL,NULL);
        int skip = 0;
        for (int j = 0; j < MAX_TEXTURES; j++) {
            if (strncmp(loaded_textures[j].path, str.data, str.length) == 0) {
                skip = 1;
                texture[*textureCount] = loaded_textures[j];
                (*textureCount)++;
                break;
            }
        }
        if (skip == 0) {
            Texture tex;        
            char buff[256] = "";
            strcat(buff, assetPath);
            strcat(buff, str.data);
            tex.id = loadTexture(buff); 
            tex.type = type - none;
            printf("%s, type: %d\n", str.data, type);
            strcpy(tex.path, str.data);

            texture[(*textureCount)++] = tex;
            loaded_textures[loadedTexturesCount++] = tex;
            if (loadedTexturesCount >=  MAX_TEXTURES) {
               printf("EXCIDED MAXIMUM TEXTURE COUNT!!!!!!\n"); 
               loadedTexturesCount--; 
            }
        }
    }
}
void setupMesh(Mesh* mesh) {
       
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);
    
    glBindVertexArray(mesh->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->verticesSize * sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indicesSize * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (sizeof(float) * 3));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (sizeof(float) * 6));

    glBindVertexArray(0);
}

// API model.h
void loadModel(Model* model, const char* modelPath, const char* absolutePath) {
    
    strcpy(assetPath, absolutePath);
    printf("Start model loading: %s\n", modelPath);
    const struct aiScene* scene = aiImportFile(
        modelPath,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_PreTransformVertices |
        aiProcess_GlobalScale
    );

    if (NULL == scene) {

        printf("Scene import failed.\n");
        model->meshesSize = 0;
        return;
    }
    // calculate model size
    int modelSize = 0;
    int vertexOffset, indicesOffset, textureOffset;
    modelSize = scene->mNumMeshes * sizeof(Mesh);

    // vertices
    vertexOffset = modelSize;
    for (int i = 0; i < scene->mNumMeshes; i++) {
        struct aiMesh* m = scene->mMeshes[i];
        modelSize += m->mNumVertices * sizeof(Vertex);
    }

    // indices
    indicesOffset = modelSize;
    for (int i = 0; i < scene->mNumMeshes; i++) {
        struct aiMesh* m = scene->mMeshes[i];
        for (int i = 0; i < m->mNumFaces; i++) {
            modelSize += m->mFaces[i].mNumIndices * sizeof(unsigned int);
            if (m->mFaces[i].mNumIndices != 3) {
                printf("Indices are not 3!!\n");
            }
        }
    }

    // textures
    textureOffset = modelSize;
    for (int i = 0; i < scene->mNumMeshes; i++) {
        int textureCount = 0;
        struct aiMaterial* mat;

        struct aiMesh* m = scene->mMeshes[i];
        if (m->mMaterialIndex >= 0 && m->mMaterialIndex < scene->mNumMaterials) {
            mat = scene->mMaterials[m->mMaterialIndex];
            for (int type = 1; type < MAX_TEXTURES_TYPES; type++) {
                textureCount += aiGetMaterialTextureCount(mat, type); 
            }
        } else {
            mat = NULL;
        }
        modelSize += textureCount * sizeof(Texture);
    }
    printf("size of model is: %d\n", modelSize);
    void* basePtr = malloc(modelSize);
    model->meshes = basePtr;

    if (NULL == model->meshes) {
        printf("ERROR: Coudn't allocate %d bytes of memory for model %s.\n", modelSize, modelPath);

    }
    model->meshesSize = scene->mNumMeshes;
    //fill memory
    for (int i = 0; i < scene->mNumMeshes; i++) {

        struct aiMesh* m = scene->mMeshes[i];
        Mesh* mesh = &model->meshes[i];
        mesh->vertices = basePtr + vertexOffset;  
        mesh->indices  = basePtr + indicesOffset;
        mesh->textures = basePtr + textureOffset;
        
        // fill vertices
        Vertex* vertices = mesh->vertices;
        for (int i = 0; i < m->mNumVertices; i++) {
            vertices[i].position[0] = m->mVertices[i].x;
            vertices[i].position[1] = m->mVertices[i].y;
            vertices[i].position[2] = m->mVertices[i].z;

            if (m->mTextureCoords[0]) {
                vertices[i].texCoords[0] = m->mTextureCoords[0][i].x;
                vertices[i].texCoords[1] = m->mTextureCoords[0][i].y;
            }
            else {
                vertices[i].texCoords[0] = 0;
                vertices[i].texCoords[1] = 0;
            }
            if (m->mNormals) {

                vertices[i].normals[0] = m->mNormals[i].x;
                vertices[i].normals[1] = m->mNormals[i].y;
                vertices[i].normals[2] = m->mNormals[i].z;
            }
            else {

                vertices[i].normals[0] = 0;
                vertices[i].normals[1] = 0;
                vertices[i].normals[2] = 0;
            } }
        mesh->verticesSize = m->mNumVertices; 

        // fill indices
        unsigned int* indices = mesh->indices;
        int offset = 0;
        for (int i = 0; i < m->mNumFaces; i++) {
            struct aiFace face = m->mFaces[i];
            for (int j = 0; j < face.mNumIndices; j++) {
                indices[offset++] = face.mIndices[j];
            }
        }
        mesh->indicesSize = offset; 

        // fill textures
        Texture* textures = mesh->textures;
        struct aiMaterial* mat;
        if (m->mMaterialIndex >= 0 
            && m->mMaterialIndex < scene->mNumMaterials) {
            mat = scene->mMaterials[m->mMaterialIndex];
        }
        if (NULL != mat) {

            int offset = 0;
            mesh->texturesSize = 0;
            for (int type = 1; type < MAX_TEXTURES_TYPES; type++) {
                loadMaterialTextures(mat, type, textures, &offset);
                mesh->texturesSize += aiGetMaterialTextureCount(mat, type); 
            }
        }
        

        // update offsets 
        vertexOffset  += mesh->verticesSize * sizeof(Vertex);
        indicesOffset += mesh->indicesSize  * sizeof(unsigned int);
        textureOffset += mesh->texturesSize * sizeof(Texture);
    }
    // setup vao,vbo,ebo
    for (int i = 0; i <  model->meshesSize; i++) {
        setupMesh(&model->meshes[i]); 
    }

    aiReleaseImport(scene);
}
void drawModel(Model* model, GLuint shader) {

    for (int i = 0; i < model->meshesSize; i++) {
        drawMesh(&model->meshes[i], shader);
    }
}

extern GLuint g_textures[3];
void drawMesh(Mesh* mesh, GLuint shader) {
    
    // textures
    /*
     * May cause a bug
    static int previousCount = 0;
    for (int i = 0; i < previousCount; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    unsigned int textureNr[2] = {1,1};
    previousCount = mesh->texturesSize;
    for (int i = 0; i < mesh->texturesSize; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, g_textures[i]);
    }
    */
    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, mesh->indicesSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void deleteModel(Model* model) {

    for (int i = 0; i < model->meshesSize; i++) {
        Mesh mesh = model->meshes[i];
        glDeleteVertexArrays(1, &(mesh.VAO));
        glDeleteBuffers(1, &mesh.VBO);
        glDeleteBuffers(1, &mesh.EBO);
    }
    free(model->meshes);
}
// API model.h end
