#pragma once

#pragma region OpenGL
#ifdef __APPLE__
#   define GL_SILENCE_DEPRECATION
#   include <OpenGL/gl.h>   /* macOS 上的 OpenGL 头文件 */
#elif defined(_WIN32) || defined(_WIN64)
#   include <GL/gl.h>       /* Windows 上的 OpenGL 头文件（需要安装相关的库，如 `freeglut` 或 `glew`） */ 
#elif defined(__linux__)
#   include <GL/gl.h>       /* Linux 上的 OpenGL 头文件（通常是 Mesa 或 Nvidia 驱动） */
#else
#   error "Unsupported platform"    /* 如果没有匹配的系统，给出编译错误 */
#endif
#pragma endregion