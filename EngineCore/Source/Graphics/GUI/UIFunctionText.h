#pragma once

//=============================================================================
// UIFunctionText.h - defines the class for image element in GUI.           
//
//=============================================================================

#include "UINode.h"

//////////////////////////////////////////////////////////////////////////
//
//	UIFunctionText class definition
//
//////////////////////////////////////////////////////////////////////////

class UIFunctionText : public UIFunction
{
private:
	FontID m_rTextFontID;
	UINT m_uFontSize;
	Vec4 m_FontColor;
	float m_fLineSpacing;
	HorizontalAlignment m_HorizontalAlignment;
	VerticalAlignment m_VerticalAlignment;

	std::string m_Text;

public:
	UIFunctionText(WeakGUIComponentPtr pGUIComponent, const FontData fontData, const AlignmentData alignmentData);
	~UIFunctionText() { };

	virtual bool VOnRender(UIScene *pUIScene) override;
	/*virtual bool VOnUpdate(UIScene *pUIScene, DWORD const elapsedMs) override;
	virtual bool VOnRestore(UIScene *pUIScene) override;
	virtual bool VOnMsgProc(CHG_Event msg) override;
	virtual bool VOnLostDevice(UIScene *pUIScene) override;*/

	void SetText(std::string text) { m_Text = text; }
	std::string GetText() { return m_Text; }
};