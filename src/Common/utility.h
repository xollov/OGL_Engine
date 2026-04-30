
#ifndef UTILITY
#define UTILITY

#include <glad/glad.h>
#include <cglm/cglm.h>

typedef struct {
    char showFPS;
    char render;
    int width;
    int height;
} Options;
Options options = {};

enum Colors{
RED,
BLUE,
GREEN,
CYAN,
RUBY
};
const vec3 colors[] = 
{
    {1,0,0},
    {0,1,0},
    {0,0,1},
    {0,1,1},
    {0.87f, 0.066f, 0.37f},
    
};

typedef struct {
    size_t size, capacity;
    void* data;
}vector;
#endif // UTILITY
