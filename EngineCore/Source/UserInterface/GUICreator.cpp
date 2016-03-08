//========================================================================
// GUICreator.cpp : implement GUICreator class
//
//========================================================================

#include "GUICreator.h"

#include "../Msvc/CthughaStd.h"
#include "../CthughaEngine/CthughaApp.h"
#include "../EventManager/Events.h"
#include "../EventManager/EventManagerImpl.h"

//	GUICreator class implementation

std::shared_ptr<UINode> GUICreator::OnCreateCheckBox(GameViewId viewID, std::string name, Vec2 screenPos, Vec2 UIElementDimensions, Vec2 UIElementScale, //Transform Properties
	std::string backgroundTextureSourceName, Vec4 backColor,	//Background image properties
	std::string checkSourceName, Vec4 checkColor,	//CheckBox image properties
	bool interactable, Vec4 normalColor, Vec4 hightLightedColor, Vec4 pressedColor, Vec4 disabledColor, float fadeDuration, EntityId parentID) //Button Properties
{
    std::string checkBoxEntityXML =
	"<UserInterface type = \"CheckBox\" resource = \"\" Collides = \"true\" ID = \"1\" Name = \"" + name + "\">\
		 <Components>\
			 <RectTransformComponent>\
				 <Scale x = \"" + std::to_string(UIElementScale.x) + "\" y = \"" + std::to_string(UIElementScale.y) + "\" />\
				 <Position x = \"" + std::to_string(screenPos.x) + "\" y = \"" + std::to_string(screenPos.y) + "\" />\
				 <Dimension width = \"" + std::to_string(UIElementDimensions.x) + "\" height = \"" + std::to_string(UIElementDimensions.y) + "\" />\
			 </RectTransformComponent>\
			 <CheckBoxUIComponent>\
				 <GameViewId gameViewId = \"" + std::to_string(viewID) + "\" />\
				 <Interactable interactable = \"" + ((interactable) ? "true" : "false") + "\" />\
				 <ImageBackgroundResource Path = \"" + backgroundTextureSourceName + "\" color = \"" + backColor.to_string() + "\" />\
				 <ImageCheckBoxResource Path = \"" + checkSourceName + "\" color = \"" + checkColor.to_string() + "\" />\
				 <BackGroundColors normalColor = \"" + normalColor.to_string() + "\" highLightedColor = \"" + hightLightedColor.to_string() + "\" pressedColor = \"" + pressedColor.to_string() + "\" disabledColor = \"" + disabledColor.to_string() + "\" fadeDuration = \"" + std::to_string(fadeDuration) + "\" />\
			 </CheckBoxUIComponent>\
		 </Components>\
	 </UserInterface>";

    TiXmlDocument doc;
    doc.Parse(checkBoxEntityXML.c_str(), NULL, TIXML_ENCODING_UTF8);

    TiXmlElement* pButtonEntityNode = doc.RootElement();

    // Load and create Terrain entity
    if (pButtonEntityNode)
    {
        StrongEntityPtr pEntity = g_pApp->m_pGame->VCreateStaticEntity(pButtonEntityNode);
        if (pEntity)
        {
            // fire an event letting everyone else know that we created a new actor
            shared_ptr<EvtData_New_Actor> pNewActorEvent(CHG_NEW EvtData_New_Actor(pEntity->GetId()));
            IEventManager::Get()->VQueueEvent(pNewActorEvent);

            shared_ptr<CheckBoxUIComponent> pCheckBoxComponent = MakeStrongPtr(pEntity->GetComponent<CheckBoxUIComponent>(CheckBoxUIComponent::g_Name));

            return pCheckBoxComponent->VGetUIFunction()->GetOwner();
        }
    }

    return NULL;
}

