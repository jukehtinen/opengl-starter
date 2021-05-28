#pragma once

#include "Common.h"

#include <array>
#include <fstream>
#include <sstream>
#include <map>

namespace opengl_starter
{
    // https://www.angelcode.com/products/bmfont/doc/file_format.html
    struct Font
    {
        struct Char
        {
            int x = 0;
            int y = 0;
            int width = 0;
            int height = 0;
            int xOffset = 0;
            int yOffset = 0;
            int xAdvance = 0;
            int page = 0;
        };

        Font(const std::string& fontfile)
        {
            std::ifstream stream(fontfile);
            if (!stream.is_open())
            {
                spdlog::error("[Font] Failed to load file {}", fontfile);
                return;
            }

            auto getIntProp = [](const std::string_view s) -> std::pair<std::string_view, int> {
                auto i = s.find('=');
                return std::make_pair(s.substr(0, i), std::atoi(s.substr(i + 1).data()));
            };

            std::string item;
            std::string line;
            while (std::getline(stream, line))
            {
                std::stringstream sstream;
                sstream << line;

                sstream >> item;
                if (item == "common")
                {
                    while (!sstream.eof())
                    {
                        sstream >> item;
                        const auto [key, value] = getIntProp(item);

                        if (key == "lineHeight")
                            lineHeight = value;
                        else if (key == "base")
                            base = value;
                        else if (key == "scaleW")
                            scaleW = value;
                        else if (key == "scaleH")
                            scaleH = value;
                    }
                }
                else if (item == "char")
                {
                    int id = 0;
                    while (!sstream.eof() && id < chars.size())
                    {
                        sstream >> item;
                        const auto [key, value] = getIntProp(item);

                        if (key == "id")
                            id = value;
                        else if (key == "x")
                            chars[id].x = value;
                        else if (key == "y")
                            chars[id].y = value;
                        else if (key == "width")
                            chars[id].width = value;
                        else if (key == "height")
                            chars[id].height = value;
                        else if (key == "xoffset")
                            chars[id].xOffset = value;
                        else if (key == "yoffset")
                            chars[id].yOffset = value;
                        else if (key == "xadvance")
                            chars[id].xAdvance = value;
                        else if (key == "page")
                            chars[id].page = value;

                        if (id >= chars.size())
                            spdlog::error("[Font] Id {} is larger than array size {}", id, chars.size());
                    }
                }
                else if (item == "kerning")
                {
                    int first = 0;
                    int second = 0;
                    int amount = 0;

                    while (!sstream.eof())
                    {
                        sstream >> item;
                        const auto [key, value] = getIntProp(item);

                        if (key == "first")
                            first = value;
                        else if (key == "second")
                            second = value;
                        else if (key == "amount")
                            amount = value;
                    }

                    kernings[std::make_pair(first, second)] = amount;
                }
            }
        }

        int lineHeight = 0;
        int base = 0;
        int scaleW = 0;
        int scaleH = 0;
        int pages = 0;

        std::map<std::pair<int, int>, int> kernings;
        std::array<Char, 256> chars;
    };
}