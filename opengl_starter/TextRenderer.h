#pragma once

#include "Common.h"

namespace opengl_starter
{
    struct Buffer;
    struct Font;
    struct Shader;
    struct Texture;

    class TextRenderer
    {
    public:
        TextRenderer(opengl_starter::Shader* textShader, opengl_starter::Texture* textTexture, opengl_starter::Font* font, int windowWidth, int windowHeight);
        ~TextRenderer();

        void Reset();
        void ResizeWindow(int windowWidth, int windowHeight);

        void RenderString(const std::string& text, const glm::mat4& model = glm::mat4{ 1.0f }, float maxWidth = -1.0f, bool centered = false, float progress = 1.0f);

    private:
        void Render(const glm::mat4& model, int characterStart, int characters);

        float GetStringWidth(const std::string& text) const;
        std::pair<int, float> GetLineInfo(const std::string& text, int currentLetter, float maxWidth) const;

    private:
        opengl_starter::Shader* _textShader = nullptr;
        opengl_starter::Texture* _textTexture = nullptr;
        opengl_starter::Font* _font = nullptr;
        glm::mat4 _projMatrix = {};

        GLuint _vao = 0;
        std::shared_ptr<Buffer> _vertexBuffer = nullptr;
        std::shared_ptr<Buffer> _indexBuffer = nullptr;
        void* _mappedBuffer = nullptr;

        int _characters = 0;
    };
}