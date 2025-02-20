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
- [ ] Integrate Assimp mesh and material loading
- [ ] ECS
    - [ ] Transform component
    - [ ] Mesh component
    - [ ] Material component
- [ ] Make the sun controllable using the editor
- [ ] Cascaded shadow maps 
- [ ] Update render stats debugging (Frame time per pass)
- [ ] Support viewport resizing
- [ ] Basic post processing (Screen quad)
- [ ] Mip chain bloom
- [ ] SSAO
- [ ] Anti-Aliasing? (Might contrast the chased artstyle)
- [ ] HDR image based lighting
- [ ] Basic deferred rendering
- [ ] Integrate Bullet Physics (Car demo)
- [ ] UI using [clay](https://github.com/nicbarker/clay)
- [ ] Improve event system referencing ([this](https://github.com/TheCherno/Hazel/blob/1feb70572fa87fa1c4ba784a2cfeada5b4a500db/Hazel/src/Hazel/Core/Base.h#L26))
- [ ] Editor mode
    - [ ] Scene (de)serialization
    - [ ] Frame buffer based picking
    - [ ] Control position and rotation using [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
- [ ] Simple particle system OR grass rendering
- [ ] Rendering a heightmap from a texture
- [ ] Audio using [miniaudio](https://github.com/mackron/miniaudio)
- [ ] Skeletal animation and GPU skinning





