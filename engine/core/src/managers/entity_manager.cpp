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
    auto it = nameCache.find(name);
    if (it != nameCache.end()) {
        return it->second;
    }

    // Fallback: slow search if not in cache
    auto found = findEntityRecursive(rootEntity, name);
    if (found) {
        nameCache[name] = found; // Cache it
    }
    return found;
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

void engine::EntityManager::addChild(std::shared_ptr<engine::Entity> child)
{
    if (child)
        rootEntity->addChild(child);
}

void engine::EntityManager::addChild(std::shared_ptr<engine::Entity> parent, std::shared_ptr<engine::Entity> child)
{
    
}

//void engine::EntityManager::buildCache()
//{
//    nameCache.clear();
//    std::function<void(std::shared_ptr<Entity>)> recurse = [&](std::shared_ptr<Entity> entity) {
//        if (entity) {
//            nameCache[entity->name] = entity;
//            for (const auto& child : entity->children) {
//                recurse(child);
//            }
//        }
//        };
//    recurse(rootEntity);
//}
