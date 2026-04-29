#include "shader.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../Common/stb_image.h"

int readFromFile(FILE* from, char** to){
    if (from == NULL) {
        return 1;
    }
    fseek(from, 0, SEEK_END);
    long size = ftell(from);
    fseek(from, 0, SEEK_SET);
    *to = malloc(size+1);
    fread(*to, 1, size, from);
    (*to)[size] = '\0';
    return 0;
}

GLuint createProgram(const char shaders[][64], int count) {
    
    GLuint program = glCreateProgram();

    for (int i = 0; i < count; i++) {

        GLenum type;
        char trail[10];
        char* loc = strrchr(shaders[i], '.');
        strcpy(trail, loc+1);
        if (strcmp(trail, "vert") == 0) {
            type = GL_VERTEX_SHADER;
        }
        else if (strcmp(trail, "frag") == 0) {
            type = GL_FRAGMENT_SHADER;
        }
        else if (strcmp(trail, "tecs") == 0) {
            type = GL_TESS_CONTROL_SHADER;;
        }
        else if (strcmp(trail, "tese") == 0) {
            type = GL_TESS_EVALUATION_SHADER;
        }
        else if (strcmp(trail, "geom") == 0) {
            type = GL_GEOMETRY_SHADER;
        }
        else {
            type = GL_NONE;
        }
        GLuint shader = createShader(shaders[i], type);
        glAttachShader(program, shader);
        glDeleteShader(shader);
    }
    glLinkProgram(program);

    int success;
    char infolog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infolog);
        printf("Shader link failed for program.\n%s\n", infolog);
        for (int i = 0; i < count; i++) {
            printf("%s\t", shaders[i]);
        }
        putchar('\n');
    }

    return program;
}
GLuint createShader(const char* path, GLenum type) {

    FILE *file = fopen(path, "rb");
    char* source = NULL;
    if (readFromFile(file, &source) != 0) {
        printf("couldnt read from file: %s\n", path);
    }
    fclose(file);

    int success;
    char infolog[512];
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar *const *)&source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infolog);
        printf("Shader compilation failed.\n%s\n", infolog);
    }
    free(source);
    return shader;
}
unsigned int loadTexture(const char* path) {

    unsigned int texture;
    glGenTextures(1, &texture);

    int width, height, nrchannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrchannels, 0);
    if (data)
    {
        GLenum format;
        switch (nrchannels) {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4: 
                format = GL_RGBA;
        }
        glBindTexture(GL_TEXTURE_2D, texture); 
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //printf("Texture loaded: %s, texID: %d, components: %d\n", path, texture, nrchannels);
    }
    else
{
        printf("failed to load texture. %s\n", path);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
    return texture; 
}

unsigned int loadCubemap(const char* path) {

    static char sides[6][10] = {"right", "left", "top", "bottom", "front", "back"};
    unsigned int cubemap;
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

    int width, height, nrchannels;
    unsigned char *data;
    char buffer[256];
    for (int i = 0; i < 6; i++) {
        strcpy(buffer, (char*)path); 
        strcat(buffer, sides[i]); 
        strcat(buffer, ".jpg"); 

        data = stbi_load(buffer, &width, &height, &nrchannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            printf("Failed to load cube map texture!!! %s.", buffer);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

    return cubemap;
}
void s_setInt(SHADER shader, char* name, int value) {
    glUniform1i(glGetUniformLocation(shader,name), value);
}
void s_setUInt(SHADER shader, char* name, unsigned int value){
    glUniform1ui(glGetUniformLocation(shader,name), value);
}
void s_setFloat(SHADER shader, char* name, float value) {
    glUniform1f(glGetUniformLocation(shader, name), value);
}
void s_setVec3f(SHADER shader, char* name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(shader, name), x, y, z);
}
void s_setVec3farr(SHADER shader, char* name, float *arr) {
    glUniform3f(glGetUniformLocation(shader, name), arr[0], arr[1], arr[2]);
}
void s_setVec4f(SHADER shader, char* name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(shader, name), x, y, z, w);
}
void s_setMatrix4fv(SHADER shader, char* name, int count, int transpose, float* value) {
    GLint loc = glGetUniformLocation(shader, name);
    glUniformMatrix4fv(loc, count, transpose, value);   
}
