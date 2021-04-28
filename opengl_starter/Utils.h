#pragma once

#include "Common.h"

#include <fstream>
#include <streambuf>

namespace Utils
{
    namespace File
    {
        static std::string LoadString(const std::string& filename)
        {
            std::ifstream stream(filename);
            if (!stream.is_open())
            {
                DEBUGGER;
                spdlog::error("[LoadString] Failed to load file {}", filename);
                return "";
            }

            std::string output;
            stream.seekg(0, std::ios::end);
            output.reserve(stream.tellg());
            stream.seekg(0, std::ios::beg);

            output.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

            return output;
        }
    }

    namespace Rng
    {
        static float RandomFloat()
        {
            return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        }
    }

}