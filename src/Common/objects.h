
#ifndef OBJECTS
#define OBJECTS

extern int trianglesCount[];
extern unsigned int planeVAO, planeVBO,
                    cubeVAO,  cubeVBO,
                    quadVAO, quadVBO,
                    skyboxVAO, skyboxVBO;
void objectsInit();
void objectsDelete();
#endif // OBJECTS
