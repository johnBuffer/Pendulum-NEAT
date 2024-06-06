#include "resource_manager.hpp"
#include <iostream>

namespace pez::resources
{

ResourceManager::ResID ResourceManager::registerTexture(const std::string& filename, const std::string& asset_name)
{
    const ResID id = textures.size();
    textures[id]   = std::make_unique<sf::Texture>();
    textures[id]->loadFromFile(filename);
    //textures[id]->generateMipmap();
    //textures[id]->setSmooth(true);
    // Update name to id
    name_to_id_textures[asset_name] = id;
    // Return texture id
    return id;
}

ResourceManager::ResID ResourceManager::registerImage(const std::string& filename, const std::string& asset_name)
{
    const ResID id = images.size();
    images[id]     = std::make_unique<sf::Image>();
    images[id]->loadFromFile(filename);
    // Update name to id
    name_to_id_images[asset_name] = id;
    // Return image id
    return id;
}

sf::Texture& ResourceManager::getTexture(ResourceManager::ResID id)
{
    return *textures[id];
}

sf::Image& ResourceManager::getImage(ResourceManager::ResID id)
{
    return *images[id];
}

ResourceManager::ResID ResourceManager::getImageID(const std::string &name)
{
    return name_to_id_images[name];
}

sf::Image& ResourceManager::getImage(const std::string &name)
{
    return *images[name_to_id_images[name]];
}

void ResourceManager::clear()
{
    for (auto& img : images) {
        //img.second.reset();
    }
}

sf::Texture& ResourceManager::getTexture(const std::string &name)
{
    return *textures[name_to_id_textures[name]];
}

ResourceManager::ResID ResourceManager::registerFont(const std::string& filename, const std::string& asset_name)
{
    const uint64_t res_id = fonts.size();
    name_to_id_fonts[asset_name] = res_id;
    fonts[res_id] = std::make_unique<sf::Font>();
    fonts[res_id]->loadFromFile(filename);
    return res_id;
}

sf::Font& ResourceManager::getFont(ResourceManager::ResID id)
{
    return *fonts[id];
}

sf::Font& ResourceManager::getFont(const std::string &name)
{
    return *fonts[name_to_id_fonts[name]];
}

}
