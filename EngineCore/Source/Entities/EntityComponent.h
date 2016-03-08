#pragma once
//========================================================================
// ActorComponent.h - Defines the base class for a component that can be attached to an actor
//
//========================================================================

#include "../Utilities/String.h"
#include "EntityResource.h"

class ComponentResource;
class TiXmlElement;

//---------------------------------------------------------------------------------------------------------------------
// EntityComponent class
//---------------------------------------------------------------------------------------------------------------------
class EntityComponent
{
	friend class EntityFactory;

protected:
	StrongEntityPtr m_pOwner;

public:
	virtual ~EntityComponent(void) { m_pOwner.reset(); }

    // These functions are meant to be overridden by the implementation classes of the components.
	virtual bool VInit(TiXmlElement* pData) = 0;
	virtual bool VInit(std::shared_ptr<ComponentResource> componentResource) = 0;
	virtual void VPostInit(void) { }
	virtual void VUpdate(int deltaMs) { }
	virtual void VOnChanged(void) { }				// [mrmike] - added post-press

    // for the editor
    virtual TiXmlElement* VGenerateXml(void) = 0;

    // This function should be overridden by the interface class.
	virtual ComponentId VGetId(void) const { return GetIdFromName(VGetName()); }
	virtual const char *VGetName() const = 0;
    static ComponentId GetIdFromName(const char* componentStr)
	{
		void* rawId = HashedString::hash_name(componentStr);
		return reinterpret_cast<ComponentId>(rawId);
	}

private:
	void SetOwner(StrongEntityPtr pOwner) { m_pOwner = pOwner; }

};


