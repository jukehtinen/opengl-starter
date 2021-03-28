#pragma once

#include "Common.h"

namespace opengl_starter
{
    struct Framebuffer
    {
        Framebuffer(const std::vector<std::pair<GLenum, GLuint>> attachments)
        {
            glCreateFramebuffers(1, &fbo);

            for (const auto [type, texture] : attachments)
            {
                glNamedFramebufferTexture(fbo, type, texture, 0);
            }

            if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                spdlog::error("[Fbo] Not complete");
            }
        }

        ~Framebuffer()
        {
            glDeleteFramebuffers(1, &fbo);
        }

        GLuint fbo = 0;
    };
}