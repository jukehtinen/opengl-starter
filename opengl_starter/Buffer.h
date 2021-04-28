#pragma once

#include "Common.h"

namespace opengl_starter
{
    struct Buffer
    {
        Buffer(GLsizeiptr size, const void* data)
            : bufferSize(size)
        {
            glCreateBuffers(1, &buffer);
            glNamedBufferData(buffer, size, data, GL_STATIC_DRAW);
        }

        Buffer(GLsizeiptr size)
            : bufferSize(size)
        {
            glCreateBuffers(1, &buffer);
            glNamedBufferStorage(buffer, size, nullptr, GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT);
        }

        ~Buffer()
        {
            glDeleteBuffers(1, &buffer);
        }

        GLuint buffer = 0;
        GLsizeiptr bufferSize = 0;
    };
}