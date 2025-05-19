#include "../../include/managers/entity_manager.h"

void engine::EntityManager::create()
{
	rootEntity = std::make_shared<engine::Entity>("Root");
}

void engine::EntityManager::set(std::shared_ptr<engine::Entity> rootEntity)
{
	rootEntity = std::move(rootEntity);
}

//std::shared_ptr<engine::Entity>& engine::EntityManager::get()
//{
//	return rootEntity;
//}

engine::Entity& engine::EntityManager::find(const std::string& name)
{
	auto ppp = engine::Entity{"aaa"};
	return ppp;
}
