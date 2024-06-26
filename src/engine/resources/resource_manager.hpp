#pragma once
#include <map>
#include <memory>
#include <string>

#include <SFML/Graphics.hpp>


namespace pez::resources
{
struct ResourceManager
{
    using ResID = uint64_t;

    std::map<ResID, std::unique_ptr<sf::Texture>> textures;
    std::map<ResID, std::unique_ptr<sf::Image>>   images;
    std::map<ResID, std::unique_ptr<sf::Font>>    fonts;

    std::map<std::string, ResID> name_to_id_fonts;
    std::map<std::string, ResID> name_to_id_textures;
    std::map<std::string, ResID> name_to_id_images;

    ResourceManager() = default;

    void clear();

    ResID registerFont(const std::string& filename, const std::string& asset_name);
    ResID registerTexture(const std::string& filename, const std::string& asset_name);
    ResID registerImage(const std::string& filename, const std::string& asset_name);

    sf::Texture& getTexture(ResID id);
    sf::Image&   getImage(ResID id);
    sf::Font&    getFont(ResID id);
    sf::Font&    getFont(const std::string& name);
    sf::Texture& getTexture(const std::string& name);
    sf::Image&   getImage(const std::string& name);

    ResID getImageID(const std::string& name);
};
}
