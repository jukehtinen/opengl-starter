#include "TextRenderer.h"
#include "Buffer.h"
#include "Font.h"
#include "Shader.h"
#include "Texture.h"
#include "Utils.h"

using namespace opengl_starter;

const int MaxLetters = 1000;

struct TextVertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec4 color;
};

TextRenderer::TextRenderer(opengl_starter::Shader* textShader, opengl_starter::Texture* textTexture, opengl_starter::Font* font, int windowWidth, int windowHeight)
    : _textShader(textShader), _textTexture(textTexture), _font(font),
      _projMatrix(glm::ortho(0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight), 0.0f, -1.0f, 1.0f))
{
    std::vector<uint32_t> indices;
    indices.resize(MaxLetters * 6);
    for (auto i = 0; i < MaxLetters; i++)
    {
        indices[i * 6 + 1] = 3 + 4 * i;
        indices[i * 6 + 0] = 2 + 4 * i;
        indices[i * 6 + 2] = 0 + 4 * i;

        indices[i * 6 + 4] = 2 + 4 * i;
        indices[i * 6 + 3] = 1 + 4 * i;
        indices[i * 6 + 5] = 0 + 4 * i;
    }

    _vertexBuffer = std::make_shared<Buffer>(MaxLetters * 4 * sizeof(TextVertex));
    _indexBuffer = std::make_shared<Buffer>(MaxLetters * 6 * sizeof(uint32_t), indices.data());

    glCreateVertexArrays(1, &_vao);

    glVertexArrayVertexBuffer(_vao, 0, _vertexBuffer->buffer, 0, sizeof(TextVertex));
    glVertexArrayElementBuffer(_vao, _indexBuffer->buffer);

    glEnableVertexArrayAttrib(_vao, 0);
    glEnableVertexArrayAttrib(_vao, 1);
    glEnableVertexArrayAttrib(_vao, 2);

    glVertexArrayAttribFormat(_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(TextVertex, pos));
    glVertexArrayAttribFormat(_vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(TextVertex, uv));
    glVertexArrayAttribFormat(_vao, 2, 4, GL_FLOAT, GL_FALSE, offsetof(TextVertex, color));

    glVertexArrayAttribBinding(_vao, 0, 0);
    glVertexArrayAttribBinding(_vao, 1, 0);
    glVertexArrayAttribBinding(_vao, 2, 0);

    _mappedBuffer = glMapNamedBufferRange(_vertexBuffer->buffer, 0, static_cast<GLintptr>(MaxLetters) * 4 * sizeof(TextVertex),
        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
}

TextRenderer::~TextRenderer()
{
    glUnmapNamedBuffer(_vertexBuffer->buffer);
    glDeleteVertexArrays(1, &_vao);
}

void TextRenderer::Reset()
{
    _characters = 0;
}

void TextRenderer::ResizeWindow(int windowWidth, int windowHeight)
{
    _projMatrix = glm::ortho(0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight), 0.0f, -1.0f, 1.0f);
}

void TextRenderer::RenderString(const std::string& text, const glm::mat4& model, float maxWidth, bool centered, float progress)
{
    if (progress == 0.0f)
        return;

    TextVertex* buffer = reinterpret_cast<TextVertex*>(_mappedBuffer);

    glm::vec2 cursor = { 0.0f, 0.0f };

    // cursor.y -= _font->lineHeight;

    const float uvscale = 1.0f / _font->scaleW;

    const int characterStart = _characters;

    int currentChar = 0;
    while (currentChar < text.length())
    {
        auto [nextChars, lineWidth] = GetLineInfo(text, currentChar, maxWidth);
        cursor.x = centered ? (-lineWidth / 2.0f) : 0.0f;

        for (auto i = currentChar; i < nextChars && (i + characterStart) < MaxLetters; i++)
        {
            const auto& chr = _font->chars[static_cast<unsigned char>(text[i])];

            float curX = cursor.x + chr.xOffset;
            float curY = cursor.y + chr.yOffset;
            float dstX = curX + chr.width;
            float dstY = curY + chr.height;

            auto now = static_cast<float>(i) / static_cast<float>(text.length());
            const auto color = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f - glm::smoothstep(progress, progress + 0.1f, now) };

            buffer[(_characters * 4) + 0].pos = { curX, dstY, 0.0f };
            buffer[(_characters * 4) + 0].uv = { uvscale * chr.x, (uvscale * (chr.y + chr.height)) };
            buffer[(_characters * 4) + 0].color = color;

            buffer[(_characters * 4) + 1].pos = { dstX, dstY, 0.0f };
            buffer[(_characters * 4) + 1].uv = { uvscale * (chr.x + chr.width), (uvscale * (chr.y + chr.height)) };
            buffer[(_characters * 4) + 1].color = color;

            buffer[(_characters * 4) + 2].pos = { dstX, curY, 0.0f };
            buffer[(_characters * 4) + 2].uv = { uvscale * (chr.x + chr.width), (uvscale * chr.y) };
            buffer[(_characters * 4) + 2].color = color;

            buffer[(_characters * 4) + 3].pos = { curX, curY, 0.0f };
            buffer[(_characters * 4) + 3].uv = { uvscale * chr.x, uvscale * chr.y };
            buffer[(_characters * 4) + 3].color = color;

            _characters++;

            if (text.length() > 1 && i + 1 < text.length())
            {
                const auto& kerning = _font->kernings.find(std::make_pair(text[i], text[i + 1]));
                if (kerning != _font->kernings.end())
                    cursor.x += kerning->second;
            }

            cursor.x += chr.xAdvance;
        }

        cursor.y += _font->lineHeight;
        currentChar = nextChars;
    }

    Render(model, characterStart, currentChar);
}

void TextRenderer::Render(const glm::mat4& model, int characterStart, int characters)
{
    glBindProgramPipeline(_textShader->pipeline);
    glProgramUniformMatrix4fv(_textShader->vertProg, glGetUniformLocation(_textShader->vertProg, "mvp"), 1, GL_FALSE, glm::value_ptr(_projMatrix * model));
    glProgramUniform1i(_textShader->fragProg, glGetUniformLocation(_textShader->fragProg, "texSampler"), 0);
    glBindTextureUnit(0, _textTexture->textureName);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, characters * 6, GL_UNSIGNED_INT, reinterpret_cast<void*>(characterStart * 6 * sizeof(GLuint)));
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

float TextRenderer::GetStringWidth(const std::string& text) const
{
    float result = 0;
    for (const auto& c : text)
        result += _font->chars[c].xAdvance;
    return result;
}

std::pair<int, float> TextRenderer::GetLineInfo(const std::string& text, int currentLetter, float maxWidth) const
{
    if (maxWidth < 0.0f)
    {
        return std::make_pair(static_cast<int>(text.length()), GetStringWidth(text));
    }

    float currentWidth = 0.0f;
    int prevLineBreak = currentLetter;
    float widthAtLineBreak = 0.0f;
    for (auto i = currentLetter; i < text.length(); i++)
    {
        if (text[i] == ' ' || text[i] == ',' || text[i] == '.' || text[i] == '!' || text[i] == '?')
        {
            prevLineBreak = i;
            widthAtLineBreak = currentWidth;
        }

        const auto& chr = _font->chars[static_cast<int>(text[i])];
        currentWidth += chr.xAdvance;
        if (currentWidth >= maxWidth)
        {
            return std::make_pair(prevLineBreak + 1, widthAtLineBreak);
        }
    }
    return std::make_pair(static_cast<int>(text.length()), currentWidth);
}