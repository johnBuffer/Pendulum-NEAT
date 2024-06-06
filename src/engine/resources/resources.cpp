#include "resources.hpp"


namespace pez::resources
{

ResourceManager::ResID registerFont(const std::string& filename, const std::string& asset_name)
{
    return ::pez::core::GlobalInstance::instance->m_resource_manager->registerFont(filename, asset_name);
}

sf::Font& getFont(const std::string& asset_name)
{
    return ::pez::core::GlobalInstance::instance->m_resource_manager->getFont(asset_name);
}

ResourceManager::ResID registerTexture(const std::string& filename, const std::string& asset_name)
{
    return ::pez::core::GlobalInstance::instance->m_resource_manager->registerTexture(filename, asset_name);
}

sf::Texture& getTexture(const std::string& asset_name)
{
    return ::pez::core::GlobalInstance::instance->m_resource_manager->getTexture(asset_name);
}

sf::Image& getImage(const std::string &asset_name) {
    return ::pez::core::GlobalInstance::instance->m_resource_manager->getImage(asset_name);
}

}
