#pragma once


#include "Entity.h"

#include <map>
#include <algorithm>

typedef std::vector<Entity> EntityVector;
typedef std::map<std::string, EntityVector> EntityMap;

class EntityManager
{
	EntityVector	entities;
	EntityMap		entityMap;
	size_t			totalEntities = 0;

	EntityVector	entitiesToAdd;


	void removeDeadEntities(EntityVector& vec)
	{
		// Removes dead entities from EntityVector and EntityMap. Removal from memory is done by entities themselves.
		auto res = std::remove_if(vec.begin(), vec.end(), [](Entity& entity) {return !MemoryPool::Instance().isActive(entity.getID()); });
		vec.erase(res, vec.end());
		totalEntities = entities.size();
	}

	void addGeneratedEntities()
	{
		// First, add new entities.
		for (auto& entity : entitiesToAdd)
		{
			entities.push_back(entity);
			entityMap[entity.getTag()].push_back(entity);
		}
		// Clear the entities-to-add vector.
		entitiesToAdd.clear();
	}

public:

	EntityManager() {}

	void update()
	{
		addGeneratedEntities();
		// Remove from local vector.
		removeDeadEntities(entities);
		// Remove from map.
		for (auto entry : entityMap)
		{
			removeDeadEntities(entry.second);
		}
	}

	Entity addEntity(const std::string& tag)
	{
		// An entity is just an ID and holds no resources. Copying it is OK.
		size_t ID = MemoryPool::Instance().addEntity(tag);
		Entity e{ ID, tag };
		totalEntities++;
		entitiesToAdd.push_back(e);
		return e;
	}

	EntityVector& getEntities()
	{
		return entities;
	}

	EntityVector& getEntitiesWithTag(const std::string& tag)
	{
		return entityMap[tag];
	}
	size_t numberOfEntities() const
	{
		return totalEntities;
	}

	Entity* getEntityWithID(size_t ID)
	{
		auto found = std::find_if(entities.begin(), entities.end(), [ID](Entity& entity) {return entity.getID() == ID; });
		if (found != entities.end())
		{
			return &(*found);
		}
		else
		{
			return nullptr;
		}
	}
};

