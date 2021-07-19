#include "Common.h"

#include "Camera.h"
#include "Decal.h"
#include "Font.h"
#include "Framebuffer.h"
#include "GltfLoader.h"
#include "ImGuiHandler.h"
#include "Mesh.h"
#include "ParticleSystem.h"
#include "SSAO.h"
#include "Shader.h"
#include "Terrain.h"
#include "TextRenderer.h"
#include "Texture.h"
#include "Window.h"

void DrawSceneUI(opengl_starter::Node* node);

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

    const int frameWidth = 2560;
    const int frameHeight = 1440;
    opengl_starter::Window wnd{ frameWidth, frameHeight };

    InitOpenGL();

    std::vector<opengl_starter::Mesh*> meshes;
    std::vector<opengl_starter::Mesh*> meshesCube;


    auto files = Utils::File::GetFiles("assets/particles");

    // todo - child nodes are currently allocated and deleted by no one.
    opengl_starter::Node root{ "root" };

    opengl_starter::GltfLoader::Load("assets/atelier.glb", &root, meshes);
    opengl_starter::GltfLoader::Load("assets/cube.glb", nullptr, meshes);
    opengl_starter::GltfLoader::Load("assets/unit_cube.glb", nullptr, meshesCube);

    opengl_starter::Mesh* meshUnitCube = meshesCube[0];

    opengl_starter::Texture texPalette("assets/lospec500-32x.png");
    opengl_starter::Texture texGear("assets/gear.png");
    opengl_starter::Texture texHeight("assets/gradient.png");
    opengl_starter::Texture texGreen("assets/green.png");
    opengl_starter::Texture texBrown("assets/brown.png");
    opengl_starter::Texture texFont{ "assets/robotoregular.png" };
    opengl_starter::Texture texFontMono{ "assets/robotomono.png" };
    opengl_starter::Texture texParticle1{ "assets/particles/flame_01.png" };
    opengl_starter::Texture texParticle2{ "assets/particles/spark_01.png" };
    auto texNoise = opengl_starter::Texture::CreateNoiseTexture(4, 4);
    opengl_starter::Shader shaderCube("assets/cube.vert", "assets/cube.frag");
    opengl_starter::Shader shaderDecal("assets/decal.vert", "assets/decal.frag");
    opengl_starter::Shader shaderTerrain("assets/terrain.vert", "assets/terrain.frag");
    opengl_starter::Shader shaderTerrainTess("assets/terrain_tess.vert", "assets/terrain_tess.frag", "assets/terrain_tess.tesc", "assets/terrain_tess.tese");
    opengl_starter::Shader shaderSSAO("assets/ssao.vert", "assets/ssao.frag");
    opengl_starter::Shader shaderSSAOBlur("assets/blur.vert", "assets/blur.frag");
    opengl_starter::Shader shaderPost("assets/post.vert", "assets/post.frag");
    opengl_starter::Shader shaderFont("assets/sdf_font.vert", "assets/sdf_font.frag");

    opengl_starter::Font fontRobotoRegular{ "assets/robotoregular.fnt" };
    opengl_starter::Font fontRobotoMono{ "assets/robotomono.fnt" };

    opengl_starter::Texture texColor{ frameWidth, frameHeight, GL_RGBA8 };
    opengl_starter::Texture texDepth{ frameWidth, frameHeight, GL_DEPTH32F_STENCIL8 };
    opengl_starter::Texture texNormals{ frameWidth, frameHeight, GL_RGBA32F };
    opengl_starter::Texture texSSAO{ frameWidth, frameHeight, GL_RGBA32F };
    opengl_starter::Texture texSSAOBlur{ frameWidth, frameHeight, GL_RGBA32F };

    opengl_starter::Framebuffer framebuffer{
        { { GL_COLOR_ATTACHMENT0, texColor.textureName },
            { GL_COLOR_ATTACHMENT1, texNormals.textureName },
            { GL_DEPTH_STENCIL_ATTACHMENT, texDepth.textureName } }
    };

    opengl_starter::Framebuffer framebufferSSAO{
        {
            { GL_COLOR_ATTACHMENT0, texSSAO.textureName },
        }
    };

    opengl_starter::Framebuffer framebufferSSAOBlur{
        {
            { GL_COLOR_ATTACHMENT0, texSSAOBlur.textureName },
        }
    };

    opengl_starter::TextRenderer textRenderer{ &shaderFont, &texFont, &fontRobotoRegular, frameWidth, frameHeight };
    opengl_starter::TextRenderer textRendererMono{ &shaderFont, &texFontMono, &fontRobotoMono, frameWidth, frameHeight };

    opengl_starter::Terrain terrain{ true, &shaderTerrainTess, &texHeight, &texGreen, &texBrown };

    opengl_starter::Camera camera{ wnd.window };

    opengl_starter::ImGuiHandler imgui{ wnd.window };

    opengl_starter::Decals decal;

    opengl_starter::SSAO ssao;

    // todo - not cleaned up
    std::vector<opengl_starter::ParticleSystem*> particleSystems;
    root.RecurseNodes([&particleSystems](opengl_starter::Node* node) {
        if (node->name.find(".particles") != std::string::npos)
        {
            auto ps = new opengl_starter::ParticleSystem{};
            ps->Load("assets/ps_torch.json", node);
            ps->Start();
            particleSystems.push_back(ps);
        }
    });

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
        for (auto ps : particleSystems)
            ps->Update(delta);

        imgui.Update(delta);

        const glm::vec3 eye{ 7.5f, 3.0f, 0.0f };
        const glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), static_cast<float>(frameWidth) / static_cast<float>(frameHeight), 0.1f, 100.0f);
        const glm::mat4 view = camera.GetViewMatrix();

        const glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::sin(r) * glm::pi<float>(), { 0.0f, 1.0f, 0.0f }) *
                                glm::rotate(glm::mat4{ 1.0f }, glm::sin(-r) * glm::pi<float>(), { 1.0f, 0.0f, 0.0f });

        auto transform = [](opengl_starter::Node* n, const glm::mat4& parentTransform, auto& transformRef) -> void {
            n->model = parentTransform * glm::translate(glm::mat4{ 1.0f }, n->pos) *
                       glm::mat4_cast(n->rotq) *
                       glm::scale(glm::mat4{ 1.0f }, n->scale);

            for (auto c : n->children)
                transformRef(c, n->model, transformRef);
        };
        transform(&root, glm::mat4{ 1.0f }, transform);

        decal.OnDecalUI();
        ssao.OnUI();
        particleSystems[0]->OnUI();
        DrawSceneUI(&root);

        // Render
        const float clearColor[4] = { 112.0f / 255.0f, 94.0f / 255.0f, 120.0f / 255.0f, 1.0f };
        const float clearColor2[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        const float clearDepth = 1.0f;

        textRenderer.Reset();
        textRendererMono.Reset();

        // Render cube to texture
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);

        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);

        glClearNamedFramebufferfv(framebuffer.fbo, GL_COLOR, 0, clearColor);
        glClearNamedFramebufferfv(framebuffer.fbo, GL_COLOR, 1, clearColor2);
        glClearNamedFramebufferfv(framebuffer.fbo, GL_COLOR, 2, clearColor2);
        glClearNamedFramebufferfv(framebuffer.fbo, GL_DEPTH, 0, &clearDepth);

        glViewport(0, 0, frameWidth, frameHeight);

        // terrain.Render(projection, view, camera.Position);

        glBindProgramPipeline(shaderCube.pipeline);
        glProgramUniformMatrix4fv(shaderCube.vertProg, glGetUniformLocation(shaderCube.vertProg, "vp"), 1, GL_FALSE, glm::value_ptr(projection * view));

        auto render = [&shaderCube, &texPalette, &view](opengl_starter::Node* node, auto& renderRef) -> void {
            if (node->mesh != nullptr)
            {
                glProgramUniformMatrix4fv(shaderCube.vertProg, glGetUniformLocation(shaderCube.vertProg, "model"), 1, GL_FALSE, glm::value_ptr(node->model));
                glProgramUniformMatrix4fv(shaderCube.vertProg, glGetUniformLocation(shaderCube.vertProg, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glProgramUniform1i(shaderCube.fragProg, glGetUniformLocation(shaderCube.fragProg, "texSampler"), 0);
                glBindTextureUnit(0, texPalette.textureName);
                glBindVertexArray(node->mesh->vao);
                glDrawElements(GL_TRIANGLES, node->mesh->indexCount, GL_UNSIGNED_INT, nullptr);
                glBindVertexArray(0);
            }
            for (auto c : node->children)
                renderRef(c, renderRef);
        };
        render(&root, render);

        for (auto ps : particleSystems)
            ps->Render(projection, view);

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
            glBindVertexArray(meshUnitCube->vao);
            glDrawElements(GL_TRIANGLES, meshUnitCube->indexCount, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
            glEnable(GL_CULL_FACE);
        }

        // ssao
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferSSAO.fbo);

        unsigned int attachments2[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, attachments2);

        glClearNamedFramebufferfv(framebufferSSAO.fbo, GL_COLOR, 0, clearColor2);

        glViewport(0, 0, frameWidth, frameHeight);

        glBindProgramPipeline(shaderSSAO.pipeline);
        glProgramUniformMatrix4fv(shaderSSAO.fragProg, glGetUniformLocation(shaderSSAO.fragProg, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glProgramUniform1f(shaderSSAO.fragProg, glGetUniformLocation(shaderSSAO.fragProg, "radius"), ssao.radius);
        glProgramUniform1f(shaderSSAO.fragProg, glGetUniformLocation(shaderSSAO.fragProg, "strength"), ssao.strength);
        glProgramUniform1f(shaderSSAO.fragProg, glGetUniformLocation(shaderSSAO.fragProg, "bias"), ssao.bias);
        glProgramUniform2fv(shaderSSAO.fragProg, glGetUniformLocation(shaderSSAO.fragProg, "winSize"), 1, glm::value_ptr(glm::vec2{ wnd.width, wnd.height }));
        glProgramUniform1i(shaderSSAO.fragProg, glGetUniformLocation(shaderSSAO.fragProg, "kernelSize"), ssao.kernelSize);
        for (int i = 0; i < ssao.kernelSize; ++i)
            glProgramUniform3fv(shaderSSAO.fragProg, glGetUniformLocation(shaderSSAO.fragProg, fmt::format("sampleSphere[{}]", i).c_str()), 1, glm::value_ptr(ssao.ssaoKernel[i]));

        glProgramUniform1i(shaderSSAO.fragProg, glGetUniformLocation(shaderSSAO.fragProg, "texNormal"), 0);
        glProgramUniform1i(shaderSSAO.fragProg, glGetUniformLocation(shaderSSAO.fragProg, "texDepth"), 1);
        glProgramUniform1i(shaderSSAO.fragProg, glGetUniformLocation(shaderSSAO.fragProg, "texNoise"), 2);
        glBindTextureUnit(0, texNormals.textureName);
        glBindTextureUnit(1, texDepth.textureName);
        glBindTextureUnit(2, texNoise->textureName);

        glBindVertexArray(dummyVao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // ssaoblur
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferSSAOBlur.fbo);

        glDrawBuffers(1, attachments2);

        glClearNamedFramebufferfv(framebufferSSAOBlur.fbo, GL_COLOR, 0, clearColor2);

        glViewport(0, 0, frameWidth, frameHeight);

        glBindProgramPipeline(shaderSSAOBlur.pipeline);
        glProgramUniform1i(shaderSSAOBlur.fragProg, glGetUniformLocation(shaderSSAOBlur.fragProg, "texSampler"), 0);
        glProgramUniform2fv(shaderSSAOBlur.fragProg, glGetUniformLocation(shaderSSAOBlur.fragProg, "winSize"), 1, glm::value_ptr(glm::vec2{ wnd.width, wnd.height }));
        glBindTextureUnit(0, texSSAO.textureName);

        glBindVertexArray(dummyVao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // Render to screen with post process
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearNamedFramebufferfv(0, GL_COLOR, 0, clearColor);
        glClearNamedFramebufferfv(0, GL_DEPTH, 0, &clearDepth);

        glViewport(0, 0, wnd.width, wnd.height);

        glBindProgramPipeline(shaderPost.pipeline);
        glProgramUniform1i(shaderPost.fragProg, glGetUniformLocation(shaderPost.fragProg, "enableAo"), (int)ssao.enableAo);
        glProgramUniform1i(shaderPost.fragProg, glGetUniformLocation(shaderPost.fragProg, "visualizeAo"), (int)ssao.visualizeAo);
        glProgramUniform1i(shaderPost.fragProg, glGetUniformLocation(shaderPost.fragProg, "texSampler"), 0);
        glProgramUniform1i(shaderPost.fragProg, glGetUniformLocation(shaderPost.fragProg, "texAO"), 1);
        glBindTextureUnit(0, texColor.textureName);
        glBindTextureUnit(1, texSSAOBlur.textureName);

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

    for (auto mesh : meshes)
    {
        delete mesh;
    }
    meshes.clear();

    glDeleteVertexArrays(1, &dummyVao);
}

void DrawSceneUI(opengl_starter::Node* node)
{
    static opengl_starter::Node* node_clicked = nullptr;

    ImGui::Begin("Scene");

    if (ImGui::Button("Add"))
    {
        auto parent = node_clicked ? node_clicked : node;

        auto n = new opengl_starter::Node{ fmt::format("{}.{}", parent->name, parent->children.size() + 1).c_str() };
        parent->children.push_back(n);
    }

    ImGui::BeginChild("scene_tree");

    auto createNode = [](opengl_starter::Node* n, bool defaultOpen, auto& createNodeRef) -> void {
        ImGuiTreeNodeFlags flags = {};
        if (defaultOpen)
            flags |= ImGuiTreeNodeFlags_DefaultOpen;
        if (node_clicked == n)
            flags |= ImGuiTreeNodeFlags_Selected;
        if (n->children.size() == 0)
            flags |= ImGuiTreeNodeFlags_Leaf;

        if (ImGui::TreeNodeEx((void*)(intptr_t)n, flags, fmt::format("{}", n->name).c_str()))
        {
            if (ImGui::IsItemClicked())
                node_clicked = n;

            for (const auto child : n->children)
            {
                createNodeRef(child, false, createNodeRef);
            }

            ImGui::TreePop();
        }
    };

    createNode(node, true, createNode);

    ImGui::EndChild();

    ImGui::End();

    ImGui::Begin("Object");
    if (node_clicked)
    {
        ImGui::Spacing();

        ImGui::Text(fmt::format("{} properties", node_clicked->name).c_str());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::DragFloat3("Pos", glm::value_ptr(node_clicked->pos), 0.1f, -1000.0f, 1000.0f);
        ImGui::DragFloat4("Rot", glm::value_ptr(node_clicked->rotq), 0.1f, -360.0f, 360.0f);
        ImGui::DragFloat3("Scl", glm::value_ptr(node_clicked->scale), 0.1f, 0.1f, 10.0f);
    }    

    ImGui::End();
}
