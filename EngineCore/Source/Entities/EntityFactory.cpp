//========================================================================
// EntityFactory.cpp : Creates actors from components
//
//========================================================================

#include "CthughaStd.h"
#include "EntityFactory.h"
#include "Entity.h"
#include "../ResourceCache/XmlResource.h"
#include "EntityComponent.h"
//#include "AudioComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "PhysicsComponent.h"
#include "GUIComponent.h"
#include "RectTransformComponent.h"
//#include "BaseScriptComponent.h"
#include "../Utilities/String.h"


//---------------------------------------------------------------------------------------------------------------------
// Factory class definition
//---------------------------------------------------------------------------------------------------------------------
EntityFactory::EntityFactory(void)
{
    m_lastEntityId = INVALID_ENTITY_ID;

    m_componentFactory.Register<TransformComponent>(EntityComponent::GetIdFromName(TransformComponent::g_Name));
    m_componentFactory.Register<MeshRenderComponent>(EntityComponent::GetIdFromName(MeshRenderComponent::g_Name));
    m_componentFactory.Register<ParticleSystemComponent>(EntityComponent::GetIdFromName(ParticleSystemComponent::g_Name));
	m_componentFactory.Register<LightRenderComponent>(EntityComponent::GetIdFromName(LightRenderComponent::g_Name)); 
	m_componentFactory.Register<DecalRenderComponent>(EntityComponent::GetIdFromName(DecalRenderComponent::g_Name));
    m_componentFactory.Register<PhysicsComponent>(EntityComponent::GetIdFromName(PhysicsComponent::g_Name));
    m_componentFactory.Register<ImageUIComponent>(EntityComponent::GetIdFromName(ImageUIComponent::g_Name));
	m_componentFactory.Register<TextUIComponent>(EntityComponent::GetIdFromName(TextUIComponent::g_Name));
    m_componentFactory.Register<ButtonUIComponent>(EntityComponent::GetIdFromName(ButtonUIComponent::g_Name));
	m_componentFactory.Register<CheckBoxUIComponent>(EntityComponent::GetIdFromName(CheckBoxUIComponent::g_Name));
    m_componentFactory.Register<RectTransformComponent>(EntityComponent::GetIdFromName(RectTransformComponent::g_Name));
}

StrongEntityPtr EntityFactory::CreateStaticObject(TiXmlElement* overrides, const Mat4x4* initialTransform, const EntityId serversEntityId)
{
	// create the actor instance
	EntityId nextEntityId = serversEntityId;
	if (nextEntityId == INVALID_ENTITY_ID)
	{
		nextEntityId = GetNextEntityId();
	}

	StrongEntityPtr pEntity(CHG_NEW Entity(nextEntityId));
    if (!pEntity->Init(overrides))
    {
        CHG_ERROR("Failed to initialize actor.");
        return StrongEntityPtr();
    }

	//Set Entity Name
	std::string name = overrides->Attribute("Name");
	pEntity->m_name = name;

	//Get child Components
	TiXmlElement* pComponents = overrides->FirstChildElement("Components");

	bool initialTransformSet = false;

    // Loop through each child element and load the component
    for (TiXmlElement* pNode = pComponents->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
    {
        StrongEntityComponentPtr pComponent(VCreateComponent(pNode));
        if (pComponent)
        {
            pEntity->AddComponent(pComponent);
            pComponent->SetOwner(pEntity);
        }
        else
        {
            // If an error occurs, we kill the actor and bail.  We could keep going, but the actor is will only be 
            // partially complete so it's not worth it.  Note that the pActor instance will be destroyed because it
            // will fall out of scope with nothing else pointing to it.
            return StrongEntityPtr();
        }
    }

	// This is a bit of a hack to get the initial transform of the transform component set before the 
	// other components (like PhysicsComponent) read it.
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pEntity->GetComponent<TransformComponent>(TransformComponent::g_Name));

    // Now that the actor has been fully created, run the post init phase
    pEntity->PostInit();

    return pEntity;
}

