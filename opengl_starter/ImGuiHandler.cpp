#include "ImGuiHandler.h"

#include "Buffer.h"
#include "Shader.h"
#include "Terrain.h"
#include "Texture.h"

#include "imgui.h"

#include <vector>

namespace opengl_starter
{
    ImGuiHandler::ImGuiHandler(GLFWwindow* window)
        : _window(window)
    {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

        io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
        io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
        io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
        io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
        io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
        io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
        io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
        io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
        io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

        io.SetClipboardTextFn = [](void* userdata, const char* text) { glfwSetClipboardString(static_cast<GLFWwindow*>(userdata), text); };
        io.GetClipboardTextFn = [](void* userdata) -> const char* { return glfwGetClipboardString(static_cast<GLFWwindow*>(userdata)); };
        io.ClipboardUserData = window;

        GLFWerrorfun prev_error_callback = glfwSetErrorCallback(NULL);
        _mouseCursors.resize(ImGuiMouseCursor_COUNT);
        _mouseJustPressed.resize(ImGuiMouseButton_COUNT);

        _mouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        _mouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
        _mouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
        _mouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        _mouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
#if GLFW_HAS_NEW_CURSORS
        _mouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
        _mouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
        _mouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
        _mouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
#else
        _mouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        _mouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        _mouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        _mouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
#endif
        glfwSetErrorCallback(prev_error_callback);

        _shader = new opengl_starter::Shader("assets/imgui.vert", "assets/imgui.frag");

        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        _texFont = new opengl_starter::Texture{ width, height, pixels };
        io.Fonts->SetTexID(reinterpret_cast<ImTextureID>(_texFont));
    }

    ImGuiHandler::~ImGuiHandler()
    {
        glUnmapNamedBuffer(_vertexBuffer->buffer);
        glUnmapNamedBuffer(_indexBuffer->buffer);
        glDeleteVertexArrays(1, &_vao);

        delete _texFont;
        delete _shader;

        for (auto cursor = 0; cursor < ImGuiMouseCursor_COUNT; cursor++)
            glfwDestroyCursor(_mouseCursors[cursor]);
    }

    bool ImGuiHandler::OnMouse(float xpos, float ypos)
    {
        ImGuiIO& io = ImGui::GetIO();

        const ImVec2 mouse_pos_backup = io.MousePos;
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

        const bool focused = glfwGetWindowAttrib(_window, GLFW_FOCUSED) != 0;

        if (focused)
        {
            if (io.WantSetMousePos)
            {
                glfwSetCursorPos(_window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);
            }
            else
            {
                double mouse_x, mouse_y;
                glfwGetCursorPos(_window, &mouse_x, &mouse_y);
                io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
            }
        }

        if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) && glfwGetInputMode(_window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
        {
            ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
            if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
            {
                glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            }
            else
            {
                glfwSetCursor(_window, _mouseCursors[imgui_cursor] ? _mouseCursors[imgui_cursor] : _mouseCursors[ImGuiMouseCursor_Arrow]);
                glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }

        return io.WantCaptureMouse;
    }

    bool ImGuiHandler::OnScroll(float xoffset, float yoffset)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseWheelH += (float)xoffset;
        io.MouseWheel += (float)yoffset;
        return io.WantCaptureMouse;
    }

    bool ImGuiHandler::OnKey(int key, int scancode, int action, int mods)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (action == GLFW_PRESS)
            io.KeysDown[key] = true;
        if (action == GLFW_RELEASE)
            io.KeysDown[key] = false;

        io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
        io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
        io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
#ifdef _WIN32
        io.KeySuper = false;
#else
        io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
#endif

