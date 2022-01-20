# OpenGL playground project

OpenGL playground project with `vcpkg, cmake, glfw, glm, glad, gltf, lodepng, etc`. Fooling around with different rendering techniques.

![This is an image](https://github.com/jukehtinen/opengl-starter/blob/main/screenshots/screenshot01.png)

### VS2022 (with C++ workload)

- Install [vcpkg](https://github.com/Microsoft/vcpkg)
- Add VCPKG_ROOT environment variables pointing to vcpkg.

### Fonts

Font files are generated using [Hiero](https://github.com/libgdx/libgdx/wiki/Distance-field-fonts) tool.

### Ideas
* Texture array tilemap
* Proper material system
* Lights
* Cel shaded lights
* RenderGraph
* Animations
    * Events (spawn particles, play sounds..)
* Skinned meshes
* Improve particles (pools, GPU, trails, different spawners..)    
* Anti-aliasing
* Improve bloom (use GL_R11F_G11F_B10F render target etc.)
* Shader / texure hot-reload
* SPIR-V shaders
* Texture compression / ktx?
* Cubemaps (skybox, env mapping)   
* SSR
* ...
