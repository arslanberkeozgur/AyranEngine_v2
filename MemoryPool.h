#pragma once

#include <tuple>
#include <vector>
#include <iostream>

#include "Component.h"

// WHENEVER YOU ADD A NEW COMPONENT:
// 1. ADD IT TO THE ComponentVectorTuple
// 2. REMEMBER TO RESIZE IT IN resizeVector()
// 3. ADD THE CORRESPONDING LINE IN setComponentsToDefault()

typedef std::tuple<
	std::vector<cTransform>,
	std::vector<cInput>,
	std::vector<cCamera>,
	std::vector<cShader>,
	std::vector<cPointLight>,
	std::vector<cSpotLight>,
	std::vector<cModel>
> ComponentVectorTuple;

class MemoryPool
{
	size_t						maxEntities;
	size_t						increaseStep;

	size_t						numberOfEntities = 0;
	ComponentVectorTuple		componentVectors;
	std::vector<bool>			entityActivity;
	std::vector<std::string>	tags;

	// Private constructor for singleton pattern.
	MemoryPool(size_t maxEntities, size_t increaseStep)
		:maxEntities{ maxEntities }, increaseStep{ increaseStep }
	{
		resizeVectors(maxEntities);
	}

	void resizeVectors(size_t resizeValue)
	{
		// Resize component vectors.
		std::get<std::vector<cTransform>>(componentVectors).resize(resizeValue);
		std::get<std::vector<cInput>>(componentVectors).resize(resizeValue);
		std::get<std::vector<cCamera>>(componentVectors).resize(resizeValue);
		std::get<std::vector<cShader>>(componentVectors).resize(resizeValue);
		std::get<std::vector<cPointLight>>(componentVectors).resize(resizeValue);
		std::get<std::vector<cSpotLight>>(componentVectors).resize(resizeValue);
		std::get<std::vector<cModel>>(componentVectors).resize(resizeValue);

		// Resize boolean vector.
		entityActivity.resize(resizeValue);

		// Resize tags
		tags.resize(resizeValue);
	}

	size_t getNextEntityIndex()
	{
		// First try to find an inactive entity.
		for (size_t i = 0; i < maxEntities; ++i)
		{
			if (entityActivity[i] == false)
			{
				return i;
			}
		}

		std::cout << "Maximum entity size reached, resizing...\n";

		// All entities are currently active: resize memory pool.
		resizeVectors(maxEntities + increaseStep);
		maxEntities = maxEntities + increaseStep;

		return maxEntities - increaseStep;
	}

	void setComponentsToDefault(size_t entityID)
	{
		std::get<std::vector<cTransform>>(componentVectors)[entityID] = cTransform();
		std::get<std::vector<cInput>>(componentVectors)[entityID] = cInput();
		std::get<std::vector<cCamera>>(componentVectors)[entityID] = cCamera();
		std::get<std::vector<cShader>>(componentVectors)[entityID] = cShader();
		std::get<std::vector<cPointLight>>(componentVectors)[entityID] = cPointLight();
		std::get<std::vector<cSpotLight>>(componentVectors)[entityID] = cSpotLight();
		std::get<std::vector<cModel>>(componentVectors)[entityID] = cModel();
	}
public:

	// These can be set before initialization.
	static const unsigned int MAX_ENTITIES;
	static const unsigned int MEMORY_RESIZE_AMOUNT;

	// MemoryPool is a singleton.
	static MemoryPool& Instance()
	{
		static MemoryPool pool(MAX_ENTITIES, MEMORY_RESIZE_AMOUNT);
		return pool;
	}

	template <typename T>
	T& getComponent(size_t entityID)
	{
		return std::get<std::vector<T>>(componentVectors)[entityID];
	}

	template <typename T>
	bool hasComponent(size_t entityID)
	{
		return std::get<std::vector<T>>(componentVectors)[entityID].active;
	}

	const std::string& getTag(size_t entityID) const
	{
		return tags[entityID];
	}

	bool isActive(size_t entityID) const
	{
		return entityActivity[entityID];
	}

	size_t addEntity(const std::string& tag)
	{
		size_t index = getNextEntityIndex();

		// By default an entity contains all default components in it. 
		// Fine for now, as long as we keep the components small.
		setComponentsToDefault(index);

		entityActivity[index] = true;
		tags[index] = tag;
		numberOfEntities++;
		std::cout << "Entity added:" << index << " | Number of entitites: " << numberOfEntities << std::endl;
		return index;
	}

	void removeEntity(size_t entityID)
	{
		entityActivity[entityID] = false;
		numberOfEntities--;
		std::cout << "Entity removed: " << entityID << " | Number of entitites: " << numberOfEntities << std::endl;
	}

	size_t numOfEntities() const
	{
		return numberOfEntities;
	}
};