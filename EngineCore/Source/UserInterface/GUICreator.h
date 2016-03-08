#pragma once

//=============================================================================
// GUICreator.h - defines a staic class which help us a create element for
//                  GUI. This class wouldn´t be necessary if we had a editor.                 
//
//=============================================================================

#include "..\Msvc\CthughaStd.h"

#include "..\Graphics\GUI\UINode.h"

//////////////////////////////////////////////////////////////////////////
//
//	GUICreator class definition
//
//////////////////////////////////////////////////////////////////////////

class GUICreator
{
public:

	static std::shared_ptr<UINode> OnCreateButton(GameViewId viewID, std::string name, std::string backgroundTextureSourceName, Vec4 color, //Image Properties
		Vec2 screenPos, Vec2 UIElementDimensions, Vec2 UIElementScale, //Transform Properties
		std::string textFontSourceName, int Size, float lineSpacing, std::string horizontalAlignment, std::string verticalAlignment, Vec4 colorFont, //Text Properties
		bool interactable, Vec4 normalColor, Vec4 hightLightedColor, Vec4 pressedColor, Vec4 disabledColor, float fadeDuration, EntityId parentID); //Button Properties
	
	static std::shared_ptr<UINode> OnCreateImage(GameViewId viewID, std::string name, std::string imageSourceName, Vec4 color, //Image Properties
		Vec2 screenPos, Vec2 UIElementDimensions, Vec2 UIElementScale, EntityId parentID); //Transform Properties
	
	static std::shared_ptr<UINode> OnCreatePanel(GameViewId viewID, std::string name, std::string imageSourceName, Vec4 color, //Image Properties
		Vec2 screenPos, Vec2 UIElementDimensions, Vec2 UIElementScale, EntityId parentID); //Transform Properties
	
	static std::shared_ptr<UINode> OnCreateText(GameViewId viewID, std::string name, Vec2 screenPos, Vec2 UIElementDimensions, Vec2 UIElementScale, //Transform Properties
		std::string textFontSourceName, int Size, float lineSpacing, std::string horizontalAlignment, std::string verticalAlignment, Vec4 colorFont, EntityId parentID); //Text Properties

	static std::shared_ptr<UINode> OnCreateCheckBox(GameViewId viewID, std::string name, Vec2 screenPos, Vec2 UIElementDimensions, Vec2 UIElementScale, //Transform Properties
		std::string backgroundTextureSourceName, Vec4 backColor,	//Background image properties
		std::string checkSourceName, Vec4 checkColor,	//CheckBox image properties
		bool interactable, Vec4 normalColor, Vec4 hightLightedColor, Vec4 pressedColor, Vec4 disabledColor, float fadeDuration, EntityId parentID); //Button Properties
};