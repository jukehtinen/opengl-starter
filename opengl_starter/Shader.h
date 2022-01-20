#pragma once

#include "Utils.h"

namespace opengl_starter
{
    struct ShaderUniform
    {
        GLuint shaderType{ 0 };
        GLenum type{ 0 };
        GLsizei arraySize{ 0 };
        std::string name;
        glm::vec4 value{ 0.0f };
    };

    struct Shader
    {
        Shader(const std::string& vertFile, const std::string& fragFile, const std::string& tesc = "", const std::string& tese = "")
        {
            glGenProgramPipelines(1, &pipeline);
            glBindProgramPipeline(pipeline);

            vertProg = CreateProgram(GL_VERTEX_SHADER, Utils::File::LoadText(vertFile));
            fragProg = CreateProgram(GL_FRAGMENT_SHADER, Utils::File::LoadText(fragFile));

            glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertProg);
            glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragProg);

            if (!tesc.empty())
            {
                tescProg = CreateProgram(GL_TESS_CONTROL_SHADER, Utils::File::LoadText(tesc));
                teseProg = CreateProgram(GL_TESS_EVALUATION_SHADER, Utils::File::LoadText(tese));
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

        void SetFloatVs(const std::string& uniform, float value)
        {
            glProgramUniform1f(vertProg, glGetUniformLocation(vertProg, uniform.c_str()), value);
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

        void SetVec3(const std::string& uniform, const glm::vec3& value)
        {
            glProgramUniform3fv(fragProg, glGetUniformLocation(fragProg, uniform.c_str()), 1, glm::value_ptr(value));
        }

        void SetVec4(const std::string& uniform, const glm::vec4& value)
        {
            glProgramUniform4fv(fragProg, glGetUniformLocation(fragProg, uniform.c_str()), 1, glm::value_ptr(value));
        }

        const std::vector<ShaderUniform>& GetUniforms()
        {
            for (auto& uniform : _uniforms)
            {
                switch (uniform.type)
                {
                case GL_FLOAT:
                case GL_FLOAT_VEC2:
                case GL_FLOAT_VEC3:
                case GL_FLOAT_VEC4:
                    glGetUniformfv(vertProg, glGetUniformLocation(vertProg, uniform.name.c_str()), glm::value_ptr(uniform.value));
                    break;
                default:
                    break;
                }
            }

            return _uniforms;
        }

        GLuint vertProg = 0;
        GLuint fragProg = 0;
        GLuint tescProg = 0;
        GLuint teseProg = 0;
        GLuint pipeline = 0;

    private:
        GLuint CreateProgram(GLuint type, const std::string& data)
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
                return 0;
            }

            ReadUniforms(type, prog);

            return prog;
        }

        void ReadUniforms(GLuint type, GLuint program)
        {
            GLint uniforms = 0;
            glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniforms);

            if (uniforms > 0)
            {
                GLint maxNameLength = 0;
                glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

                GLsizei actualNameLength = 0;
                GLsizei arraySize = 0;
                GLenum uniformType = GL_NONE;
                for (GLint i = 0; i < uniforms; ++i)
                {
                    std::string name(maxNameLength, '\0');
                    glGetActiveUniform(program, i, maxNameLength, &actualNameLength, &arraySize, &uniformType, name.data());
                    spdlog::debug("[Shader] Uniform {} {} {}", name, uniformType, arraySize);
                    _uniforms.push_back({ type, uniformType, arraySize, name });
                }
            }
        }

    private:
        std::vector<ShaderUniform> _uniforms;
    };
}
