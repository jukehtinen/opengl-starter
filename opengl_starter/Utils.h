#pragma once

#include "Common.h"

#include <filesystem>
#include <fstream>
#include <streambuf>

namespace Utils
{
    namespace File
    {
        static std::string LoadText(const std::string& filename)
        {
            std::ifstream stream(filename);
            if (!stream.is_open())
            {
                DEBUGGER;
                spdlog::error("[LoadText] Failed to load file {}", filename);
                return "";
            }

            std::string output;
            stream.seekg(0, std::ios::end);
            output.reserve(stream.tellg());
            stream.seekg(0, std::ios::beg);

            output.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

            return output;
        }

        static const std::vector<std::string> GetFiles(const std::string& path)
        {
            std::vector<std::string> files;
            for (const auto& entry : std::filesystem::directory_iterator(path))
            {
                files.push_back(entry.path().string());
            }
            return files;
        }
    }

    namespace Rng
    {
        static float RandomFloat()
        {
            return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        }

        static float RandomFloat(float a, float b)
        {
            float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            float diff = b - a;
            float r = random * diff;
            return a + r;
        }
    }

    namespace String
    {
        static std::string Trim(const std::string& str)
        {
            const std::string trimmedChars = " ";

            std::string temp = str;

            temp.erase(temp.find_last_not_of(trimmedChars) + 1);
            temp.erase(0, temp.find_first_not_of(trimmedChars));

            return temp;
        }
    }
}