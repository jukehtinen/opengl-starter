#pragma once

#include "Buffer.h"
#include "Mesh.h"
#include "Node.h"

namespace opengl_starter
{
    struct GltfLoader
    {
        static opengl_starter::Node* Load(const std::string& filename, opengl_starter::Node* parentNode, std::vector<opengl_starter::Mesh*>& outMeshes, 
            bool mergeAnimations = false);
    };
}