std::shared_ptr<UINode> GUICreator::OnCreateButton(GameViewId viewID, std::string name, std::string backgroundTextureSourceName, Vec4 color,
	Vec2 screenPos, Vec2 UIElementDimensions, Vec2 UIElementScale, 
	std::string textFontSourceName, int Size, float lineSpacing, std::string horizontalAlignment, std::string verticalAlignment, Vec4 colorFont, 
	bool interactable, Vec4 normalColor, Vec4 hightLightedColor, Vec4 pressedColor, Vec4 disabledColor, float fadeDuration, EntityId parentID)
{
    std::string buttonEntityXML =
        "<UserInterface type = \"Button\" resource = \"\" Collides = \"true\" ID = \"1\" Name = \""+ name +"\">\
             <Components>\
                 <RectTransformComponent>\
				     <Scale x = \"" + std::to_string(UIElementScale.x) + "\" y = \"" + std::to_string(UIElementScale.y) + "\" />\
                     <Position x = \""+ std::to_string(screenPos.x) +"\" y = \""+ std::to_string(screenPos.y) +"\" />\
                     <Dimension width = \""+ std::to_string(UIElementDimensions.x) +"\" height = \""+ std::to_string(UIElementDimensions.y) +"\" />\
                 </RectTransformComponent>\
                 <ImageUIComponent>\
				 	 <GameViewId gameViewId = \"" + std::to_string(viewID) + "\" />\
                     <ImageResource Path = \""+ backgroundTextureSourceName +"\" />\
					 <Color color = \""+ color.to_string() +"\" />\
                 </ImageUIComponent>\
                 <ButtonUIComponent>\
				 	 <GameViewId gameViewId = \"" + std::to_string(viewID) + "\" />\
                     <Interactable interactable = \""+ ((interactable) ? "true" : "false") +"\" />\
                     <ButtonColors normalColor = \""+ normalColor.to_string() +"\" highLightedColor = \""+ hightLightedColor.to_string() +"\" pressedColor = \""+ pressedColor.to_string() +"\" disabledColor = \""+ disabledColor.to_string() +"\" fadeDuration = \""+ std::to_string(fadeDuration) +"\" />\
                 </ButtonUIComponent>\
                 <TextUIComponent>\
                     <GameViewId gameViewId = \"" + std::to_string(viewID) + "\" />\
                     <Font type = \"" + textFontSourceName + "\" size = \"" + std::to_string(Size) + "\" color = \"" + colorFont.to_string() + "\" />\
                     <Alignment lineSpacing = \"" + std::to_string(lineSpacing) + "\" horizontalAlignment = \"" + horizontalAlignment + "\" verticalAlignment = \"" + verticalAlignment + "\" />\
                 </TextUIComponent>\
             </Components>\
         </UserInterface>";

	TiXmlDocument doc;
	doc.Parse(buttonEntityXML.c_str(), NULL, TIXML_ENCODING_UTF8);

	TiXmlElement* pButtonEntityNode = doc.RootElement();

	// Load and create Terrain entity
	if (pButtonEntityNode)
	{
		StrongEntityPtr pEntity = g_pApp->m_pGame->VCreateStaticEntity(pButtonEntityNode);
		if (pEntity)
		{
			// fire an event letting everyone else know that we created a new actor
			shared_ptr<EvtData_New_Actor> pNewActorEvent(CHG_NEW EvtData_New_Actor(pEntity->GetId()));
			IEventManager::Get()->VQueueEvent(pNewActorEvent);

            shared_ptr<ImageUIComponent> pImageComponent = MakeStrongPtr(pEntity->GetComponent<ImageUIComponent>(ImageUIComponent::g_Name));

            return pImageComponent->VGetUIFunction()->GetOwner();
		}
	}

	return NULL;
}

std::shared_ptr<UINode> GUICreator::OnCreateImage(GameViewId viewID, std::string name, std::string imageSourceName, Vec4 color,
	Vec2 screenPos, Vec2 UIElementDimensions, Vec2 UIElementScale, EntityId parentID)
{
    std::string imageEntityXML =
		"<UserInterface type = \"Image\" resource = \"\" Collides = \"true\" ID = \"1\" Name = \"" + name + "\">\
             <Components>\
                 <RectTransformComponent>\
					<Scale x = \"" + std::to_string(UIElementScale.x) + "\" y = \"" + std::to_string(UIElementScale.y) + "\" />\
					<Position x = \"" + std::to_string(screenPos.x) + "\" y = \"" + std::to_string(screenPos.y) + "\" />\
					<Dimension width = \"" + std::to_string(UIElementDimensions.x) + "\" height = \"" + std::to_string(UIElementDimensions.y) + "\" />\
                 </RectTransformComponent>\
                 <ImageUIComponent>\
				 	<GameViewId gameViewId = \"" + std::to_string(viewID) + "\" />\
				 	<ImageResource Path = \"" + imageSourceName + "\" />\
					<Color color = \"" + color.to_string() + "\" />\
                 </ImageUIComponent>\
             </Components>\
         </UserInterface>";

	TiXmlDocument doc;
	doc.Parse(imageEntityXML.c_str(), NULL, TIXML_ENCODING_UTF8);

	TiXmlElement* pImageEntityNode = doc.RootElement();

	// Load and create Terrain entity
	if (pImageEntityNode)
	{
		StrongEntityPtr pEntity = g_pApp->m_pGame->VCreateStaticEntity(pImageEntityNode);
		if (pEntity)
		{
			// fire an event letting everyone else know that we created a new actor
			shared_ptr<EvtData_New_Actor> pNewActorEvent(CHG_NEW EvtData_New_Actor(pEntity->GetId()));
			IEventManager::Get()->VQueueEvent(pNewActorEvent);

			shared_ptr<ImageUIComponent> pImageComponent = MakeStrongPtr(pEntity->GetComponent<ImageUIComponent>(ImageUIComponent::g_Name));

			return pImageComponent->VGetUIFunction()->GetOwner();
		}
	}

	return NULL;
}

