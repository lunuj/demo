#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <unistd.h>

#include "main.h"
#define LOG_IMPLEMENTATION
#include "log.h"
#include "platform.h"
#include "libretro.h"
#include "GLFW/glfw3.h"

#define RATIO 64/48

void error_callback(int error, const char* description)
{
    LOG_L(LOG_ERROR, "GLFW Error %d: %s", error, description);
}

void keep_ratio(int w, int h, float ra)
{
    float re_w = 1.0, re_h = 1.0;
    float ratio = (float)w/(float)h;
    if (ratio > ra) {
        re_w /=ratio;
    } else {
        re_h /= ratio;
    }

    glViewport(0, 0, w, h);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); 
    glVertex2f(-1.0f*re_w, 1.0f*re_h);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1.0f*re_w, 1.0f*re_h);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1.0f*re_w, -1.0f*re_h);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-1.0f*re_w, -1.0f*re_h);
    glEnd();
}

void render(const void *data, unsigned width, unsigned height, size_t pitch)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    static GLuint test = 0;
    if(test == 0)
    {
        glGenTextures(1, &test);
        glBindTexture(GL_TEXTURE_2D, test);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        srand((unsigned int)time(NULL));
    }

    glBindTexture(GL_TEXTURE_2D, test);
    glPixelStorei(GL_TEXTURE_2D, pitch);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glEnable(GL_TEXTURE_2D);
}

int main(int argc, char const *argv[])
{
    int c = getopt(argc, (char * const *)argv, "v");
    switch (c)
    {
    case 'v': // asfa 
        printf("demo mode %s\n", VERSION);
        exit(0);
    default:
        break;
    }
    LOG("Demo started.");
    glfwInit();
    glfwSetErrorCallback(error_callback);
    GLFWwindow* window = glfwCreateWindow(480, 480, "Hello World", NULL, NULL);
    glfwMakeContextCurrent(window);
    int w, h;
    glfwSwapInterval(1);
    retro_set_video_refresh(render);
    while (!glfwWindowShouldClose(window))
    {
        retro_run();
        glfwGetFramebufferSize(window, &w, &h);
        keep_ratio(w, h, RATIO);
        glfwSwapBuffers(window);
        glfwPollEvents();

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
        }
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
