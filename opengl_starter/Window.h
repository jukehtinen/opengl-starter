#pragma once

#include "Common.h"

namespace opengl_starter
{
    struct Window
    {
        Window(int width = 1280, int height = 720)
            : width(width), height(height)
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
                if (w->onResize)
                    w->onResize(width, height);
            });

            glfwSetCursorPosCallback(window, [](GLFWwindow* wnd, double x, double y) {
                auto w = static_cast<Window*>(glfwGetWindowUserPointer(wnd));
                if (w->onCursorPos)
                    w->onCursorPos(x, y);
            });

            glfwSetScrollCallback(window, [](GLFWwindow* wnd, double x, double y) {
                auto w = static_cast<Window*>(glfwGetWindowUserPointer(wnd));
                if (w->onScroll)
                    w->onScroll(x, y);
            });
        }

        ~Window()
        {
            glfwTerminate();
        }

        GLFWwindow* window = nullptr;
        int width = 0;
        int height = 0;

        std::function<void(int, int)> onResize;
        std::function<void(double, double)> onCursorPos;
        std::function<void(double, double)> onScroll;
    };
}
