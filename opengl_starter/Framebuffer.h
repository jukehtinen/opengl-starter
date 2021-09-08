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

            if (attachments.size() == 1 && attachments[0].first == GL_DEPTH_ATTACHMENT)
            {
                glNamedFramebufferDrawBuffer(fbo, GL_NONE);
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