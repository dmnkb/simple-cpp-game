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
- [x] ECS
    - [x] Add entities
    - [x] Remove entities
    - [x] Scene hierarchy panel
        - [x] Display associated components
        - [x] Add / remove components
        - [x] Modify components
        - [x] Create and switch scenes
    - [x] (De)serialize scene
- [x] Save Button - Saves registry and serializes scenes and materials
- [ ] Assets (Material editing)
    - [x] Assets panel
        - [x] List all assets
        - [x] Create new material
        - [x] When assigning a mesh to mesh component, automatically use default material (Bug)
    - [x] (De)serialize assets registry (project-wide asset database)
    - [x] (De)serialize materials
    - [x] Assign material to material slot
    - [ ] Edit material
- [ ] ImGUI Gizmo controls
- [ ] Mouse picking
- [x] Display frametime (CPU)
- [ ] Mip chain bloom
- [ ] SSAO
- [ ] Debug view (Wireframe, per instance coloring)
- [ ] Integrate Bullet Physics (Pull and throw objects)
- [ ] Audio using [miniaudio](https://github.com/mackron/miniaudio)
- [ ] UI using [RmlUi](https://github.com/mikke89/RmlUi)? (Or stick with ImGUI?)
- [ ] ? Improve event system referencing ([this](https://github.com/TheCherno/Hazel/blob/1feb70572fa87fa1c4ba784a2cfeada5b4a500db/Hazel/src/Hazel/Core/Base.h#L26))
- [ ] Simple particle system
- [ ] Rendering a heightmap from a texture (Vertex painting?)
- [ ] Skeletal animation and GPU skinning





