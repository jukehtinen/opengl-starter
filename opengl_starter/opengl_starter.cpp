#include "Common.h"

#include "Camera.h"
#include "Decal.h"
#include "Font.h"
#include "Framebuffer.h"
#include "ImGuiHandler.h"
#include "Mesh.h"
#include "Shader.h"
#include "Terrain.h"
#include "TextRenderer.h"
#include "Texture.h"
#include "Window.h"

void InitOpenGL()
{
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glDebugMessageCallback([](GLenum /*source*/, GLenum type, GLuint /*id*/, GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/) {
        auto level = type == GL_DEBUG_TYPE_ERROR ? spdlog::level::err : spdlog::level::warn;
        spdlog::log(level, "[opengl]: Severity = {}, Message = {}", severity, message);
    },
        nullptr);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    GLint MaxPatchVertices = 0;
    glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
}

int main()
{
    spdlog::set_level(spdlog::level::debug);

    const int frameWidth = 1280;
    const int frameHeight = 720;
    opengl_starter::Window wnd{ frameWidth, frameHeight };

    InitOpenGL();

    opengl_starter::Mesh meshCube("assets/cube.glb");
    opengl_starter::Mesh meshUnitCube("assets/unit_cube.glb");
    opengl_starter::Texture texOpengl("assets/opengl.png");
    opengl_starter::Texture texGear("assets/gear.png");
    opengl_starter::Texture texHeight("assets/gradient.png");
    opengl_starter::Texture texGreen("assets/green.png");
    opengl_starter::Texture texBrown("assets/brown.png");
    opengl_starter::Texture texFont{ "assets/robotoregular.png" };
    opengl_starter::Texture texFontMono{ "assets/robotomono.png" };
    opengl_starter::Shader shaderCube("assets/cube.vert", "assets/cube.frag");
    opengl_starter::Shader shaderDecal("assets/decal.vert", "assets/decal.frag");
    opengl_starter::Shader shaderTerrain("assets/terrain.vert", "assets/terrain.frag");
    opengl_starter::Shader shaderTerrainTess("assets/terrain_tess.vert", "assets/terrain_tess.frag", "assets/terrain_tess.tesc", "assets/terrain_tess.tese");
    opengl_starter::Shader shaderPost("assets/post.vert", "assets/post.frag");
    opengl_starter::Shader shaderFont("assets/sdf_font.vert", "assets/sdf_font.frag");

    opengl_starter::Font fontRobotoRegular{ "assets/robotoregular.fnt" };
    opengl_starter::Font fontRobotoMono{ "assets/robotomono.fnt" };

    opengl_starter::Texture texColor{ frameWidth, frameHeight, GL_RGBA8 };
    opengl_starter::Texture texDepth{ frameWidth, frameHeight, GL_DEPTH32F_STENCIL8 };

    opengl_starter::Framebuffer framebuffer{
        { { GL_COLOR_ATTACHMENT0, texColor.textureName },
            { GL_DEPTH_STENCIL_ATTACHMENT, texDepth.textureName } }
    };

    opengl_starter::TextRenderer textRenderer{ &shaderFont, &texFont, &fontRobotoRegular, frameWidth, frameHeight };
    opengl_starter::TextRenderer textRendererMono{ &shaderFont, &texFontMono, &fontRobotoMono, frameWidth, frameHeight };

    opengl_starter::Terrain terrain{ true, &shaderTerrainTess, &texHeight, &texGreen, &texBrown };

    opengl_starter::Camera camera{ wnd.window };

    opengl_starter::ImGuiHandler imgui{ wnd.window };

    opengl_starter::Decals decal;

    wnd.onResize = [&](int width, int height) {
        textRenderer.ResizeWindow(width, height);
        textRendererMono.ResizeWindow(width, height);
    };

    wnd.onCursorPos = [&](double x, double y) {
        if (imgui.OnMouse(static_cast<float>(x), static_cast<float>(y)))
            return;

        camera.UpdateMouse(static_cast<float>(x), static_cast<float>(y));
    };

    wnd.onScroll = [&](double x, double y) {
        if (imgui.OnScroll(static_cast<float>(x), static_cast<float>(y)))
            return;

        camera.UpdateScroll(static_cast<float>(x), static_cast<float>(y));
    };

    wnd.onKey = [&](int key, int scancode, int action, int mods) { imgui.OnKey(key, scancode, action, mods); };
    wnd.onChar = [&](unsigned int chr) { imgui.OnChar(chr); };

    // DummyVao for post process step. glDraw cannot draw without bound vao. (todo - or can it somehow?)
    GLuint dummyVao = 0;
    glCreateVertexArrays(1, &dummyVao);

    auto prevTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(wnd.window))
    {
        glfwPollEvents();

        if (glfwGetKey(wnd.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(wnd.window, true);

        imgui.NewFrame();

        // Update
        const auto nowTime = std::chrono::high_resolution_clock::now();
        const auto delta = static_cast<float>(std::chrono::duration_cast<std::chrono::duration<double>>(nowTime - prevTime).count());
        prevTime = nowTime;

        static float r = 0.0f;
        r += delta * 1.0f;

        camera.Update(delta);
        imgui.Update(delta);

        const glm::vec3 eye{ 7.5f, 3.0f, 0.0f };
        const glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), static_cast<float>(frameWidth) / static_cast<float>(frameHeight), 0.1f, 100.0f);
        const glm::mat4 view = camera.GetViewMatrix();

        const glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::sin(r) * glm::pi<float>(), { 0.0f, 1.0f, 0.0f }) *
                                glm::rotate(glm::mat4{ 1.0f }, glm::sin(-r) * glm::pi<float>(), { 1.0f, 0.0f, 0.0f });

        const float mixValue = glm::sin(r);

        decal.OnDecalUI();

        // Render
        const float clearColor[4] = { 112.0f / 255.0f, 94.0f / 255.0f, 120.0f / 255.0f, 1.0f };
        const float clearDepth = 1.0f;

        textRenderer.Reset();
        textRendererMono.Reset();

        // Render cube to texture
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);

        glClearNamedFramebufferfv(framebuffer.fbo, GL_COLOR, 0, clearColor);
        glClearNamedFramebufferfv(framebuffer.fbo, GL_DEPTH, 0, &clearDepth);

        glViewport(0, 0, frameWidth, frameHeight);

        terrain.Render(projection, view, camera.Position);

        glBindProgramPipeline(shaderCube.pipeline);
        glProgramUniformMatrix4fv(shaderCube.vertProg, glGetUniformLocation(shaderCube.vertProg, "vp"), 1, GL_FALSE, glm::value_ptr(projection * view));
        glProgramUniformMatrix4fv(shaderCube.vertProg, glGetUniformLocation(shaderCube.vertProg, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glProgramUniform1i(shaderCube.fragProg, glGetUniformLocation(shaderCube.fragProg, "texSampler"), 0);
        glBindTextureUnit(0, texOpengl.textureName);
        glBindVertexArray(meshCube.vao);
        glDrawElements(GL_TRIANGLES, meshCube.indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        for (const auto& decal : decal.decals)
        {
            const glm::mat4 modelDecal = glm::translate(glm::mat4{ 1.0f }, decal.pos) *
                                         glm::rotate(glm::mat4{ 1.0f }, glm::radians(decal.rot.y), { 0.0f, 1.0f, 0.0f }) *
                                         glm::rotate(glm::mat4{ 1.0f }, glm::radians(decal.rot.z), { 0.0f, 0.0f, 1.0f }) *
                                         glm::rotate(glm::mat4{ 1.0f }, glm::radians(decal.rot.x), { 1.0f, 0.0f, 0.0f }) *
                                         glm::scale(glm::mat4{ 1.0f }, decal.scale);

            glDisable(GL_CULL_FACE);
            glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindProgramPipeline(shaderDecal.pipeline);
            glProgramUniformMatrix4fv(shaderDecal.vertProg, glGetUniformLocation(shaderDecal.vertProg, "vp"), 1, GL_FALSE, glm::value_ptr(projection * view));
            glProgramUniformMatrix4fv(shaderDecal.vertProg, glGetUniformLocation(shaderDecal.vertProg, "model"), 1, GL_FALSE, glm::value_ptr(modelDecal));
            glProgramUniform1i(shaderDecal.fragProg, glGetUniformLocation(shaderDecal.fragProg, "texSampler"), 0);
            glProgramUniform1i(shaderDecal.fragProg, glGetUniformLocation(shaderDecal.fragProg, "texDepth"), 1);
            glProgramUniformMatrix4fv(shaderDecal.fragProg, glGetUniformLocation(shaderDecal.fragProg, "invView"), 1, GL_FALSE, glm::value_ptr(glm::inverse(view)));
            glProgramUniformMatrix4fv(shaderDecal.fragProg, glGetUniformLocation(shaderDecal.fragProg, "invProj"), 1, GL_FALSE, glm::value_ptr(glm::inverse(projection)));
            glProgramUniformMatrix4fv(shaderDecal.fragProg, glGetUniformLocation(shaderDecal.fragProg, "invModel"), 1, GL_FALSE, glm::value_ptr(glm::inverse(modelDecal)));
            glProgramUniform4fv(shaderDecal.fragProg, glGetUniformLocation(shaderDecal.fragProg, "decalColor"), 1, glm::value_ptr(decal.color));
            glBindTextureUnit(0, texGear.textureName);
            glBindTextureUnit(1, texDepth.textureName);
            glBindVertexArray(meshUnitCube.vao);
            glDrawElements(GL_TRIANGLES, meshUnitCube.indexCount, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
            glEnable(GL_CULL_FACE);
        }

        // Render to screen with post process
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearNamedFramebufferfv(0, GL_COLOR, 0, clearColor);
        glClearNamedFramebufferfv(0, GL_DEPTH, 0, &clearDepth);

        glViewport(0, 0, wnd.width, wnd.height);

        glBindProgramPipeline(shaderPost.pipeline);
        glProgramUniform1f(shaderPost.fragProg, glGetUniformLocation(shaderPost.fragProg, "mixValue"), 0);
        glProgramUniform1i(shaderPost.fragProg, glGetUniformLocation(shaderPost.fragProg, "texSampler"), 0);
        glBindTextureUnit(0, texColor.textureName);

        glBindVertexArray(dummyVao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // Render some text
        const auto ipsum = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";

        auto text1transform = glm::translate(glm::mat4{ 1.0f }, { 5.0f, 5.0f, 0.0f });
        textRenderer.RenderString(fmt::format("Single line - {}", ipsum), text1transform);

        auto text2transform = glm::translate(glm::mat4{ 1.0f }, { 5.0f, 55.0f, 0.0f });
        textRenderer.RenderString(fmt::format("Wrapped - {}", ipsum), text2transform, 500.0f);

        auto text3transform = glm::translate(glm::mat4{ 1.0f }, { wnd.width / 2.0f, wnd.height - 60.0f, 0.0f }) * glm::scale(glm::mat4{ 1.0f }, glm::vec3{ 0.6f });
        textRendererMono.RenderString(fmt::format("Center - {}", ipsum), text3transform, 1500.0f, true);

        auto text4transform = glm::translate(glm::mat4{ 1.0f }, { 5.0f, 250.0f, 0.0f });
        textRenderer.RenderString(fmt::format("Progress - {}", ipsum), text4transform, 700.0f, false, glm::sin(r / 2.0f));

        imgui.Render();

        glfwSwapBuffers(wnd.window);
    }

    glDeleteVertexArrays(1, &dummyVao);
}