        return io.WantCaptureKeyboard;
    }

    bool ImGuiHandler::OnChar(unsigned int chr)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(chr);
        return io.WantCaptureKeyboard;
    }

    void ImGuiHandler::Update(float delta)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = delta;

        for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
        {
            // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
            io.MouseDown[i] = _mouseJustPressed[i] || glfwGetMouseButton(_window, i) != 0;
            _mouseJustPressed[i] = false;
        }
    }

    void ImGuiHandler::NewFrame()
    {
        ImGuiIO& io = ImGui::GetIO();

        int w, h;
        int display_w, display_h;
        glfwGetWindowSize(_window, &w, &h);
        glfwGetFramebufferSize(_window, &display_w, &display_h);
        io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
        if (w > 0 && h > 0)
            io.DisplayFramebufferScale = ImVec2(static_cast<float>(display_w / w), static_cast<float>(display_h / h));

        ImGui::NewFrame();

        // bool show_demo_window = true;
        // ImGui::ShowDemoWindow(&show_demo_window);
    }

    void ImGuiHandler::Render()
    {
        ImGuiIO& io = ImGui::GetIO();

        ImGui::Render();

        auto drawData = ImGui::GetDrawData();

        // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
        int fb_width = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
        int fb_height = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);
        if (fb_width <= 0 || fb_height <= 0)
            return;

        // Will project scissor/clipping rectangles into framebuffer space
        ImVec2 clip_off = drawData->DisplayPos;         // (0,0) unless using multi-viewports
        ImVec2 clip_scale = drawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

        GLsizeiptr vertBufferSizeRequired = 0;
        GLsizeiptr indexBufferSizeRequired = 0;
        for (int i = 0; i < drawData->CmdListsCount; i++)
        {
            vertBufferSizeRequired += drawData->CmdLists[i]->VtxBuffer.Size * sizeof(ImDrawVert);
            indexBufferSizeRequired += drawData->CmdLists[i]->IdxBuffer.Size * sizeof(ImDrawIdx);
        }

        if (vertBufferSizeRequired > 0 && indexBufferSizeRequired > 0)
        {
            if (_vertexBuffer == nullptr || vertBufferSizeRequired >= _vertexBuffer->bufferSize ||
                _indexBuffer == nullptr || indexBufferSizeRequired >= _indexBuffer->bufferSize)
            {
                // Add bit extra capacity to buffers.
                CreateBuffers(static_cast<GLsizeiptr>(vertBufferSizeRequired * 1.5), static_cast<GLsizeiptr>(indexBufferSizeRequired * 1.5));
            }
        }

        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);

        glBindProgramPipeline(_shader->pipeline);
        glBindTextureUnit(0, _texFont->textureName);

        const auto projection = glm::ortho(drawData->DisplayPos.x, drawData->DisplayPos.x + drawData->DisplaySize.x, drawData->DisplayPos.y + drawData->DisplaySize.y, drawData->DisplayPos.y);
        glProgramUniformMatrix4fv(_shader->vertProg, glGetUniformLocation(_shader->vertProg, "mvp"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(_vao);

        size_t vertCmdListOffset = 0;
        size_t idxCmdListOffset = 0;

        ImDrawVert* vertexPtr = reinterpret_cast<ImDrawVert*>(_mappedVertexBuffer);
        ImDrawIdx* indexPtr = reinterpret_cast<ImDrawIdx*>(_mappedIndexBuffer);

        for (int n = 0; n < drawData->CmdListsCount; n++)
        {
            const auto cmdList = drawData->CmdLists[n];

            std::memcpy(vertexPtr + vertCmdListOffset, cmdList->VtxBuffer.Data, (GLsizeiptr)cmdList->VtxBuffer.Size * (int)sizeof(ImDrawVert));
            std::memcpy(indexPtr + idxCmdListOffset, cmdList->IdxBuffer.Data, (GLsizeiptr)cmdList->IdxBuffer.Size * (int)sizeof(ImDrawIdx));

            for (int i = 0; i < cmdList->CmdBuffer.Size; i++)
            {
                const auto pcmd = &cmdList->CmdBuffer[i];
                if (pcmd->UserCallback != NULL)
                {
                    spdlog::warn("[ImGui] UserCallbacks not implemented.");
                    continue;
                }

                ImVec4 clip_rect;
                clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

                if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
                {
                    glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));

                    auto indicesOffset = reinterpret_cast<void*>((pcmd->IdxOffset + idxCmdListOffset) * sizeof(ImDrawIdx));
                    auto baseVert = static_cast<GLint>(pcmd->VtxOffset + vertCmdListOffset);
                    glDrawElementsBaseVertex(GL_TRIANGLES, static_cast<GLsizei>(pcmd->ElemCount), sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
                        indicesOffset, baseVert);
                }
            }

            vertCmdListOffset += (GLsizeiptr)cmdList->VtxBuffer.Size;
            idxCmdListOffset += (GLsizeiptr)cmdList->IdxBuffer.Size;
        }

        glBindVertexArray(0);

        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_SCISSOR_TEST);
    }

    void ImGuiHandler::CreateBuffers(GLsizeiptr vertBufferSize, GLsizeiptr indexBufferSize)
    {
        if (_vao)
        {
            glUnmapNamedBuffer(_vertexBuffer->buffer);
            glUnmapNamedBuffer(_indexBuffer->buffer);
            glDeleteVertexArrays(1, &_vao);
        }

        spdlog::debug("[ImGui] CreateBuffers {}, {}.", vertBufferSize, indexBufferSize);

        _vertexBuffer = std::make_shared<Buffer>(vertBufferSize);
        _indexBuffer = std::make_shared<Buffer>(indexBufferSize);

        glCreateVertexArrays(1, &_vao);

        glVertexArrayVertexBuffer(_vao, 0, _vertexBuffer->buffer, 0, sizeof(ImDrawVert));
        glVertexArrayElementBuffer(_vao, _indexBuffer->buffer);

        glEnableVertexArrayAttrib(_vao, 0);
        glEnableVertexArrayAttrib(_vao, 1);
        glEnableVertexArrayAttrib(_vao, 2);

        glVertexArrayAttribFormat(_vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(ImDrawVert, pos));
        glVertexArrayAttribFormat(_vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(ImDrawVert, uv));
        glVertexArrayAttribFormat(_vao, 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, offsetof(ImDrawVert, col));

        glVertexArrayAttribBinding(_vao, 0, 0);
        glVertexArrayAttribBinding(_vao, 1, 0);
        glVertexArrayAttribBinding(_vao, 2, 0);

        _mappedVertexBuffer = glMapNamedBufferRange(_vertexBuffer->buffer, 0, vertBufferSize,
            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

        _mappedIndexBuffer = glMapNamedBufferRange(_indexBuffer->buffer, 0, indexBufferSize,
            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    }
}
