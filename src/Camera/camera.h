#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

vec2 mouseLast;    // last pos of mouse
float yaw = -90, pitch = 0;    // NOTE(xollow): learn what it is again

typedef struct {

    vec3 position;
    vec3 up;
    vec3 front;
    float speed;
} Camera;

Camera g_camera;
void cameraMove(GLFWwindow* window, Camera* camera, float deltaTime) {

    camera->speed = 250 * deltaTime;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        vec3 temp;
        glm_vec3_copy(camera->front, temp);
        glm_vec3_scale(temp, camera->speed * deltaTime, temp);
        glm_vec3_add(camera->position, temp, camera->position);
    }
    if (glfwGetKey(window,  GLFW_KEY_S) == GLFW_PRESS) {
        vec3 temp;
        glm_vec3_copy(camera->front, temp);
        glm_vec3_scale(temp, -1 * camera->speed * deltaTime, temp);
        glm_vec3_add(camera->position, temp, camera->position);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        vec3 temp;
        glm_vec3_cross(camera->up, camera->front, temp);
        glm_vec3_normalize(temp);
        glm_vec3_scale(temp, camera->speed * deltaTime, temp);
        glm_vec3_add(camera->position, temp, camera->position);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        vec3 temp;
        glm_vec3_cross(camera->up, camera->front, temp);
        glm_vec3_normalize(temp);
        glm_vec3_scale(temp, -camera->speed * deltaTime, temp);
        glm_vec3_add(camera->position, temp, camera->position);
    }
    // mode camera down 
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        vec3 temp;
        glm_vec3_scale(camera->up, -camera->speed * deltaTime, temp);
        glm_vec3_add(camera->position, temp, camera->position);
    }
    // move camera up
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        vec3 temp;
        glm_vec3_scale(camera->up, camera->speed * deltaTime, temp);
        glm_vec3_add(camera->position, temp, camera->position);
    }
}

void cameraRotate(Camera *camera, double xpos, double ypos) {

    static char firstMouse = 1;
    if (firstMouse) {

        mouseLast[0] = xpos;
        mouseLast[1] = ypos;
        firstMouse = 0;
    }

    float xoffset  = xpos - mouseLast[0];
    float yoffset = mouseLast[1] - ypos ;
    mouseLast[0] = xpos;
    mouseLast[1] = ypos;


    float sens = 0.01f;
    xoffset *= sens;
    yoffset *= sens;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89) pitch = 89;
    if (pitch < -89) pitch = -89;

    vec3 dir = {
        cos(glm_rad(yaw)) * cos(glm_rad(pitch)),
        sin(glm_rad(pitch)),
        sin(glm_rad(yaw)) * cos(glm_rad(pitch)),
    };
    glm_normalize(dir);
    glm_vec3_copy(dir, camera->front);
}

void cameraSetup() {
    g_camera.position[0] = 0;
    g_camera.position[1] = 0;
    g_camera.position[2] = 0;

    g_camera.front[0] =  0;
    g_camera.front[1] =  0;
    g_camera.front[2] = -1;
    
    g_camera.up[0] = 0;
    g_camera.up[1] = 1;
    g_camera.up[2] = 0;

    g_camera.speed = 5;
}
#endif // CAMERA_H
