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
    return findEntityRecursive(rootEntity, name);
	

}

std::shared_ptr<engine::Entity> engine::EntityManager::findEntityRecursive(const std::shared_ptr<engine::Entity>& entity, const std::string& name)
{
    // Draw the entity if it has a model
    if (entity && entity->name == name)
    {
        return entity;
    }

    // Recurse on children

    for (const auto& child : entity->children)
    {
        findEntityRecursive(child, name);
    }

    return nullptr;
}
