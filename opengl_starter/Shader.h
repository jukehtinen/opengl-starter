#pragma once

#include "Utils.h"

namespace opengl_starter
{
    struct Shader
    {
        Shader(const std::string& vertFile, const std::string& fragFile, const std::string& tesc = "", const std::string& tese = "")
        {
            glGenProgramPipelines(1, &pipeline);
            glBindProgramPipeline(pipeline);

            vertProg = CreateProgram(GL_VERTEX_SHADER, Utils::File::LoadString(vertFile));
            fragProg = CreateProgram(GL_FRAGMENT_SHADER, Utils::File::LoadString(fragFile));

            glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertProg);
            glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragProg);

            if (!tesc.empty())
            {
                tescProg = CreateProgram(GL_TESS_CONTROL_SHADER, Utils::File::LoadString(tesc));
                teseProg = CreateProgram(GL_TESS_EVALUATION_SHADER, Utils::File::LoadString(tese));
                glUseProgramStages(pipeline, GL_TESS_CONTROL_SHADER_BIT, tescProg);
                glUseProgramStages(pipeline, GL_TESS_EVALUATION_SHADER_BIT, teseProg);
            }

            glValidateProgramPipeline(pipeline);
        }

        ~Shader()
        {
            glDeleteProgramPipelines(1, &pipeline);
            glDeleteProgram(vertProg);
            glDeleteProgram(fragProg);
        }

        void Bind()
        {
            glBindProgramPipeline(pipeline);
        }

        void SetFloat(const std::string& uniform, float value)
        {
            glProgramUniform1f(fragProg, glGetUniformLocation(fragProg, uniform.c_str()), value);
        }

        void SetInt(const std::string& uniform, int value)
        {
            glProgramUniform1i(fragProg, glGetUniformLocation(fragProg, uniform.c_str()), value);
        }

        void SetMat4(const std::string& uniform, const glm::mat4& value)
        {
            glProgramUniformMatrix4fv(vertProg, glGetUniformLocation(vertProg, uniform.c_str()), 1, GL_FALSE, glm::value_ptr(value));
        }

        void SetVec2(const std::string& uniform, const glm::vec2& value)
        {
            glProgramUniform2fv(fragProg, glGetUniformLocation(fragProg, uniform.c_str()), 1, glm::value_ptr(value));
        }

        void SetVec4(const std::string& uniform, const glm::vec4& value)
        {
            glProgramUniform4fv(fragProg, glGetUniformLocation(fragProg, uniform.c_str()), 1, glm::value_ptr(value));
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

                DEBUGGER;
            }

            // PrintUniforms(prog);

            return prog;
        }

        static void PrintUniforms(GLuint prog)
        {
            GLint uniforms = 0;
            glGetProgramiv(prog, GL_ACTIVE_UNIFORMS, &uniforms);

            if (uniforms > 0)
            {
                GLint maxNameLength = 0;
                glGetProgramiv(prog, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

                GLsizei length = 0;
                GLsizei count = 0;
                GLenum type = GL_NONE;
                for (GLint i = 0; i < uniforms; ++i)
                {
                    std::string name(maxNameLength, '\0');
                    glGetActiveUniform(prog, i, maxNameLength, &length, &count, &type, name.data());
                    spdlog::debug("[Shader] Uniform {} {} {}", name, type, count);
                }
            }
            else
            {
                spdlog::debug("[Shader] No uniforms found");
            }
        }

        GLuint vertProg = 0;
        GLuint fragProg = 0;
        GLuint tescProg = 0;
        GLuint teseProg = 0;
        GLuint pipeline = 0;
    };
}
