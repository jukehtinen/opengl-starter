#include "ParticleSystem.h"

#include "Buffer.h"
#include "Node.h"
#include "Shader.h"
#include "Texture.h"
#include "Utils.h"

#include "imgui.h"

#include <nlohmann/json.hpp>

namespace opengl_starter
{
    glm::vec4 GetColor(const std::vector<ColorStop>& colors, float t)
    {
        if (colors.size() == 1)
            return colors[0].color;
        if (t >= 1.0f)
            return colors[colors.size() - 1].color;

        for (int i = 0; i < colors.size() - 1; i++)
        {
            if (colors[i].t <= t && colors[i + 1].t > t)
            {
                float range = colors[i + 1].t - colors[i].t;
                return glm::mix(colors[i].color, colors[i + 1].color, (t - colors[i].t) / range);
            }
        }
        return colors[0].color;
    }

    Particle& InitParticle(Emitter& emitter)
    {
        auto particles = (Particle*)emitter._mappedBuffer;

        auto& particle = particles[emitter.firstUnused];

        particle.Param.x = Utils::Rng::RandomFloat(emitter.life.x, emitter.life.y);
        particle.Param.y = particles[emitter.firstUnused].Param.x;
        particle.Param.z = Utils::Rng::RandomFloat(emitter.size.x, emitter.size.y);
        particle.Param.w = Utils::Rng::RandomFloat(emitter.size.z, emitter.size.w);
        particle.Param2.x = Utils::Rng::RandomFloat(emitter.rotation.x, emitter.rotation.y);
        particle.Position = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
        particle.Velocity = glm::vec4{
            Utils::Rng::RandomFloat(emitter.velocityMin.x, emitter.velocityMax.x),
            Utils::Rng::RandomFloat(emitter.velocityMin.y, emitter.velocityMax.y),
            Utils::Rng::RandomFloat(emitter.velocityMin.z, emitter.velocityMax.z),
            0.0f
        };

        emitter.firstUnused++;

        return particle;
    }

    void Modules::PointSpawner(Emitter& emitter, float delta)
    {
        emitter.spawnCooldown += delta;
        if (emitter.spawnCooldown > emitter.spawnRate)
        {
            emitter.spawnCooldown = 0.0f;

            for (int i = 0; i < emitter.maxSpawn; i++)
            {
                if (emitter.firstUnused >= emitter.maxParticles)
                    continue;

                auto& particle = InitParticle(emitter);

                particle.Position = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
            }
        }
    }

    void Modules::BoxSpawner(Emitter& emitter, float delta)
    {
        emitter.spawnCooldown += delta;
        if (emitter.spawnCooldown > emitter.spawnRate)
        {
            emitter.spawnCooldown = 0.0f;
            for (int i = 0; i < emitter.maxSpawn; i++)
            {
                if (emitter.firstUnused >= emitter.maxParticles)
                    continue;

                auto& particle = InitParticle(emitter);

                particle.Position = glm::vec4{
                    Utils::Rng::RandomFloat(-10.0f, 10.0f),
                    Utils::Rng::RandomFloat(0.0f, 10.0f),
                    Utils::Rng::RandomFloat(-10.0f, 10.0f),
                    0.0f
                };
            }
        }
    }

    void Modules::CircleSpawner(Emitter& emitter, float delta)
    {
        emitter.spawnCooldown += delta;
        if (emitter.spawnCooldown > emitter.spawnRate)
        {
            emitter.spawnCooldown = 0.0f;
            for (int i = 0; i < emitter.maxSpawn; i++)
            {
                if (emitter.firstUnused >= emitter.maxParticles)
                    continue;

                auto& particle = InitParticle(emitter);

                float ang = Utils::Rng::RandomFloat(0.0f, glm::pi<float>() * 2.0f);
                glm::vec4 center{ 0.0f };
                float d = Utils::Rng::RandomFloat(0.0f, 10.0f);
                particle.Position = center + glm::vec4(d * glm::sin(ang), 0.0f, d * glm::cos(ang), 0.0f);
            }
        }
    }

    void Modules::BasicUpdater(Emitter& emitter, float delta)
    {
        auto particles = (Particle*)emitter._mappedBuffer;

        for (int i = 0; i < emitter.firstUnused; i++)
        {
            auto& p = particles[i];
            if (p.Param.x <= 0.0f)
            {
                std::swap(particles[i], particles[emitter.firstUnused - 1]);
                p = particles[i];
                emitter.firstUnused--;
            }

            p.Param.x -= delta;
            p.Color = GetColor(emitter.colors, 1.0f - (p.Param.x / p.Param.y));
            p.Velocity += emitter.gravity * delta * 0.5f;
            p.Position += p.Velocity * delta;
            p.Param2.y += p.Param2.x * delta;
        }
    }

