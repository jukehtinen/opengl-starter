#pragma once

#include "Common.h"
#include "lodepng.h"

#include <random>

namespace opengl_starter
{
    struct Texture
    {
        enum class Filter
        {
            Nearest = GL_NEAREST,
            Linear = GL_LINEAR
        };

        enum class Wrap
        {
            ClampToBorder = GL_CLAMP_TO_BORDER,
            ClampToEdge = GL_CLAMP_TO_EDGE,
            ClampToEdgeMirrored = GL_MIRROR_CLAMP_TO_EDGE,
            Repeat = GL_REPEAT,
            RepeatMirrored = GL_MIRRORED_REPEAT
        };

        Texture()
        {
        }

        Texture(const std::string& filename, Wrap wrapS = Wrap::Repeat, Wrap wrapT = Wrap::Repeat)
        {
            std::vector<unsigned char> image;
            unsigned int width, height;
            auto error = lodepng::decode(image, width, height, filename);

            if (error != 0)
            {
                spdlog::error("[lodepng] Error = {}, Message = {}", error, lodepng_error_text(error));
                return;
            }

            glCreateTextures(GL_TEXTURE_2D, 1, &textureName);

            glTextureParameteri(textureName, GL_TEXTURE_WRAP_S, (GLint)wrapS);
            glTextureParameteri(textureName, GL_TEXTURE_WRAP_T, (GLint)wrapT);

            glTextureParameteri(textureName, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(textureName, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTextureStorage2D(textureName, 1, GL_RGBA8, width, height);

            glTextureSubImage2D(textureName, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

            glGenerateTextureMipmap(textureName);

            sourceFile = filename;
        }

        Texture(int width, int height, void* data)
        {
            glCreateTextures(GL_TEXTURE_2D, 1, &textureName);

            glTextureParameteri(textureName, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(textureName, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTextureParameteri(textureName, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(textureName, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTextureStorage2D(textureName, 1, GL_RGBA8, width, height);

            glTextureSubImage2D(textureName, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

            glGenerateTextureMipmap(textureName);
        }

        Texture(int width, int height, GLenum format, Wrap wrapS = Wrap::ClampToEdge, Wrap wrapT = Wrap::ClampToEdge, Filter filter = Filter::Linear)
        {
            glCreateTextures(GL_TEXTURE_2D, 1, &textureName);

            glTextureParameteri(textureName, GL_TEXTURE_WRAP_S, (GLint)wrapS);
            glTextureParameteri(textureName, GL_TEXTURE_WRAP_T, (GLint)wrapT);
            glTextureParameteri(textureName, GL_TEXTURE_MIN_FILTER, (GLint)filter);
            glTextureParameteri(textureName, GL_TEXTURE_MAG_FILTER, (GLint)filter);

            // shadow map only
            float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            glTextureParameterfv(textureName, GL_TEXTURE_BORDER_COLOR, borderColor);

            glTextureStorage2D(textureName, 1, format, width, height);
        }

        ~Texture()
        {
            glDeleteTextures(1, &textureName);
        }

        static const std::shared_ptr<Texture> CreateNoiseTexture(int width, int height)
        {
            std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
            std::default_random_engine generator;

            std::vector<glm::vec4> data(width * height);
            for (int i = 0; i < width * height; i++)
                data[i] = glm::vec4{ distribution(generator) * 2.0f - 1.0f, distribution(generator) * 2.0f - 1.0f, 0.0f, 1.0f };

            auto texture = std::make_shared<Texture>();

            glCreateTextures(GL_TEXTURE_2D, 1, &texture->textureName);

            glTextureParameteri(texture->textureName, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(texture->textureName, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTextureParameteri(texture->textureName, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(texture->textureName, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTextureStorage2D(texture->textureName, 1, GL_RGBA32F, width, height);

            glTextureSubImage2D(texture->textureName, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, data.data());

            return texture;
        }

        GLuint textureName = 0;

        std::string sourceFile;
    };
}