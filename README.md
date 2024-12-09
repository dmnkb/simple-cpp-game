# Tiny game written in C++ using OpenGL
I build this game to learn the basics of game development in C++. I aim to adhere to common best practices as I come to understand them. For a first iteration of what could be an actually playable game, I imagine a simple car simulation (using Bullet Physics' raycast vehicle) on a heightmap generated terrain.

### Roadmap
- [x] Project setup (CMake, GLFW, GLAD)
- [x] Batch rendered cubes (To learn about batch rendering)
- [x] First person controller
- [x] Dear ImGUI
- [x] Player has gravity and AABB collision detection
- Lighting
    - [x] Blinn-Phong lighting model
    - [x] Dynamics amount of lights (using UBO)
    - [ ] Spot lights
- [ ] Render pass abstraction
- [ ] Frame buffer object abstraction
- [ ] Shadow mapping
- [ ] Basic deferred rendering
- [ ] Scene graph & scene nodes
- [ ] Simple particle system (To learn about instanced rendering)
- [ ] Refactor the renderer to become more generic
- [ ] Integrate Assimp mesh loading
- [ ] Integrate Bullet Physics
- [ ] Rendering a heightmap from a texture
- [ ] Skeletal animation and GPU skinning
- [ ] Sounds using miniaudio




