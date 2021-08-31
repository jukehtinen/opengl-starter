#pragma once

#include "Common.h"

namespace opengl_starter
{
    struct Camera
    {
        const float MaxSpeed = 15.0f;
        const float Sensitivity = 0.1f;
        const float FOV = 45.0f;

        Camera(GLFWwindow* window, glm::vec3 position = glm::vec3(15.0f, 12.0f, 10.0f))
            : window(window), Position(position)
        {
            UpdateVectors();
        }

        void Update(float delta)
        {
            float velocity = MovementSpeed * delta;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                Position += Front * velocity;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                Position -= Front * velocity;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                Position -= Right * velocity;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                Position += Right * velocity;
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                Position += glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
            if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
                Position += glm::vec3(0.0f, -1.0f, 0.0f) * velocity;
        }

        void UpdateVectors()
        {
            glm::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(front);
            Right = glm::normalize(glm::cross(Front, WorldUp));
            Up = glm::normalize(glm::cross(Right, Front));
        }

        void UpdateMouse(float xpos, float ypos)
        {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) != GLFW_PRESS)
            {
                firstMouse = true;
                return;
            }

            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = (xpos - lastX) * MouseSensitivity;
            float yoffset = (lastY - ypos) * MouseSensitivity;

            lastX = xpos;
            lastY = ypos;

            Yaw += xoffset;
            Pitch += yoffset;

            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;

            UpdateVectors();
        }

        void UpdateScroll(float xoffset, float yoffset)
        {
            Fov -= yoffset;

            if (Fov < 1.0f)
                Fov = 1.0f;
            if (Fov > 45.0f)
                Fov = 45.0f;
        }

        const glm::mat4 GetViewMatrix()
        {
            return glm::lookAt(Position, Position + Front, Up);
        }

        GLFWwindow* window = nullptr;
        glm::vec3 Position;
        glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 Up = glm::vec3{ 0.0f, 1.0f, 0.0f };
        glm::vec3 Right;
        glm::vec3 WorldUp = glm::vec3{ 0.0f, 1.0f, 0.0f };

        float Yaw = -130.0f;
        float Pitch = -30.0f;

        float MovementSpeed = MaxSpeed;
        float MouseSensitivity = Sensitivity;
        float Fov = FOV;

        bool firstMouse = true;
        float lastX = 0;
        float lastY = 0;
    };
}