    void Modules::FloorUpdater(Emitter& emitter, float delta)
    {
        auto particles = (Particle*)emitter._mappedBuffer;

        for (int i = 0; i < emitter.firstUnused; i++)
        {
            auto& p = particles[i];
            if (p.Param.x <= 0.0f)
            {
                std::swap(particles[i], particles[emitter.firstUnused - 1]);
                p = particles[i];
                emitter.firstUnused--;
            }

            p.Param.x -= delta;
            p.Color = GetColor(emitter.colors, 1.0f - (p.Param.x / p.Param.y));
            p.Velocity += emitter.gravity * delta * 0.5f;
            p.Position += p.Velocity * delta;
            p.Param2.y += p.Param2.x * delta;

            if (p.Position.y < 0.0f)
            {
                glm::vec4 force = p.Velocity;
                float normalFactor = glm::dot(force, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
                if (normalFactor < 0.0f)
                    force -= glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * normalFactor;

                float m_bounceFactor = 0.5f;
                float velFactor = glm::dot(p.Velocity, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
                if (velFactor < 0.0)
                    p.Velocity -= glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * (1.0f + m_bounceFactor) * velFactor;
            }
        }
    }

    void Modules::AttractorUpdater(Emitter& emitter, float delta)
    {
        auto particles = (Particle*)emitter._mappedBuffer;

        for (int i = 0; i < emitter.firstUnused; i++)
        {
            auto& p = particles[i];
            if (p.Param.x <= 0.0f)
            {
                std::swap(particles[i], particles[emitter.firstUnused - 1]);
                p = particles[i];
                emitter.firstUnused--;
            }

            p.Param.x -= delta;
            p.Color = GetColor(emitter.colors, 1.0f - (p.Param.x / p.Param.y));
            p.Velocity += emitter.gravity * delta * 0.5f;
            p.Position += p.Velocity * delta;
            p.Param2.y += p.Param2.x * delta;

            glm::vec4 attactor{ 0.0f, 5.0f, 0.0f, 0.5f };
            glm::vec4 off{ 1.0f };
            off.x = attactor.x - p.Position.x;
            off.y = attactor.y - p.Position.y;
            off.z = attactor.z - p.Position.z;
            float dist = glm::dot(off, off);

            //if (fabs(dist) > 0.00001)
            dist = attactor.w / dist;

            p.Velocity += off * dist;
        }
    }

    ParticleSystem::ParticleSystem()
    {
        glCreateVertexArrays(1, &_vao);
    }

    ParticleSystem::~ParticleSystem()
    {
        glDeleteVertexArrays(1, &_vao);
    }

    void ParticleSystem::Start()
    {
        _isRunning = true;
    }

    void ParticleSystem::Stop()
    {
        _isRunning = false;
    }

    void ParticleSystem::Update(float delta)
    {
        if (!_isRunning)
            return;

        for (auto emitter : emitters)
        {
            for (auto mod : emitter->modules)
            {
                mod(*emitter, delta);
            }
        }
    }

    void ParticleSystem::Render(const glm::mat4& projection, const glm::mat4& view)
    {
        glEnable(GL_BLEND);

        for (auto emitter : emitters)
        {
            if (emitter->firstUnused == 0)
                continue;

            switch (emitter->blendMode)
            {
            case BlendMode::GL_ONE_GL_ONE:
                glBlendFunc(GL_ONE, GL_ONE);
                break;
            case BlendMode::GL_SRC_ALPHA_GL_ONE:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                break;
            case BlendMode::GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA:
                glBlendEquation(GL_FUNC_ADD);
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                break;
            default:
                break;
            }

            glBindProgramPipeline(emitter->_shader->pipeline);

            glDepthMask(GL_FALSE);
            glProgramUniformMatrix4fv(emitter->_shader->vertProg, glGetUniformLocation(emitter->_shader->vertProg, "p"), 1, GL_FALSE, glm::value_ptr(projection));
            glProgramUniformMatrix4fv(emitter->_shader->vertProg, glGetUniformLocation(emitter->_shader->vertProg, "v"), 1, GL_FALSE, glm::value_ptr(view));
            glProgramUniformMatrix4fv(emitter->_shader->vertProg, glGetUniformLocation(emitter->_shader->vertProg, "model"), 1, GL_FALSE, glm::value_ptr(emitter->parentNode->model));
            auto block_index = glGetProgramResourceIndex(emitter->_shader->vertProg, GL_SHADER_STORAGE_BLOCK, "particleBuffer");
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, emitter->_particleBuffer->buffer, 0, 1000 * sizeof(opengl_starter::Particle));
            glShaderStorageBlockBinding(emitter->_shader->vertProg, block_index, 0);
            glProgramUniform1i(emitter->_shader->fragProg, glGetUniformLocation(emitter->_shader->fragProg, "texSampler"), 0);
            glBindTextureUnit(0, emitter->_texture->textureName);
            glBindVertexArray(_vao);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, emitter->firstUnused);
            glBindVertexArray(0);
            glDepthMask(GL_TRUE);
        }

        glDisable(GL_BLEND);
    }

