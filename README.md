# opengl-starter

OpenGL starter project with `vcpkg, cmake, glfw, glm, glad, gltf, lodepng, etc`.

### VS2019 (with C++ workload)

- Install [vcpkg](https://github.com/Microsoft/vcpkg)
- Either do the `vcpkg intergrate install` and "Open a local folder" in VS.
- Or "Open a local folder", and set the vcpkg CMake toolchain manually in "Project - CMake Settings for project".

### VSCode (with CMake and C++ extensions)

- Install [vcpkg](https://github.com/Microsoft/vcpkg)
- Add toolchain file to workspace properties `.vscode/settings.json`

```json
{
  "cmake.configureArgs": [
    "-DCMAKE_TOOLCHAIN_FILE=C:/src/vcpkg/scripts/buildsystems/vcpkg.cmake"
  ]
}
```
