#include "../../include/managers/entity_manager.h"

void engine::EntityManager::create()
{
	rootEntity = std::make_shared<engine::Entity>("Root");
}

void engine::EntityManager::set(std::shared_ptr<engine::Entity> rootEntity)
{
	rootEntity = std::move(rootEntity);
}

std::shared_ptr<engine::Entity> engine::EntityManager::find(const std::string& name)
{
    auto zzz = findEntityRecursive(rootEntity, name);
	
    return zzz;

}

std::shared_ptr<engine::Entity> engine::EntityManager::findEntityRecursive(const std::shared_ptr<engine::Entity>& entity, const std::string& name)
{
    if (entity && entity->name == name)
    {
        return entity;
    }

    for (const auto& child : entity->children)
    {
        auto found = findEntityRecursive(child, name);
        if (found)
        {
            return found; // Return as soon as you find a match
        }
    }

    return nullptr; // Not found
}
