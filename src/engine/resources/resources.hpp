#pragma once
#include "engine/core/instance.hpp"
#include "engine/resources/resource_manager.hpp"


namespace pez::resources
{

ResourceManager::ResID registerFont(const std::string& filename, const std::string& asset_name);
ResourceManager::ResID registerTexture(const std::string& filename, const std::string& asset_name);
sf::Font&              getFont(const std::string& asset_name);
sf::Texture&           getTexture(const std::string& asset_name);
sf::Image&             getImage(const std::string& asset_name);

}
