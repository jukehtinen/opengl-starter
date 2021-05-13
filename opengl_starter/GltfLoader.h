#pragma once

#include "Buffer.h"
#include "Mesh.h"
#include "Node.h"

namespace opengl_starter
{
    struct GltfLoader
    {
        static void Load(const std::string& filename, opengl_starter::Node* parentNode, std::vector<opengl_starter::Mesh*>& outMeshes);
    };
}