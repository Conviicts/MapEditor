#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class WindowManager {
public:
    GLFWwindow* window;
    GLint wWindow = 1440;
    GLint hWindow = 900;

    WindowManager();
    ~WindowManager();

    static void FramebufferSizeCallback(GLFWwindow* window, int w, int h);
    static void WindowCloseCallback(GLFWwindow* window);
    int Initialize();
};