    Emitter* ParticleSystem::NewEmitter()
    {
        auto emitter = new Emitter();
        emitters.push_back(emitter);
        emitter->_particleBuffer = std::make_shared<Buffer>(emitter->maxParticles * sizeof(Particle));
        emitter->_mappedBuffer = glMapNamedBufferRange(emitter->_particleBuffer->buffer, 0, emitter->maxParticles * sizeof(Particle),
            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

        return emitter;
    }

    void ParticleSystem::OnUI()
    {
        ImGui::Begin("Particles");

        char nameBuffer[128];
        strncpy_s(nameBuffer, 128, _fileName.c_str(), _fileName.length());
        if (ImGui::InputText("Filename", nameBuffer, 128))
        {
            _fileName = nameBuffer;
        }

        if (ImGui::Button("Save"))
            Save(_fileName);

        if (ImGui::BeginListBox("Emitters"))
        {
            for (int n = 0; n < emitters.size(); n++)
            {
                bool selected = (_currentListIndex == n);
                if (ImGui::Selectable(fmt::format("Emitter {}", n).c_str(), selected))
                    _currentListIndex = n;

                if (selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }

        if (_currentListIndex != -1)
        {
            auto& emitter = *emitters[_currentListIndex];

            ImGui::Text(fmt::format("firstUnused {}", emitter.firstUnused).c_str());

            ImGui::DragInt("maxSpawn", &emitter.maxSpawn, 1.0f, 0, 1000);
            ImGui::DragFloat("spawnRate", &emitter.spawnRate, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat2("life", glm::value_ptr(emitter.life), 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat2("rotation", glm::value_ptr(emitter.rotation), 0.01f, -100.0f, 100.0f);
            ImGui::DragFloat3("velocityMin", glm::value_ptr(emitter.velocityMin), 0.01f, -100.0f, 100.0f);
            ImGui::DragFloat3("velocityMax", glm::value_ptr(emitter.velocityMax), 0.01f, -100.0f, 100.0f);
            ImGui::DragFloat4("gravity", glm::value_ptr(emitter.gravity), 0.01f, -100.0f, 100.0f);
            ImGui::DragFloat4("size", glm::value_ptr(emitter.size), 0.01f, 0.0f, 100.0f);

            const char* blendModes[] = { "GL_SRC_ALPHA_GL_ONE", "GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA", "GL_ONE_GL_ONE" };

            ImGui::Spacing();
            ImGui::Combo("BlendMode", (int*)&emitter.blendMode, blendModes, IM_ARRAYSIZE(blendModes));

            ImGui::Spacing();
            for (int i = 0; i < emitter.colors.size(); i++)
            {
                ImGui::DragFloat(fmt::format("stop{}", i).c_str(), &emitter.colors[i].t);
                ImGui::ColorEdit4(fmt::format("color{}", i).c_str(), glm::value_ptr(emitter.colors[i].color));
            }

            if (ImGui::Button("Add"))
                emitter.colors.emplace_back(ColorStop{});
        }
        ImGui::End();
    }

    void ParticleSystem::Load(const std::string& fileName, opengl_starter::Node* node)
    {
        _fileName = fileName;

        std::ifstream i(fileName);
        nlohmann::json json;
        i >> json;

        auto getVec2 = [](const nlohmann::json& j) -> glm::vec2 {
            if (!j.is_array() || j.size() < 2)
                return {};
            return { j[0].get<float>(), j[1].get<float>() };
        };

        auto getVec3 = [](const nlohmann::json& j) -> glm::vec3 {
            if (!j.is_array() || j.size() < 3)
                return {};
            return { j[0].get<float>(), j[1].get<float>(), j[2].get<float>() };
        };

        auto getVec4 = [](const nlohmann::json& j) -> glm::vec4 {
            if (!j.is_array() || j.size() < 4)
                return {};
            return { j[0].get<float>(), j[1].get<float>(), j[2].get<float>(), j[3].get<float>() };
        };

        for (auto& e : json["emitters"])
        {
            auto emitter = NewEmitter();

            emitter->parentNode = node;

            emitter->maxSpawn = e["maxSpawn"].get<int>();
            emitter->maxParticles = e["maxParticles"].get<int>();
            emitter->spawnRate = e["spawnRate"].get<float>();
            emitter->worldSpace = e["worldSpace"].get<bool>();

            emitter->life = getVec2(e["life"]);
            emitter->rotation = getVec2(e["rotation"]);
            emitter->size = getVec4(e["size"]);
            emitter->velocityMin = getVec3(e["velocityMin"]);
            emitter->velocityMax = getVec3(e["velocityMax"]);
            emitter->gravity = glm::vec4{ getVec3(e["gravity"]), 0.0f };

            emitter->blendMode = (BlendMode)e["blendMode"].get<int>();

            emitter->colors.clear();
            for (auto& color : e["colors"])
            {
                emitter->colors.push_back(ColorStop{ color["t"].get<float>(), { color["color"][0].get<float>(),
                                                                                  color["color"][1].get<float>(),
                                                                                  color["color"][2].get<float>(),
                                                                                  color["color"][3].get<float>() } });
            }

            emitter->modules.clear();
            for (auto& mod : e["modules"])
            {
                auto name = mod.get<std::string>();
                if (name == "PointSpawner")
                    emitter->modules.push_back(Modules::PointSpawner);
                else if (name == "BoxSpawner")
                    emitter->modules.push_back(Modules::BoxSpawner);
                else if (name == "BoxSpawner")
                    emitter->modules.push_back(Modules::CircleSpawner);

                else if (name == "BasicUpdater")
                    emitter->modules.push_back(Modules::BasicUpdater);
                else if (name == "FloorUpdater")
                    emitter->modules.push_back(Modules::FloorUpdater);
                else if (name == "AttractorUpdater")
                    emitter->modules.push_back(Modules::AttractorUpdater);
            }

            emitter->_texture = new opengl_starter::Texture(e["texture"].get<std::string>());
            emitter->_shader = new opengl_starter::Shader("assets/particles.vert", "assets/particles.frag");
        }
    }

    void ParticleSystem::Save(const std::string& fileName)
    {
        auto emitterArray = nlohmann::json::array();
        for (auto& emitter : emitters)
        {
            auto e = nlohmann::json::object();

            e["maxSpawn"] = emitter->maxSpawn;
            e["maxParticles"] = emitter->maxParticles;
            e["spawnRate"] = emitter->spawnRate;
            e["worldSpace"] = emitter->worldSpace;

            e["life"] = { emitter->life.x, emitter->life.y };
            e["rotation"] = { emitter->rotation.x, emitter->rotation.y };
            e["size"] = { emitter->size.x, emitter->size.y, emitter->size.z, emitter->size.w };
            e["velocityMin"] = { emitter->velocityMin.x, emitter->velocityMin.y, emitter->velocityMin.z };
            e["velocityMax"] = { emitter->velocityMax.x, emitter->velocityMax.y, emitter->velocityMax.z };
            e["gravity"] = { emitter->gravity.x, emitter->gravity.y, emitter->gravity.z };

            e["blendMode"] = (int)emitter->blendMode;

            e["texture"] = emitter->_texture->sourceFile;

            auto colorArray = nlohmann::json::array();
            for (auto& cs : emitter->colors)
            {
                auto c = nlohmann::json::object();
                c["t"] = cs.t;
                c["color"] = { cs.color.x, cs.color.y, cs.color.z, cs.color.w };
                colorArray.push_back(c);
            }
            e["colors"] = colorArray;

            auto moduleArray = nlohmann::json::array();
            for (auto& md : emitter->modules)
            {
                if (md == Modules::PointSpawner)
                    moduleArray.push_back("PointSpawner");
                else if (md == Modules::BoxSpawner)
                    moduleArray.push_back("BoxSpawner");
                else if (md == Modules::CircleSpawner)
                    moduleArray.push_back("CircleSpawner");

                else if (md == Modules::BasicUpdater)
                    moduleArray.push_back("BasicUpdater");
                else if (md == Modules::FloorUpdater)
                    moduleArray.push_back("FloorUpdater");
                else if (md == Modules::AttractorUpdater)
                    moduleArray.push_back("AttractorUpdater");
            }
            e["modules"] = moduleArray;

            emitterArray.push_back(e);
        }

        nlohmann::json json;
        json["name"] = "ps";
        json["emitters"] = emitterArray;

        _fileName = Utils::String::Trim(fileName);
        std::ofstream o(_fileName);
        o << json;
    }
}