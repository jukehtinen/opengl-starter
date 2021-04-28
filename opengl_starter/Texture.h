#pragma once

#include "Common.h"
#include "lodepng.h"

namespace opengl_starter
{
    struct Texture
    {
        Texture(const std::string& filename)
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

            glTextureParameteri(textureName, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(textureName, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTextureParameteri(textureName, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(textureName, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTextureStorage2D(textureName, 1, GL_RGBA8, width, height);

            glTextureSubImage2D(textureName, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

            glGenerateTextureMipmap(textureName);
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

        Texture(int width, int height, GLenum format)
        {
            glCreateTextures(GL_TEXTURE_2D, 1, &textureName);

            glTextureParameteri(textureName, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(textureName, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTextureParameteri(textureName, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(textureName, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTextureStorage2D(textureName, 1, format, width, height);
        }

        ~Texture()
        {
            glDeleteTextures(1, &textureName);
        }

        GLuint textureName = 0;
    };
}