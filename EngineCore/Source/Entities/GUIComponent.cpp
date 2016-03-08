#pragma once
//========================================================================
// GUIComponent.cpp : classes that define GUI components of entities like Buttons, Panels, Images, etc.
//
//========================================================================

#include "CthughaStd.h"

#include "..\Utilities\String.h"

#include "..\Graphics\GUI\UINode.h"
#include "..\Graphics\GUI\UIFunctionImage.h"
#include "..\Graphics\GUI\UIFunctionText.h"
#include "..\Graphics\GUI\UIFunctionButton.h"
#include "..\Graphics\GUI\UIFunctionCheckBox.h"
#include "..\EventManager\Events.h"

#include "GUIComponent.h"
#include "RectTransformComponent.h"

const char* ImageUIComponent::g_Name = "ImageUIComponent";
const char* TextUIComponent::g_Name = "TextUIComponent";
const char* ButtonUIComponent::g_Name = "ButtonUIComponent";
const char* CheckBoxUIComponent::g_Name = "CheckBoxUIComponent";

//---------------------------------------------------------------------------------------------------------------------
// RenderComponent
//---------------------------------------------------------------------------------------------------------------------
bool BaseGUIComponent::VInit(TiXmlElement* pData)
{
	return VDelegateInit(pData);
}

void BaseGUIComponent::VPostInit(void)
{
	shared_ptr<UIFunction> pUIFunction(VGetUIFunction());
	shared_ptr<EvtData_New_GUI_Component> pEvent(CHG_NEW EvtData_New_GUI_Component(m_pOwner->GetId(), m_GameViewId, pUIFunction, m_pOwner));
	IEventManager::Get()->VTriggerEvent(pEvent);
}


void BaseGUIComponent::VOnChanged(void)
{
	shared_ptr<EvtData_Modified_GUI_Component> pEvent(CHG_NEW EvtData_Modified_GUI_Component(m_pOwner->GetId(), m_GameViewId));
	IEventManager::Get()->VTriggerEvent(pEvent);
}


TiXmlElement* BaseGUIComponent::VGenerateXml(void)
{
	TiXmlElement* pBaseElement = VCreateBaseElement();

	// create XML for inherited classes
	VCreateInheritedXmlElements(pBaseElement);

	return pBaseElement;
}

shared_ptr<UIFunction> BaseGUIComponent::VGetUIFunction(void)
{
	if (!m_pUIFunction)
		m_pUIFunction = VCreateUIFunction();
	return m_pUIFunction;
}

GameViewId BaseGUIComponent::VGetGameViewID(void)
{
	return m_GameViewId;
}

//---------------------------------------------------------------------------------------------------------------------
// ImageUIComponent
//---------------------------------------------------------------------------------------------------------------------

bool ImageUIComponent::VInit(TiXmlElement* pData)
{
	TiXmlElement* pGameViewIdElement = pData->FirstChildElement("GameViewId");
	if (pGameViewIdElement)
	{
		m_GameViewId = std::atoi(pGameViewIdElement->Attribute("gameViewId"));
	}

	TiXmlElement* pImageResourceElement = pData->FirstChildElement("ImageResource");
	if (pImageResourceElement)
	{
		m_ImageResourceName = pImageResourceElement->Attribute("Path");
	}

	TiXmlElement* pColorElement = pData->FirstChildElement("Color");
	if (pColorElement)
	{
		m_Color = Vec4(pColorElement->Attribute("color"));
	}

	return true;
}

shared_ptr<UIFunction> ImageUIComponent::VCreateUIFunction(void)
{
	// get the transform component
    shared_ptr<RectTransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<RectTransformComponent>(RectTransformComponent::g_Name));
	if (pTransformComponent)
	{
        WeakGUIComponentPtr weakThis(this);
		
		shared_ptr<UIFunction> imageFunction(CHG_NEW UIFunctionImage(weakThis, m_ImageResourceName, m_Color));

        return imageFunction;
	}

    return shared_ptr<UIFunction>();
}

void ImageUIComponent::VCreateInheritedXmlElements(TiXmlElement *)
{
}

//---------------------------------------------------------------------------------------------------------------------
// TextUIComponent
//---------------------------------------------------------------------------------------------------------------------
bool TextUIComponent::VInit(TiXmlElement* pData)
{
	TiXmlElement* pGameViewIdElement = pData->FirstChildElement("GameViewId");
	if (pGameViewIdElement)
	{
		m_GameViewId = std::atoi(pGameViewIdElement->Attribute("gameViewId"));
	}

	TiXmlElement* pFontElement = pData->FirstChildElement("Font");
	if (pFontElement)
	{
		m_FontData.FontTypeResource = pFontElement->Attribute("type");

		m_FontData.fFontSize = (float)std::atof(pFontElement->Attribute("size"));

		m_FontData.Color = Vec4(pFontElement->Attribute("color"));
	}

	TiXmlElement* pAlignmentElement = pData->FirstChildElement("Alignment");
	if (pAlignmentElement)
	{
		m_AlignmentData.fLineSpacing = (float)atof(pAlignmentElement->Attribute("lineSpacing"));
		
		std::string verticalAlignment = pAlignmentElement->Attribute("verticalAlignment");
		m_AlignmentData.VerticalAlignment = (verticalAlignment == "TOP_ALIGN") ? TOP_ALIGN : (verticalAlignment == "MIDDLE_ALIGN") ? MIDDLE_ALIGN : BOTTOM_ALIGN;

		std::string horizontalAlignment = pAlignmentElement->Attribute("horizontalAlignment");
		m_AlignmentData.HorizontalAlignment = (horizontalAlignment == "RIGHT_ALIGN") ? RIGHT_ALIGN : (horizontalAlignment == "CENTER_ALIGN") ? CENTER_ALIGN : LEFT_ALIGN;
	}

	return true;
}

