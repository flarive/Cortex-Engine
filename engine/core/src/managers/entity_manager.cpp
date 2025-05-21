#include "../../include/managers/entity_manager.h"

void engine::EntityManager::create()
{
    m_rootEntity = std::make_shared<engine::Entity>(ROOT_ENTITY_NAME);
}

void engine::EntityManager::set(std::shared_ptr<engine::Entity> rootEntity)
{
	rootEntity = std::move(rootEntity);
}

std::shared_ptr<engine::Entity>& engine::EntityManager::getRootEntity()
{
    return m_rootEntity;
}

std::shared_ptr<engine::Entity> engine::EntityManager::find(const std::string& name)
{
    // Try to get from cache
    auto it = m_entityCache.find(name);
    if (it != m_entityCache.end()) {
        // Found
        return it->second;
    }

    // Fallback: slow search if not in cache
    auto found = findEntityRecursive(m_rootEntity, name);
    if (found) {
        m_entityCache[name] = found; // Cache it
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
    {
        m_rootEntity->addChild(child);

        // add to cache
        addToCache(child);
    }
}

void engine::EntityManager::addChild(std::shared_ptr<engine::Entity> parent, std::shared_ptr<engine::Entity> child)
{
    if (parent && child)
    {
        auto parentEntity = find(parent->name);
        if (parentEntity)
        {
            parentEntity->addChild(child);

            // add to cache
            addToCache(child);
        }
    }
}

bool engine::EntityManager::remove(const std::string& name)
{
    if (!m_rootEntity) return false;

    // Special case: trying to remove the root itself
    if (m_rootEntity->name == name) {
        m_rootEntity.reset();
        m_entityCache.clear();
        return true;
    }

    return removeRecursive(nullptr, m_rootEntity, name);
}

bool engine::EntityManager::removeRecursive(
    const std::shared_ptr<engine::Entity>& parent,
    const std::shared_ptr<engine::Entity>& current,
    const std::string& targetName)
{
    if (!current) return false;

    // Search in current's children
    for (auto it = current->children.begin(); it != current->children.end(); ++it) {
        if (*it && (*it)->name == targetName) {
            // Optionally: recursively erase from cache
            removeFromCacheRecursive(*it);

            // Erase child and its subtree
            current->children.erase(it);
            return true;
        }
        else {
            // Recurse into children
            if (removeRecursive(current, *it, targetName)) {
                return true;
            }
        }
    }

    return false;
}

void engine::EntityManager::addToCache(const std::shared_ptr<engine::Entity>& entity)
{
    if (entity)
        m_entityCache[entity->name] = entity;
}

void engine::EntityManager::removeFromCache(const std::shared_ptr<engine::Entity>& entity)
{
    if (!entity) return;

    m_entityCache.erase(entity->name);
}

void engine::EntityManager::removeFromCacheRecursive(const std::shared_ptr<engine::Entity>& entity)
{
    if (!entity) return;

    m_entityCache.erase(entity->name);
    for (const auto& child : entity->children) {
        removeFromCacheRecursive(child);
    }
}

void engine::EntityManager::updateSelfAndChild()
{
    m_rootEntity->updateSelfAndChild();
}