#pragma once

//=============================================================================
// UIFunctionButton.h - defines the class for button element in GUI.           
//
//=============================================================================

#include "UINode.h"

//////////////////////////////////////////////////////////////////////////
//
//	UIFunctionButton class definition
//
//////////////////////////////////////////////////////////////////////////

enum UIFunctionButtonState { STATE_BUTTON_NORMAL, STATE_BUTTON_HIGHLIGHT, STATE_BUTTON_PRESSED };

class UIFunctionButton : public UIFunction
{
private:
    ShaderID m_rButtonGUIRenderingShaderID;
    VertexFormatID m_rButtonGUIRenderingShaderVFID;

    TextureID m_rButtonTextureID;

	bool m_Interactable;
	
    /*Vec4 m_NormalColor;
	Vec4 m_HighlightedColor;
	Vec4 m_PressedColor;
	Vec4 m_DisabledColor;*/

    Vec4 m_LastColor;
    Vec4 m_ButtonColors[4];
    float m_fFadeDuration;

    UIFunctionButtonState m_ButtonState;
    UIFunctionButtonState m_LastButtonState;

    float m_fBlendCoefficient;
    float m_fTimeCounter;

	void* m_pUserContext;
	void(*m_tOnActionPerformed)(CHG_Event msg, void* pUserContext);

public:
	UIFunctionButton(WeakGUIComponentPtr pGUIComponent, const bool interactable, const ButtonColors buttonColors);
	~UIFunctionButton() { };

	virtual bool VOnRender(UIScene *pUIScene) override;
	virtual bool VOnUpdate(UIScene *pUIScene, DWORD const elapsedMs) override;
	//virtual bool VOnRestore(UIScene *pUIScene) override;
	virtual bool VOnMsgProc(CHG_Event msg) override;
	//virtual bool VOnLostDevice(UIScene *pUIScene) override;

	void SetActionPerformed(void(*OnActionPerformed)(CHG_Event msg, void* pUserContext), void* pUserContext);
};