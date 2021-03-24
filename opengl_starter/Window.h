#pragma once

#include "Common.h"

namespace opengl_starter
{
    class Window
    {
    public:
        Window()
        {
            glfwSetErrorCallback([](int code, const char* message) { spdlog::error("[glfw] Error = {}, Message = {}", code, message); });

            glfwInit();

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_SAMPLES, 4);

            window = glfwCreateWindow(width, height, "opengl_starter", nullptr, nullptr);

            if (window == nullptr)
            {
                glfwTerminate();
            }

            glfwMakeContextCurrent(window);

            if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
            {
                spdlog::error("[glad] Failed to initialize");
                glfwTerminate();
            }

            spdlog::info("{}, {}", glGetString(GL_VERSION), glGetString(GL_RENDERER));

            glfwSetWindowUserPointer(window, this);

            glfwSetFramebufferSizeCallback(window, [](GLFWwindow* wnd, int width, int height) {
                auto w = static_cast<Window*>(glfwGetWindowUserPointer(wnd));
                w->width = width;
                w->height = height;
            });
        }

        ~Window()
        {
            glfwTerminate();
        }

        GLFWwindow* window = nullptr;
        int width = 1280;
        int height = 720;
    };
}
