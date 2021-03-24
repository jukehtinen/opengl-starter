#include "Common.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"

void InitOpenGL()
{
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
        auto level = type == GL_DEBUG_TYPE_ERROR ? spdlog::level::err : spdlog::level::warn;
        spdlog::log(level, "[opengl]: Severity = {}, Message = {}", severity, message);
    }, nullptr);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
}

int main()
{
    opengl_starter::Window wnd;

    InitOpenGL();

    opengl_starter::Mesh meshCube("assets/cube.glb");
    opengl_starter::Texture texOpengl("assets/opengl.png");
    opengl_starter::Shader shaderCube("assets/cube.vert", "assets/cube.frag");

    auto prevTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(wnd.window))
    {
        glfwPollEvents();

        if (glfwGetKey(wnd.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(wnd.window, true);

        // Update
        auto nowTime = std::chrono::high_resolution_clock::now();
        const auto delta = static_cast<float>(std::chrono::duration_cast<std::chrono::duration<double>>(nowTime - prevTime).count());
        prevTime = nowTime;

        static float r = 0.0f;
        r += delta * 1.0f;

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)wnd.width / (float)wnd.height, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3{ 7.5f, 3.0f, 0.0f }, { 0.0f, 0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::sin(r) * glm::pi<float>(), { 0.0f, 1.0f, 0.0f }) *
            glm::rotate(glm::mat4{ 1.0f }, glm::sin(-r) * glm::pi<float>(), { 1.0f, 0.0f, 0.0f });

        // Render
        glClearColor(112.0f / 256.0f, 94.0f / 256.0f, 120.0f / 256.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, static_cast<GLint>(wnd.width), static_cast<GLint>(wnd.height));

        glBindProgramPipeline(shaderCube.pipeline);
        glProgramUniformMatrix4fv(shaderCube.vertProg, glGetUniformLocation(shaderCube.vertProg, "vp"), 1, GL_FALSE, glm::value_ptr(projection * view));
        glProgramUniformMatrix4fv(shaderCube.vertProg, glGetUniformLocation(shaderCube.vertProg, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glProgramUniform1i(shaderCube.fragProg, glGetUniformLocation(shaderCube.vertProg, "texSampler"), 0);
        glBindTextureUnit(0, texOpengl.textureName);

        glBindVertexArray(meshCube.vao);
        glDrawElements(GL_TRIANGLES, meshCube.indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        glfwSwapBuffers(wnd.window);
    }
}
