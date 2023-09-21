#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>
#define WINDOW_KEY_SPACE 1
#define WINDOW_KEY_M 2
#define WINDOW_KEY_K 3
#define WINDOW_KEY_L 4
#define WINDOW_KEY_H 5
using std::shared_ptr;
using std::vector;
struct GLFWwindow;
class Window
{
public:
    static Window& getInstance()
    {
        static Window window;
        return window;
    }
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    int getKeyPressType(){int tmp=keyPressType;keyPressType=0; return tmp;}
    GLFWwindow* getWindows(){return windows;}
    void processInputs();
    static void keyfunc(GLFWwindow *window, int key, int scancode, int action, int mods);
    static int width;
    static int height;

private:
    Window();
    ~Window();
    static GLFWwindow* windows;
    static int keyPressType;
};