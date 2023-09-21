#include "Window.h"
#include <glad.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
int Window::keyPressType = 0;
GLFWwindow *Window::windows = nullptr;
int Window::width = 1280;
int Window::height = 720;
Window::Window()
{

    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    // 设置OpenGLES 3.0 profile
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    windows = glfwCreateWindow(width, height, "test opengl windows", NULL, NULL); // 创建窗口
    if (!windows)
    {
        glfwTerminate();
    }
    glfwMakeContextCurrent(windows);
    glfwSetInputMode(windows, GLFW_STICKY_KEYS, GLFW_FALSE);
    glfwSetKeyCallback(windows, Window::keyfunc);
    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL/ES context" << std::endl;
    }
}
Window::~Window()
{
    glfwDestroyWindow(windows);
    glfwTerminate();
}
void Window::processInputs()
{

    if (glfwGetKey(windows, GLFW_KEY_D) == GLFW_PRESS)
        ;
    if (glfwGetKey(windows, GLFW_KEY_A) == GLFW_PRESS)
        ;
    if (glfwGetKey(windows, GLFW_KEY_W) == GLFW_PRESS)
        ;
    if (glfwGetKey(windows, GLFW_KEY_S) == GLFW_PRESS)
        ;
}
void Window::keyfunc(GLFWwindow *window, int key, int scancode, int action, int mods) // 按键回调函数
{

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE); // 退出是销毁窗口
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        keyPressType = WINDOW_KEY_SPACE;
    }
    else if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        keyPressType = WINDOW_KEY_M;
    }
    else if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        keyPressType = WINDOW_KEY_L;
    }
    else if (key == GLFW_KEY_K && action == GLFW_PRESS)
    {
        keyPressType = WINDOW_KEY_K;
    }
    else if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        keyPressType = WINDOW_KEY_H;
    }
}
