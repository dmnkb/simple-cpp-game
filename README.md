# Tiny game written in C++ using OpenGL
I'm building this game to learn game development in C++ sticking to best practices as I come to understand them. I'm not quite sure what the game might look like yet, as I'm constantly implementing new features based on current interests. I outline the scope of this project below:

### Roadmap
- [x] Project setup (CMake, GLFW, GLAD)
- [x] First person controller
- [x] Dear ImGUI
- [x] Lighting
    - [x] Blinn-Phong lighting model
    - [x] Dynamic amount of lights (using UBO)
    - [x] Spot, point, directional
- [x] Refactor the renderer to become more generic (Introduce Mesh class)
- [x] Scene graph & scene nodes
- [x] Instanced rendering
- [x] Render pass abstraction
- [x] Shadow mapping
- [x] Materials
- [x] Update FrameBuffer to support multiple attachments
- [x] Integrate Assimp mesh and material loading
- [x] Basic post processing (Screen quad)
- [x] Profiler
- [x] Window & Framebuffer rezising
- [x] Refactor lights
- [ ] Cascaded shadow maps 
- [ ] Change debug to editor
- [ ] ImGUI Gimbal controls
- [ ] ECS
- [x] Display frametime (CPU)
- [ ] Update stats panel to display frametime per file, instead of region
- [ ] Split architecture into engine and game
- [ ] Layer architecture
- [ ] SSAO
- [ ] Debug view (Wireframe, per instance coloring)
- [ ] Mip chain bloom
- [ ] Integrate Bullet Physics (Drag and drop objects)
- [ ] Audio using [miniaudio](https://github.com/mackron/miniaudio)
- [ ] UI using [clay](https://github.com/nicbarker/clay)
- [ ] Improve event system referencing ([this](https://github.com/TheCherno/Hazel/blob/1feb70572fa87fa1c4ba784a2cfeada5b4a500db/Hazel/src/Hazel/Core/Base.h#L26))
- [ ] Simple particle system OR grass rendering
- [ ] Rendering a heightmap from a texture
- [ ] Skeletal animation and GPU skinning





