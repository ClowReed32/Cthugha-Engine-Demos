#pragma once
//========================================================================
// EntityFactory.h - Defines a factory for creating actors & components
//
//========================================================================


//---------------------------------------------------------------------------------------------------------------------
// EntityFactory class
//
//---------------------------------------------------------------------------------------------------------------------
class EntityFactory
{
    EntityId m_lastEntityId;

protected:
    GenericObjectFactory<EntityComponent, ComponentId> m_componentFactory;

public:
    EntityFactory(void);

	StrongEntityPtr CreateEntity(const char* entityResource, std::string sName, std::vector<ComponentResource> extraComponents, const Mat4x4* initialTransform, const EntityId serversEntityId);
    StrongEntityPtr CreateEntity(const char* actorResource, TiXmlElement* overrides, const Mat4x4* initialTransform, const EntityId serversEntityId);
	StrongEntityPtr CreateStaticObject(TiXmlElement* overrides, const Mat4x4* initialTransform, const EntityId serversEntityId);
	void ModifyEntity(StrongEntityPtr pActor, TiXmlElement* overrides);
	void ModifyEntity(StrongEntityPtr pEntity, std::vector<ComponentResource> extraComponents);

//protected:
    // [rez] This function can be overridden by a subclass so you can create game-specific C++ components.  If you do
    // this, make sure you call the base-class version first.  If it returns NULL, you know it's not an engine component.
    virtual StrongEntityComponentPtr VCreateComponent(TiXmlElement* pData);
	virtual StrongEntityComponentPtr VCreateComponent(std::shared_ptr<ComponentResource> pData);

private:
    EntityId GetNextEntityId(void) { ++m_lastEntityId; return m_lastEntityId; }
};

