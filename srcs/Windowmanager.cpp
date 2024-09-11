#include "WindowManager.h"

WindowManager::WindowManager() : window(nullptr) {}

WindowManager::~WindowManager() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void WindowManager::FramebufferSizeCallback(GLFWwindow* window, int w, int h) {
    const auto manager = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    manager->wWindow = w;
    manager->hWindow = h;
    glViewport(0, 0, w, h);
}

void WindowManager::WindowCloseCallback(GLFWwindow* window) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int WindowManager::Initialize() {
    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(wWindow, hWindow, "Map Editor", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetWindowCloseCallback(window, WindowCloseCallback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glewInit();
    glEnable(GL_DEPTH_TEST);

    return 0;
}
