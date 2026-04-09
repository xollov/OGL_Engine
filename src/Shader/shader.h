
#ifndef SHADER_H
#define SHADER_H


#include<stdio.h>
#include <glad/glad.h>

typedef unsigned int SHADER; 
int readFromFile(FILE* from, char** to);

GLuint createProgram(const char shaders[][64], int count);
GLuint createShader(const char* path, GLenum type);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(const char* path);
void s_setInt(SHADER shader, char* name, int value);
void s_setFloat(SHADER shader, char* name, float value);
void s_setVec4f(SHADER shader, char* name, float x, float y, float z, float w);
void s_setVec3f(SHADER shader, char* name, float x, float y, float z);
void s_setVec3farr(SHADER shader, char* name, float *arr);
void s_setMatrix4fv(SHADER shader, char* name, int count, int flag, float* value);
#endif // SHADER_H
