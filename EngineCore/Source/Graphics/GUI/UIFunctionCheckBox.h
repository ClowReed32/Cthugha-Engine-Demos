#pragma once

//=============================================================================
// UIFunctionCheckBox.h - defines the class for checkbox element in GUI.           
//
//=============================================================================

#include "UINode.h"

//////////////////////////////////////////////////////////////////////////
//
//	UIFunctionCheckBox class definition
//
//////////////////////////////////////////////////////////////////////////

enum UIFunctionCheckBoxState { STATE_CHECKBOX_NORMAL, STATE_CHECKBOX_HIGHLIGHT, STATE_CHECKBOX_PRESSED };

class UIFunctionCheckBox : public UIFunction
{
private:
	ShaderID m_rButtonGUIRenderingShaderID;
	VertexFormatID m_rButtonGUIRenderingShaderVFID;

	TextureID m_rBackgroundTextureID;
	TextureID m_rCheckTextureID;
	Color m_BackgroundImageColor;
	Color m_CheckImageColor;

	bool m_Interactable;

	/*Vec4 m_NormalColor;
	Vec4 m_HighlightedColor;
	Vec4 m_PressedColor;
	Vec4 m_DisabledColor;*/

	Vec4 m_LastColor;
	Vec4 m_CheckBoxColors[4];
	float m_fFadeDuration;

	UIFunctionCheckBoxState m_CheckBoxState;
	UIFunctionCheckBoxState m_LastCheckBoxState;

	float m_fBlendCoefficient;
	float m_fTimeCounter;

	void* m_pUserContext;
	void(*m_tOnActionPerformed)(CHG_Event msg, void* pUserContext);

	bool m_bActive;

public:
	UIFunctionCheckBox(WeakGUIComponentPtr pGUIComponent, const bool interactable, const ButtonColors CheckBoxColors, const std::string backgroundImageSource,
		Color backgroundImageColor, const std::string checkImageSource, Color checkImageColor);
	~UIFunctionCheckBox() { };

	virtual bool VOnRender(UIScene *pUIScene) override;
	virtual bool VOnUpdate(UIScene *pUIScene, DWORD const elapsedMs) override;
	//virtual bool VOnRestore(UIScene *pUIScene) override;
	virtual bool VOnMsgProc(CHG_Event msg) override;
	//virtual bool VOnLostDevice(UIScene *pUIScene) override;

	void SetActionPerformed(void(*OnActionPerformed)(CHG_Event msg, void* pUserContext), void* pUserContext);
	bool IsActive() { return m_bActive; }
    void SetActive(bool bActive) { m_bActive = bActive; }
};