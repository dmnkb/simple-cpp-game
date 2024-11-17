#include "Level.h"
#include <Renderer.h>
#include <TextureManager.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

Level::Level()
{
    auto tex0 = TextureManager::loadTexture("assets/texture_02.png");
    tex0.bind(1);
    m_texture0 = tex0.id;
    auto tex1 = TextureManager::loadTexture("assets/texture_01.png");
    tex1.bind(2);
    m_texture1 = tex1.id;

    m_coords = getCoordsByTextureFile("assets/poisson.png");
}

std::vector<glm::vec2> Level::getCoordsByTextureFile(const std::string filename)
{
    assert(std::filesystem::exists(filename) && "Level texture file not found");

    int texWidth, texHeight, channelCount;
    unsigned char* data = stbi_load(filename.c_str(), &texWidth, &texHeight, &channelCount, 0);

    assert((data != nullptr && texWidth > 0 && texHeight > 0) && "Level texture corrupted");

    int i = 0;
    std::vector<glm::vec2> coords;
    for (int y = 0; y < texHeight; y++)
    {
        for (int x = 0; x < texWidth; x++)
        {
            int pixelIndex = (y * texWidth + x) * channelCount;
            auto pixel = static_cast<int>(data[pixelIndex]);
            if (pixel)
                coords.push_back(glm::vec2({x, y}));
            i++;
        }
    }
    stbi_image_free(data);

    return coords;
}

std::vector<glm::vec2> Level::getCubesInsideRadius(const glm::vec3 origin, const int radius)
{
    std::vector<glm::vec2> result;
    glm::vec2 origin2D = glm::vec2(origin.x, origin.z);

    for (const auto& coord : m_coords)
    {
        if (glm::distance(origin2D, coord) <= radius)
        {
            result.push_back(coord);
        }
    }
    return result;
}

void Level::update()
{
    int i = 0;
    for (const auto& coord : m_coords)
    {
        float x = coord.x;
        float z = coord.y;
        Renderer::submitCube(glm::vec3(x, 0, z), glm::vec3(0, 0, 0), glm::vec3(1), m_texture0);
        i++;
    }
    Renderer::submitCube(glm::vec3(49.5, -1, 49.5), glm::vec3(0, 0, 0), glm::vec3({100, 1, 100}), m_texture1, 100);
}