#pragma once

#include "Common.h"

namespace opengl_starter
{
    struct Buffer;
    struct Shader;
    struct Texture;

    class ImGuiHandler
    {
    public:
        ImGuiHandler(GLFWwindow* window);
        ~ImGuiHandler();

        bool OnMouse(float xpos, float ypos);
        bool OnScroll(float xoffset, float yoffset);
        bool OnKey(int key, int scancode, int action, int mods);
        bool OnChar(unsigned int chr);

        void NewFrame();
        void Update(float delta);

        void Render();

    private:
        void CreateBuffers(GLsizeiptr vertBufferSize, GLsizeiptr indexBufferSize);

    private:
        GLFWwindow* _window = nullptr;
        std::vector<GLFWcursor*> _mouseCursors;
        std::vector<bool> _mouseJustPressed;

        opengl_starter::Shader* _shader = nullptr;
        opengl_starter::Texture* _texFont = nullptr;

        GLuint _vao = 0;
        std::shared_ptr<Buffer> _vertexBuffer = nullptr;
        std::shared_ptr<Buffer> _indexBuffer = nullptr;
        void* _mappedVertexBuffer = nullptr;
        void* _mappedIndexBuffer = nullptr;
    };
}