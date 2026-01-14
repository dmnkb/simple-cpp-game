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
- [x] Cascaded shadow maps 
- [x] Change debug to editor
- [ ] ECS
    - [x] Add entities
    - [ ] Remove entities
    - [ ] Scene hierarchy panel
        - [x] Display associated components
        - [x] Add / remove components
        - [x] Modify components
    - [ ] (De)serialize scene
- [ ] Assets (Material editing)
    - [ ] Assets panel
        - [x] List all assets
        - [x] Create new material
        - [x] When assigning a mesh to mesh component, automatically use default material (Bug)
    - [ ] (De)serialize assets registry (project-wide asset database)
    - [x] (De)serialize materials
    - [x] Assign material to material slot
    - [ ] Edit material
- [ ] ImGUI Gizmo controls
- [x] Display frametime (CPU)
- [ ] Mip chain bloom
- [ ] Split architecture into engine and game
- [ ] Layer architecture (Or scene stack?)
- [ ] SSAO
- [ ] Debug view (Wireframe, per instance coloring)
- [ ] Integrate Bullet Physics (Pull and throw objects)
- [ ] Audio using [miniaudio](https://github.com/mackron/miniaudio)
- [ ] UI using [RmlUi](https://github.com/mikke89/RmlUi)
- [ ] Improve event system referencing ([this](https://github.com/TheCherno/Hazel/blob/1feb70572fa87fa1c4ba784a2cfeada5b4a500db/Hazel/src/Hazel/Core/Base.h#L26))
- [ ] Simple particle system OR grass rendering
- [ ] Rendering a heightmap from a texture
- [ ] Skeletal animation and GPU skinning





