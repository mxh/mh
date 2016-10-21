#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <map>
#include <memory>
#include <string>

#include "mh/gpu/texture.h"

namespace mh
{

class TextureManager
{
public:
    static TextureManager & get_instance()
    {
        static TextureManager instance;
        return instance;
    }

    inline std::shared_ptr<Texture> load_texture(const std::string & filename);

protected:

private:
    TextureManager() {}

    TextureManager(const TextureManager &);
    void operator=(const TextureManager &);

    std::map<std::string, std::shared_ptr<Texture> > m_cached_textures;

}; // class TextureManager;

std::shared_ptr<Texture> TextureManager::load_texture(const std::string & filename)
{
    auto cache_it = m_cached_textures.find(filename);
    if (cache_it == m_cached_textures.end())
    {
        m_cached_textures[filename] = std::make_shared<Texture>(filename);
    }

    return m_cached_textures[filename];
}

} // namespace mh

#endif
