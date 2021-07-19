#pragma once

#include "Common.h"

namespace opengl_starter
{
    struct Node;
    struct Buffer;
    struct Shader;
    struct Texture;

    struct Emitter;
    using ModuleFunc = std::function<void(Emitter&, float)>;

    struct Modules
    {
        static void PointSpawner(Emitter& emitter, float delta);
        static void BoxSpawner(Emitter& emitter, float delta);
        static void CircleSpawner(Emitter& emitter, float delta);

        static void BasicUpdater(Emitter& emitter, float delta);
        static void FloorUpdater(Emitter& emitter, float delta);
        static void AttractorUpdater(Emitter& emitter, float delta);
    };

    struct Particle
    {
        glm::vec4 Position{ 0.0f };
        glm::vec4 Velocity{ 0.0f };
        glm::vec4 Color{ 0.0f };
        glm::vec4 Param{ 0.0f };
        glm::vec4 Param2{ 0.0f };
    };

    struct ColorStop
    {
        float t{ 0.0f };
        glm::vec4 color;
    };

    enum class BlendMode
    {
        GL_SRC_ALPHA_GL_ONE,
        GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA,
        GL_ONE_GL_ONE
    };

    struct Emitter
    {
        std::vector<void (*)(Emitter&, float)> modules;

        int maxSpawn = 5;
        int maxParticles = 1000;
        float spawnRate = 0.2f;
        bool worldSpace = false;

        glm::vec2 life = { 0.3f, 0.4f };
        glm::vec2 rotation = { -0.4f, 0.4f };
        glm::vec4 size = { 0.3f, 0.4f, 0.3f, 0.4f };
        glm::vec3 velocityMin = { -1.0f, 1.0f, -1.0f };
        glm::vec3 velocityMax = { 1.0f, 5.0f, 1.0f };
        glm::vec4 gravity = { 0.0f, -9.81f, 0.0f, 0.0f };
        std::vector<ColorStop> colors;       

        float spawnCooldown = 0.0f;
        int firstUnused = 0;

        opengl_starter::Shader* _shader = nullptr;
        opengl_starter::Texture* _texture = nullptr;
        BlendMode blendMode = BlendMode::GL_SRC_ALPHA_GL_ONE;
        std::shared_ptr<Buffer> _particleBuffer = nullptr;
        void* _mappedBuffer = nullptr;
        Node* parentNode = nullptr;
    };

    class ParticleSystem
    {
    public:
        ParticleSystem();
        ~ParticleSystem();

        void Start();
        void Stop();

        bool IsRunning() const
        {
            return _isRunning;
        }

        void Update(float delta);
        void Render(const glm::mat4& projection, const glm::mat4& view);

        void Load(const std::string& fileName, opengl_starter::Node* node);
        void Save(const std::string& fileName);

        Emitter* NewEmitter();
        std::vector<Emitter*>& Emitters()
        {
            return emitters;
        }

        void OnUI();

    private:
        std::vector<Emitter*> emitters;
        float _isRunning = false;
        GLuint _vao{ 0 };
        int _currentListIndex = -1;

        std::string _fileName;
    };
}