
#ifndef UTILITY
#define UTILITY

#include <glad/glad.h>
#include <cglm/cglm.h>

typedef struct {
    char showFPS;
    char renderLights;
    enum RenderMode{
        DEFERRED,
        FORWARD,
    } renderMode;
    int width;
    int height;
    int lightCount;
    int gameObjectCount;
} Options;

enum Colors{
RED,
BLUE,
GREEN,
CYAN,
RUBY
};

extern Options options;
extern const vec3 colors[];

typedef struct {
    size_t size, capacity;
    void* data;
}vector;
#endif // UTILITY