StrongEntityPtr EntityFactory::CreateEntity(const char* entityResource, std::string sName, std::vector<ComponentResource> extraComponents, const Mat4x4* initialTransform, const EntityId serversEntityId)
{
	Resource resource(entityResource);
	std::shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);
	std::shared_ptr<EntityResourceExtraData> pEntityResourceExtraData = std::static_pointer_cast<EntityResourceExtraData>(pResourceHandle->GetExtra());

	if (!pEntityResourceExtraData)
	{
		CHG_ERROR("Failed to create actor from resource: " + std::string(entityResource));
		return StrongEntityPtr();
	}

	// create the actor instance
	EntityId nextEntityId = serversEntityId;
	if (nextEntityId == INVALID_ENTITY_ID)
	{
		nextEntityId = GetNextEntityId();
	}

	StrongEntityPtr pEntity(CHG_NEW Entity(nextEntityId));
	if (!pEntity->Init(pEntityResourceExtraData))
	{
		CHG_ERROR("Failed to initialize actor: " + std::string(entityResource));
		return StrongEntityPtr();
	}

	//Set Entity Name
	pEntity->m_name = sName;

	bool initialTransformSet = false;

	// Loop through each child element and load the component
	for (std::map<std::string, std::shared_ptr<ComponentResource>>::iterator it = pEntityResourceExtraData->GetFirstComponent(); it != pEntityResourceExtraData->GetLastComponent(); ++it)
	{
		StrongEntityComponentPtr pComponent(VCreateComponent(it->second));
		if (pComponent)
		{
			pEntity->AddComponent(pComponent);
			pComponent->SetOwner(pEntity);
		}
	}

	if (extraComponents.size() > 0)
	{
		ModifyEntity(pEntity, extraComponents);
	}	

	// This is a bit of a hack to get the initial transform of the transform component set before the 
	// other components (like PhysicsComponent) read it.
	shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pEntity->GetComponent<TransformComponent>(TransformComponent::g_Name));

	// Now that the actor has been fully created, run the post init phase
	pEntity->PostInit();

	return pEntity;
}

StrongEntityPtr EntityFactory::CreateEntity(const char* entityResource, TiXmlElement *overrides, const Mat4x4 *pInitialTransform, const EntityId serversEntityId)
{
	Resource resource(entityResource);
	std::shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);
	std::shared_ptr<EntityResourceExtraData> pEntityResourceExtraData = std::static_pointer_cast<EntityResourceExtraData>(pResourceHandle->GetExtra());

	if (!pEntityResourceExtraData)
    {
        CHG_ERROR("Failed to create actor from resource: " + std::string(entityResource));
        return StrongEntityPtr();
    }

    // create the actor instance
	EntityId nextEntityId = serversEntityId;
	if (nextEntityId == INVALID_ENTITY_ID)
	{
		nextEntityId = GetNextEntityId();
	}

    StrongEntityPtr pEntity(CHG_NEW Entity(nextEntityId));
	if (!pEntity->Init(pEntityResourceExtraData))
    {
        CHG_ERROR("Failed to initialize actor: " + std::string(entityResource));
        return StrongEntityPtr();
    }

	//Set Entity Name
	if(overrides)
	{
		std::string name = overrides->Attribute("Name");
		pEntity->m_name = name;
	}
	else
	{
		pEntity->m_name = entityResource;
	}

	bool initialTransformSet = false;

    // Loop through each child element and load the component
	for (std::map<std::string, std::shared_ptr<ComponentResource>>::iterator it = pEntityResourceExtraData->GetFirstComponent(); it != pEntityResourceExtraData->GetLastComponent(); ++it)
	{
		StrongEntityComponentPtr pComponent(VCreateComponent(it->second));
		if (pComponent)
		{
			pEntity->AddComponent(pComponent);
			pComponent->SetOwner(pEntity);
		}
		else
		{
			// If an error occurs, we kill the actor and bail.  We could keep going, but the actor is will only be 
			// partially complete so it's not worth it.  Note that the pActor instance will be destroyed because it
			// will fall out of scope with nothing else pointing to it.
			return StrongEntityPtr();
		}
	}

	if (overrides)
	{
		ModifyEntity(pEntity, overrides);
	}

	// This is a bit of a hack to get the initial transform of the transform component set before the 
	// other components (like PhysicsComponent) read it.
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pEntity->GetComponent<TransformComponent>(TransformComponent::g_Name));

    // Now that the actor has been fully created, run the post init phase
    pEntity->PostInit();

    return pEntity;
}

