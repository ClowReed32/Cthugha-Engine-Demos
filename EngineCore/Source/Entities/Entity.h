#pragma once
//========================================================================
// Entity.h - Defines the Entity class
//
//========================================================================

#include "EntityResource.h"

class EntityResourceExtraData;
class TiXmlElement;
typedef std::string EntityType;

//---------------------------------------------------------------------------------------------------------------------
// Entity class
//---------------------------------------------------------------------------------------------------------------------
class Entity
{
    friend class EntityFactory;

public:

    typedef std::map<ComponentId, StrongEntityComponentPtr> EntityComponents;

private:
    EntityId m_id;					// unique id for the Entity
	std::string m_name;				// entity name
    EntityComponents m_components;	// all components this Entity has
    EntityType m_type;

	// [mrmike] - these were added post press as editor helpers, but will also be great for save game files if we ever make them.
	std::string m_resource;			// the XML file from which this actor was initialized (considered the "Archetype" file)

public:
    explicit Entity(EntityId id);
    ~Entity(void);

    bool Init(TiXmlElement* pData);
	bool Init(std::shared_ptr<EntityResourceExtraData> pData);
    void PostInit(void);
    void Destroy(void);
    void Update(int deltaMs);

    // editor functions
	std::string Entity::ToXML();

    // accessors
    EntityId GetId(void) const { return m_id; }
    EntityType GetType(void) const { return m_type; }
	std::string GetName(void) const { return m_name; }

    // template function for retrieving components
    template <class ComponentType>
    weak_ptr<ComponentType> GetComponent(ComponentId id)
    {
        EntityComponents::iterator findIt = m_components.find(id);
        if (findIt != m_components.end())
        {
            StrongEntityComponentPtr pBase(findIt->second);
            shared_ptr<ComponentType> pSub(static_pointer_cast<ComponentType>(pBase));  // cast to subclass version of the pointer
            weak_ptr<ComponentType> pWeakSub(pSub);  // convert strong pointer to weak pointer
            return pWeakSub;  // return the weak pointer
        }
        else
        {
            return weak_ptr<ComponentType>();
        }
    }

    template <class ComponentType>
    weak_ptr<ComponentType> GetComponent(const char *name)
    {
		ComponentId id = EntityComponent::GetIdFromName(name);
        EntityComponents::iterator findIt = m_components.find(id);
        if (findIt != m_components.end())
        {
            StrongEntityComponentPtr pBase(findIt->second);
            shared_ptr<ComponentType> pSub(static_pointer_cast<ComponentType>(pBase));  // cast to subclass version of the pointer
            weak_ptr<ComponentType> pWeakSub(pSub);  // convert strong pointer to weak pointer
            return pWeakSub;  // return the weak pointer
        }
        else
        {
            return weak_ptr<ComponentType>();
        }
    }

	const EntityComponents* GetComponents() { return &m_components; }

    void AddComponent(StrongEntityComponentPtr pComponent);
};


