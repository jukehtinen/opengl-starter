#pragma once

#include <fstream>
#include <streambuf>

namespace opengl_starter
{
    struct Shader
    {
        Shader(const std::string& vertFile, const std::string& fragFile)
        {
            vertProg = CreateProgram(GL_VERTEX_SHADER, LoadString(vertFile));
            fragProg = CreateProgram(GL_FRAGMENT_SHADER, LoadString(fragFile));

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

        static std::string LoadString(const std::string& filename)
        {
            std::ifstream stream(filename);
            if (!stream.is_open())
            {
                spdlog::error("[Shader] Failed to load file {}", filename);
                return "";
            }

            std::string output;
            stream.seekg(0, std::ios::end);
            output.reserve(stream.tellg());
            stream.seekg(0, std::ios::beg);

            output.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
            return output;
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
