#include <glad/glad.h>
#include <string>
#include <unordered_map>

typedef struct
{
    GLuint id;
    int texWidth, texHeight, nrChannels;
} Texture;

class TextureManager
{
  public:
    ~TextureManager();

    Texture loadTexture(const std::string path);
    void deleteTexture(const GLuint& textureID);

  private:
    std::unordered_map<std::string, Texture> m_textureCache;
};