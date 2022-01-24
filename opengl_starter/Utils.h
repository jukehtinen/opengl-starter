#pragma once

#include "Common.h"

#include <filesystem>
#include <fstream>
#include <streambuf>

namespace Utils
{
    namespace String
    {
        static std::string ReplaceAll(const std::string& str, const std::string& from, const std::string& to)
        {
            std::string copyOfStr = str;
            size_t index = 0;
            while ((index = str.find(from, index)) != std::string::npos)
            {
                copyOfStr.replace(index, from.length(), to);
                index += to.length();
            }
            return copyOfStr;
        }

        static std::string Trim(const std::string& str)
        {
            const std::string trimmedChars = " ";

            std::string temp = str;

            temp.erase(temp.find_last_not_of(trimmedChars) + 1);
            temp.erase(0, temp.find_first_not_of(trimmedChars));

            return temp;
        }
    }

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

        static const std::vector<std::string> GetFiles(const std::string& path, const std::string& ext = "")
        {
            std::vector<std::string> files;
            for (const auto& entry : std::filesystem::directory_iterator(path))
            {
                if (ext.length() > 0 && entry.path().extension().string() != ext)
                    continue;

                files.push_back(String::ReplaceAll(entry.path().string(), "\\", "/"));
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

        static float RandomFloat(float min, float max)
        {
            float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            float diff = max - min;
            float r = random * diff;
            return min + r;
        }
    }

    namespace Shapes
    {
        static bool ContainsPoint(const glm::vec4& rect, const glm::vec2& pt)
        {
            return pt.x >= rect.x && pt.x <= (rect.x + rect.z) && pt.y >= rect.y && pt.y <= (rect.y + rect.w);
        }
    }
}