#pragma once

#include "Utils.h"

namespace opengl_starter
{
    struct Shader
    {
        Shader(const std::string& vertFile, const std::string& fragFile)
        {
            vertProg = CreateProgram(GL_VERTEX_SHADER, Utils::File::LoadString(vertFile));
            fragProg = CreateProgram(GL_FRAGMENT_SHADER, Utils::File::LoadString(fragFile));

            glGenProgramPipelines(1, &pipeline);
            glBindProgramPipeline(pipeline);
            glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertProg);
            glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragProg);
            glValidateProgramPipeline(pipeline);
        }

        ~Shader()
        {
            glDeleteProgramPipelines(1, &pipeline);
            glDeleteProgram(vertProg);
            glDeleteProgram(fragProg);
        }

        static GLuint CreateProgram(GLuint type, const std::string& data)
        {
            const auto ptr = static_cast<const GLchar*>(data.c_str());

            const auto prog = glCreateShaderProgramv(type, 1, &ptr);

            GLint logLength;
            glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength > 0)
            {
                std::string output;
                output.reserve(logLength);
                glGetProgramInfoLog(prog, logLength, 0, static_cast<GLchar*>(&output[0]));
                spdlog::error("[Shader] {}", output.c_str());
            }

            return prog;
        }

        GLuint vertProg = 0;
        GLuint fragProg = 0;
        GLuint pipeline = 0;
    };
}