std::shared_ptr<UINode> GUICreator::OnCreatePanel(GameViewId viewID, std::string name, std::string imageSourceName, Vec4 color, Vec2 screenPos, Vec2 UIElementDimensions, Vec2 UIElementScale, EntityId parentID)
{
    std::string panelEntityXML =
		"<UserInterface type = \"Panel\" resource = \"\" Collides = \"true\" ID = \"1\" Name = \"" + name + "\">\
             <Components>\
                 <RectTransformComponent>\
					<Scale x = \"" + std::to_string(UIElementScale.x) + "\" y = \"" + std::to_string(UIElementScale.y) + "\" />\
					<Position x = \"" + std::to_string(screenPos.x) + "\" y = \"" + std::to_string(screenPos.y) + "\" />\
					<Dimension width = \"" + std::to_string(UIElementDimensions.x) + "\" height = \"" + std::to_string(UIElementDimensions.y) + "\" />\
				 </RectTransformComponent>\
                 </RectTransformComponent>\
                 <ImageUIComponent>\
				 	<GameViewId gameViewId = \"" + std::to_string(viewID) + "\" />\
					<ImageResource Path = \"" + imageSourceName + "\" />\
					<Color color = \"" + color.to_string() + "\" />\
                 </ImageUIComponent>\
             </Components>\
         </UserInterface>";

	TiXmlDocument doc;
	doc.Parse(panelEntityXML.c_str(), NULL, TIXML_ENCODING_UTF8);

	TiXmlElement* pPanelEntityNode = doc.RootElement();

	// Load and create Terrain entity
	if (pPanelEntityNode)
	{
		StrongEntityPtr pEntity = g_pApp->m_pGame->VCreateStaticEntity(pPanelEntityNode);
		if (pEntity)
		{
			// fire an event letting everyone else know that we created a new actor
			shared_ptr<EvtData_New_Actor> pNewActorEvent(CHG_NEW EvtData_New_Actor(pEntity->GetId()));
			IEventManager::Get()->VQueueEvent(pNewActorEvent);

			shared_ptr<ImageUIComponent> pImageComponent = MakeStrongPtr(pEntity->GetComponent<ImageUIComponent>(ImageUIComponent::g_Name));

			return pImageComponent->VGetUIFunction()->GetOwner();
		}
	}

	return NULL;
}

std::shared_ptr<UINode> GUICreator::OnCreateText(GameViewId viewID, std::string name, Vec2 screenPos, Vec2 UIElementDimensions, Vec2 UIElementScale, std::string textFontSourceName, int Size, float lineSpacing,
	std::string horizontalAlignment, std::string verticalAlignment, Vec4 colorFont, EntityId parentID)
{
    std::string textEntityXML =
		"<UserInterface type = \"Text\" resource = \"\" Collides = \"true\" ID = \"1\" Name = \"" + name + "\">\
            <Components>\
                <RectTransformComponent>\
					<Scale x = \"" + std::to_string(UIElementScale.x) + "\" y = \"" + std::to_string(UIElementScale.y) + "\" />\
					<Position x = \"" + std::to_string(screenPos.x) + "\" y = \"" + std::to_string(screenPos.y) + "\" />\
					<Dimension width = \"" + std::to_string(UIElementDimensions.x) + "\" height = \"" + std::to_string(UIElementDimensions.y) + "\" />\
                </RectTransformComponent>\
                <TextUIComponent>\
					<GameViewId gameViewId = \"" + std::to_string(viewID) + "\" />\
					<Font type = \"" + textFontSourceName + "\" size = \"" + std::to_string(Size) + "\" color = \"" + colorFont.to_string() + "\" />\
					<Alignment lineSpacing = \"" + std::to_string(lineSpacing) + "\" horizontalAlignment = \"" + horizontalAlignment + "\" verticalAlignment = \"" + verticalAlignment + "\" />\
                </TextUIComponent>\
            </Components>\
        </StaticObject>";

	TiXmlDocument doc;
	doc.Parse(textEntityXML.c_str(), NULL, TIXML_ENCODING_UTF8);

	TiXmlElement* pTextEntityNode = doc.RootElement();

	// Load and create Terrain entity
	if (pTextEntityNode)
	{
		StrongEntityPtr pEntity = g_pApp->m_pGame->VCreateStaticEntity(pTextEntityNode);
		if (pEntity)
		{
			// fire an event letting everyone else know that we created a new actor
			shared_ptr<EvtData_New_Actor> pNewActorEvent(CHG_NEW EvtData_New_Actor(pEntity->GetId()));
			IEventManager::Get()->VQueueEvent(pNewActorEvent);

			shared_ptr<TextUIComponent> pTextComponent = MakeStrongPtr(pEntity->GetComponent<TextUIComponent>(TextUIComponent::g_Name));
			
			return pTextComponent->VGetUIFunction()->GetOwner();
		}
	}

	return NULL;
}