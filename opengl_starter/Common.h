#pragma once

#include <array>
#include <chrono>
#include <map>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 4201) // warning C4201: nonstandard extension used: nameless struct/union

#include "glad/glad.h"

#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/color_space.hpp"

#include "fmt/format.h"

#include "spdlog/spdlog.h"

#include "fmt/format.h"

#pragma warning(pop)

#define DEBUGGER __debugbreak();