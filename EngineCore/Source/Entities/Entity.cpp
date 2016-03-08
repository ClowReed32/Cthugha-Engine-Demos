//========================================================================
// Entity.cpp - Implements the Entity class
//
//========================================================================


#include "CthughaStd.h"
#include "Entity.h"
#include "EntityComponent.h"
#include "../Utilities/String.h"


//---------------------------------------------------------------------------------------------------------------------
// Entity
//---------------------------------------------------------------------------------------------------------------------
Entity::Entity(EntityId id)
{
    m_id = id;
    m_type = "Unknown";

	// [mrmike] added post press - this is an editor helper
	m_resource = "Unknown";
}

Entity::~Entity(void)
{
    CHG_LOG("Entity", std::string("Destroying Entity ") + ToStr(m_id));
    CHG_ASSERT(m_components.empty());  // [rez] if this assert fires, the Entity was destroyed without calling Entity::Destroy()
}

bool Entity::Init(TiXmlElement* pData)
{
    CHG_LOG("Entity", std::string("Initializing Entity ") + ToStr(m_id));

	m_type = pData->Attribute("type");
	m_resource = pData->Attribute("resource");
    return true;
}

bool Entity::Init(std::shared_ptr<EntityResourceExtraData> pData)
{
	CHG_LOG("Entity", std::string("Initializing Entity ") + ToStr(m_id));

	m_type = pData->GetEntityType();
	m_resource = pData->GetResourceName();
	return true;
}

void Entity::PostInit(void)
{
    for (EntityComponents::iterator it = m_components.begin(); it != m_components.end(); ++it)
    {
        it->second->VPostInit();
    }
}

void Entity::Destroy(void)
{
    m_components.clear();
}

void Entity::Update(int deltaMs)
{
    for (EntityComponents::iterator it = m_components.begin(); it != m_components.end(); ++it)
    {
        it->second->VUpdate(deltaMs);
    }
}

std::string Entity::ToXML()
{
    TiXmlDocument outDoc;

    // Entity element
    TiXmlElement* pEntityElement = CHG_NEW TiXmlElement("Entity");
    pEntityElement->SetAttribute("type", m_type.c_str());
	pEntityElement->SetAttribute("resource", m_resource.c_str());

    // components
    for (auto it = m_components.begin(); it != m_components.end(); ++it)
    {
        StrongEntityComponentPtr pComponent = it->second;
        TiXmlElement* pComponentElement = pComponent->VGenerateXml();
        pEntityElement->LinkEndChild(pComponentElement);
    }

    outDoc.LinkEndChild(pEntityElement);
	TiXmlPrinter printer;
	outDoc.Accept(&printer);

	return printer.CStr();
}


void Entity::AddComponent(StrongEntityComponentPtr pComponent)
{
    std::pair<EntityComponents::iterator, bool> success = m_components.insert(std::make_pair(pComponent->VGetId(), pComponent));
    CHG_ASSERT(success.second);
}

