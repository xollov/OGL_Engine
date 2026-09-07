#include "assetsLoader.h"

Model* g_models;
Object* g_objects;
GLuint* g_shaders;
GLuint g_cubeShader, gbufferShader, gbufferRenderShader, gbufferShowShader, fboShader ;

Material* g_materials;
GLuint g_materialsBuffer;

GameObject* g_lightRenderer;
Light* g_lights;
GLuint g_lightsBuffer;

GLuint gbuffer, gbuffer_tex[4];
GLuint fbo, fbo_texture, fbo_depth;

GLuint debug_texture;
GLuint g_textures[3];

void loadShaders() {
    g_shaders = malloc(sizeof(GLuint) * SHADER_COUNT);

    g_shaders[SHADER_MATERIAL] = createProgram((const char[][64]){"Shaders/Material/Material.vert", "Shaders/Material/Material.frag"}, 2);

    g_shaders[SHADER_MODEL] = createProgram((const char[][64])
            {"Shaders/Model/Model.vert", 
            "Shaders/Model/Model.frag"}, 
            2);

    g_shaders[SHADER_LIGHT_SOURCE] = createProgram((const char[][64])
            {"Shaders/LightSource/LightSource.vert", 
            "Shaders/LightSource/LightSource.frag"}, 
            2);

    g_cubeShader = createProgram((const char[][64])
            {"Shaders/Cube/Cube.vert", 
            "Shaders/Cube/Cube.frag"}, 
            2);

    // debug
    gbufferShader = createProgram((const char[][64])
            {"Shaders/GBuffer/GBuffer.vert", 
            "Shaders/GBuffer/GBuffer.frag"}, 
            2);
    
    gbufferShowShader = createProgram((const char[][64])
            {"Shaders/GBuffer/GBufferRender.vert", 
            "Shaders/GBuffer/GBufferShow.frag"}, 
            2);
    
    gbufferRenderShader = createProgram((const char[][64])
            {"Shaders/GBuffer/GBufferRender.vert", 
            "Shaders/GBuffer/GBufferRender.frag"}, 
            2);

    fboShader = createProgram((const char[][64])
            {"Shaders/GBuffer/GBufferRender.vert", 
            "Shaders/PostProcessing/FramebufferShow.frag"}, 
            2);

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
