#pragma once

#include "MemoryPool.h"
#include "Engine.h"

class Entity
{
	size_t      ID			 = 0;
	bool	    active;
	std::string tag;

	// Private constructor: An entity can only be created through the EntityManager.
	friend class EntityManager;
	Entity(size_t id, const std::string& ttag = "") : ID{ id }, active{ true }, tag{ ttag } {}

public:

	template <typename T>
	T& getComponent()
	{
		return MemoryPool::Instance().getComponent<T>(ID);
	}

	template <typename T>
	const T& getComponent() const
	{
		return MemoryPool::Instance().getComponent<T>(ID);
	}

	template <typename T>
	bool hasComponent() const
	{
		return MemoryPool::Instance().hasComponent<T>(ID);
	}

	template <typename T, typename... TArgs>
	T& addComponent(TArgs&&... args)
	{
		T& component = getComponent<T>();
		component = T(std::forward<TArgs>(args)...);
		component.active = true;
		component.ownerID = ID;

		Engine::Instance().OnAddComponent(component);

		return component;
	}

	template <typename T>
	void removeComponent()
	{
		Engine::Instance().OnRemoveComponent(MemoryPool::Instance().getComponent<T>(ID));

		MemoryPool::Instance().getComponent<T>(ID).active = false;
	}

	void destroy()
	{
		Engine::Instance().OnEntityDestroy(*this);

		active = false;
		MemoryPool::Instance().removeEntity(ID);
	}

	bool isActive() const
	{
		return active;
	}

	const std::string& getTag() const
	{
		return tag;
	}

	const size_t getID() const
	{
		return ID;
	}
};