StrongEntityComponentPtr EntityFactory::VCreateComponent(TiXmlElement* pData)
{
    const char* name = pData->Value();
    StrongEntityComponentPtr pComponent(m_componentFactory.Create(EntityComponent::GetIdFromName(name)));

    // initialize the component if we found one
    if (pComponent)
    {
        if (!pComponent->VInit(pData))
        {
            CHG_ERROR("Component failed to initialize: " + std::string(name));
            return StrongEntityComponentPtr();
        }
    }
    else
    {
        CHG_ERROR("Couldn't find ActorComponent named " + std::string(name));
        return StrongEntityComponentPtr();  // fail
    }

    // pComponent will be NULL if the component wasn't found.  This isn't necessarily an error since you might have a 
    // custom CreateComponent() function in a sub class.
    return pComponent;
}

StrongEntityComponentPtr EntityFactory::VCreateComponent(std::shared_ptr<ComponentResource> pData)
{
	std::string name = pData->GetName();
	StrongEntityComponentPtr pComponent(m_componentFactory.Create(EntityComponent::GetIdFromName(name.c_str())));

	// initialize the component if we found one
	if (pComponent)
	{
		if (!pComponent->VInit(pData))
		{
			CHG_ERROR("Component failed to initialize: " + name);
			return StrongEntityComponentPtr();
		}
	}
	else
	{
		CHG_WARNING("Couldn't find ActorComponent named " + name);
		return StrongEntityComponentPtr();  // fail
	}

	// pComponent will be NULL if the component wasn't found.  This isn't necessarily an error since you might have a 
	// custom CreateComponent() function in a sub class.
	return pComponent;
}

void EntityFactory::ModifyEntity(StrongEntityPtr pEntity, std::vector<ComponentResource> extraComponents)
{
	// Loop through each vector element and load the component
	for (UINT i = 0; i < extraComponents.size(); i++)
	{
		StrongEntityComponentPtr pComponent = MakeStrongPtr(pEntity->GetComponent<EntityComponent>(extraComponents[i].GetName().c_str()));
		
		if (pComponent)
		{
			pComponent->VInit(std::shared_ptr<ComponentResource>(new ComponentResource(extraComponents[i])));
			pComponent->VOnChanged();
		}
		else
		{
			pComponent = VCreateComponent(std::shared_ptr<ComponentResource>(new ComponentResource(extraComponents[i])));
			if (pComponent)
			{
				pEntity->AddComponent(pComponent);
				pComponent->SetOwner(pEntity);
			}
		}
	}
}

void EntityFactory::ModifyEntity(StrongEntityPtr pActor, TiXmlElement* overrides)
{
	//Get child Components
	TiXmlElement* pComponents = overrides->FirstChildElement("Components");

	// Loop through each child element and load the component
	for (TiXmlElement* pNode = pComponents->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
	{
		ComponentId componentId = EntityComponent::GetIdFromName(pNode->Value());
		StrongEntityComponentPtr pComponent = MakeStrongPtr(pActor->GetComponent<EntityComponent>(componentId));
		if (pComponent)
		{
			pComponent->VInit(pNode);

			// [mrmike] - added post press to ensure that components that need it have
			//            Events generated that can notify subsystems when changes happen.
			//            This was done to have SceneNode derived classes respond to RenderComponent
			//            changes.

			pComponent->VOnChanged();		
		}
		else
		{
			pComponent = VCreateComponent(pNode);
			if (pComponent)
			{
				pActor->AddComponent(pComponent);
				pComponent->SetOwner(pActor);
			}
		}
	}		
}