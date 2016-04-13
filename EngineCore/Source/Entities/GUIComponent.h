#pragma once
//==========================================================================
// GUIComponent.h : classes that define GUI components of actors like 
//					Buttons, Images, Sliders, etc.
//
//==========================================================================

#include "GUIComponentInterface.h"
#include "EntityResource.h"

class ComponentResource;

//---------------------------------------------------------------------------------------------------------------------
// GUIComponent base class.  This class does most of the work except actually creating the GUI scene, which is 
// delegated to the subclass through a factory method.
//---------------------------------------------------------------------------------------------------------------------
class BaseGUIComponent : public GUIComponentInterface
{
protected:
	shared_ptr<UIFunction> m_pUIFunction;
	GameViewId m_GameViewId;

public:
	virtual bool VInit(TiXmlElement* pData) override;
    virtual bool VInit(std::shared_ptr<ComponentResource> pComponentResource) override { return true; }
	virtual void VPostInit(void) override;
	virtual void VOnChanged(void) override;
	virtual TiXmlElement* VGenerateXml(void) override;
	virtual shared_ptr<UIFunction> VGetUIFunction(void) override;

protected:
	virtual bool VDelegateInit(TiXmlElement* pData) { return true; }
	virtual shared_ptr<UIFunction> VCreateUIFunction(void) = 0;  // factory method to create the appropriate UI Function

	// editor stuff
	virtual TiXmlElement* VCreateBaseElement(void) { return CHG_NEW TiXmlElement(VGetName()); }
	virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement) = 0;

private:
	virtual GameViewId VGetGameViewID(void) override;
};


//---------------------------------------------------------------------------------------------------------------------
//
//	ImageUIComponent class definition
//
//---------------------------------------------------------------------------------------------------------------------
class ImageUIComponent : public BaseGUIComponent
{
public:
	static const char *g_Name;

	virtual const char *VGetName() const { return g_Name; }
	virtual bool VInit(TiXmlElement* pData) override;

	ImageUIComponent(void)
	{
		m_ImageResourceName = "";
	};

protected:
	// Image Properties
	std::string m_ImageResourceName;
	Vec4 m_Color;

	virtual shared_ptr<UIFunction> VCreateUIFunction(void) override;  // factory method to create the appropriate UI function

	// editor stuff
	virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);
};

//---------------------------------------------------------------------------------------------------------------------
//
//	TextUIComponent class definition
//
//---------------------------------------------------------------------------------------------------------------------

enum VerticalAlignment {TOP_ALIGN, MIDDLE_ALIGN, BOTTOM_ALIGN};
enum HorizontalAlignment {LEFT_ALIGN, CENTER_ALIGN, RIGHT_ALIGN};

struct FontData
{
	std::string FontTypeResource;
	Vec4 Color;
	float fFontSize;
};

struct AlignmentData
{
	float fLineSpacing;
	HorizontalAlignment HorizontalAlignment;
	VerticalAlignment VerticalAlignment;
};

class TextUIComponent : public BaseGUIComponent
{
public:
	static const char *g_Name;

	virtual const char *VGetName() const { return g_Name; }
	virtual bool VInit(TiXmlElement* pData) override;

	TextUIComponent(void)
	{
		m_FontData.FontTypeResource = "";
		m_FontData.fFontSize = 10.0f;
		m_AlignmentData.fLineSpacing = 0.1f;
		m_AlignmentData.HorizontalAlignment = CENTER_ALIGN;
		m_AlignmentData.VerticalAlignment = MIDDLE_ALIGN;
	};

protected:
	// Text Properties
	FontData m_FontData;
	AlignmentData m_AlignmentData;


	virtual shared_ptr<UIFunction> VCreateUIFunction(void) override;  // factory method to create the appropriate UI function

	// editor stuff
	virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);
};

//---------------------------------------------------------------------------------------------------------------------
//
//	ButtonUIComponent class definition
//
//---------------------------------------------------------------------------------------------------------------------

struct ButtonColors
{
	Vec4 NormalColor;
	Vec4 HighlightedColor;
	Vec4 PressedColor;
	Vec4 DisabledColor;
	float fFadeDuration;
};

class ButtonUIComponent : public BaseGUIComponent
{
public:
	static const char *g_Name;

	virtual const char *VGetName() const { return g_Name; }
	virtual bool VInit(TiXmlElement* pData) override;

	ButtonUIComponent(void)
	{
		m_bInteractable = true;
		m_Colors.fFadeDuration = 0.0f;
	};

protected:
	// Button Properties
	bool m_bInteractable;
	ButtonColors m_Colors;

	virtual shared_ptr<UIFunction> VCreateUIFunction(void) override;  // factory method to create the appropriate UI function

	// editor stuff
	virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);
};

class CheckBoxUIComponent : public BaseGUIComponent
{
public:
	static const char *g_Name;

	virtual const char *VGetName() const { return g_Name; }
	virtual bool VInit(TiXmlElement* pData) override;

	CheckBoxUIComponent(void)
	{
		m_bInteractable = true;
		m_Colors.fFadeDuration = 0.0f;
	};

protected:
	// Button Properties
	bool m_bInteractable;
	ButtonColors m_Colors;
	std::string m_BackgroundImageResource;
	std::string m_CheckImageResource;
	Vec4 m_BackgroundColor;
	Vec4 m_CheckColor;

	virtual shared_ptr<UIFunction> VCreateUIFunction(void) override;  // factory method to create the appropriate UI function

	// editor stuff
	virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);
};