shared_ptr<UIFunction> TextUIComponent::VCreateUIFunction(void)
{
	// get the transform component
	shared_ptr<RectTransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<RectTransformComponent>(RectTransformComponent::g_Name));
	if (pTransformComponent)
	{
		WeakGUIComponentPtr weakThis(this);

		shared_ptr<UIFunction> textFunction(CHG_NEW UIFunctionText(weakThis, m_FontData, m_AlignmentData));

		return textFunction;
	}

	return shared_ptr<UIFunction>();
}

void TextUIComponent::VCreateInheritedXmlElements(TiXmlElement *pBaseElement)
{
}

//---------------------------------------------------------------------------------------------------------------------
// ButtonUIComponent
//---------------------------------------------------------------------------------------------------------------------
bool ButtonUIComponent::VInit(TiXmlElement* pData)
{
	TiXmlElement* pGameViewIdElement = pData->FirstChildElement("GameViewId");
	if (pGameViewIdElement)
	{
		m_GameViewId = std::atoi(pGameViewIdElement->Attribute("gameViewId"));
	}

	TiXmlElement* pInteractableElement = pData->FirstChildElement("Interactable");
	if (pInteractableElement)
	{
		m_bInteractable = std::string(pInteractableElement->Attribute("interactable")) == "true";
	}

	TiXmlElement* pButtonColorElement = pData->FirstChildElement("ButtonColors");
	if (pButtonColorElement)
	{
		m_Colors.NormalColor = Vec4(pButtonColorElement->Attribute("normalColor"));
		m_Colors.HighlightedColor = Vec4(pButtonColorElement->Attribute("highLightedColor"));
		m_Colors.PressedColor = Vec4(pButtonColorElement->Attribute("pressedColor"));
		m_Colors.DisabledColor = Vec4(pButtonColorElement->Attribute("disabledColor"));
		
		m_Colors.fFadeDuration = (float)atof(pButtonColorElement->Attribute("fadeDuration"));
	}

	return true;
}

shared_ptr<UIFunction> ButtonUIComponent::VCreateUIFunction(void)
{
    // get the transform component
    shared_ptr<RectTransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<RectTransformComponent>(RectTransformComponent::g_Name));
    if (pTransformComponent)
    {
        WeakGUIComponentPtr weakThis(this);

        shared_ptr<UIFunction> buttonFunction(CHG_NEW UIFunctionButton(weakThis, m_bInteractable, m_Colors));

        return buttonFunction;
    }

    return shared_ptr<UIFunction>();
}

void ButtonUIComponent::VCreateInheritedXmlElements(TiXmlElement *pBaseElement)
{
}

//---------------------------------------------------------------------------------------------------------------------
// CheckBoxUIComponent
//---------------------------------------------------------------------------------------------------------------------
bool CheckBoxUIComponent::VInit(TiXmlElement* pData)
{
	TiXmlElement* pGameViewIdElement = pData->FirstChildElement("GameViewId");
	if (pGameViewIdElement)
	{
		m_GameViewId = std::atoi(pGameViewIdElement->Attribute("gameViewId"));
	}

	TiXmlElement* pInteractableElement = pData->FirstChildElement("Interactable");
	if (pInteractableElement)
	{
		m_bInteractable = std::string(pInteractableElement->Attribute("interactable")) == "true";
	}

	TiXmlElement* pButtonColorElement = pData->FirstChildElement("BackGroundColors");
	if (pButtonColorElement)
	{
		m_Colors.NormalColor = Vec4(pButtonColorElement->Attribute("normalColor"));
		m_Colors.HighlightedColor = Vec4(pButtonColorElement->Attribute("highLightedColor"));
		m_Colors.PressedColor = Vec4(pButtonColorElement->Attribute("pressedColor"));
		m_Colors.DisabledColor = Vec4(pButtonColorElement->Attribute("disabledColor"));

		m_Colors.fFadeDuration = (float)atof(pButtonColorElement->Attribute("fadeDuration"));
	}

	TiXmlElement* pBackgroundImageElement = pData->FirstChildElement("ImageBackgroundResource");
    if (pBackgroundImageElement)
	{
        m_BackgroundImageResource = pBackgroundImageElement->Attribute("Path");
        m_BackgroundColor = Vec4(pBackgroundImageElement->Attribute("color"));
	}

	TiXmlElement* pCheckImageElement = pData->FirstChildElement("ImageCheckBoxResource");
    if (pCheckImageElement)
	{
        m_CheckImageResource = pCheckImageElement->Attribute("Path");
        m_CheckColor = Vec4(pCheckImageElement->Attribute("color"));
	}

	return true;
}

shared_ptr<UIFunction> CheckBoxUIComponent::VCreateUIFunction(void)
{
	// get the transform component
	shared_ptr<RectTransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<RectTransformComponent>(RectTransformComponent::g_Name));
	if (pTransformComponent)
	{
		WeakGUIComponentPtr weakThis(this);

		shared_ptr<UIFunction> checkBoxFunction(CHG_NEW UIFunctionCheckBox(weakThis, m_bInteractable, m_Colors, m_BackgroundImageResource, m_BackgroundColor, m_CheckImageResource, m_CheckColor));

        return checkBoxFunction;
	}

	return shared_ptr<UIFunction>();
}

void CheckBoxUIComponent::VCreateInheritedXmlElements(TiXmlElement *pBaseElement)
{
}
