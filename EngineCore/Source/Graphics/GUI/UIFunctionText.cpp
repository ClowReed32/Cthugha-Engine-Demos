//========================================================================
// UIFunctionText.cpp - implement the class for manager GUI text element in the scene                 
//
//========================================================================

#include "UIFunctionText.h"
#include "UIScene.h"
#include "..\BitMapFont.h"

//---------------------------------------------------------------------------------------------------------------------
// UIFunctionImage Implementation
//---------------------------------------------------------------------------------------------------------------------

UIFunctionText::UIFunctionText(WeakGUIComponentPtr pGUIComponent, const FontData fontData, const AlignmentData alignmentData)
{
	//Load font from file if it`s necessary
	Resource resource(fontData.FontTypeResource);
	shared_ptr<ResHandle> handle = g_pApp->m_ResCache->GetHandle(&resource);

	if (handle.get() == NULL)
	{
		CHG_ERROR("Not found font resource: " + fontData.FontTypeResource);
	}

	m_rTextFontID = ((BitMapFontResourceExtraData*)handle->GetExtra().get())->GetFontID();

	m_uFontSize = (UINT)fontData.fFontSize;
	m_FontColor = fontData.Color;;
	m_fLineSpacing = alignmentData.fLineSpacing;

	m_HorizontalAlignment = alignmentData.HorizontalAlignment;
	m_VerticalAlignment = alignmentData.VerticalAlignment;

	m_Id = 0;

	m_Type = UIFUNCTION_TEXT;
}

bool UIFunctionText::VOnRender(UIScene *pUIScene)
{
	std::shared_ptr<UINode> pOwner = MakeStrongPtr(m_pOwner);

	Vec2 screenSize = g_pApp->GetScreenSize();
	Mat4x4 world = translate(pOwner->ToWorld().GetPosition());
	Aabb aabbT = pOwner->getNodeAabb().Transform(world);

	std::shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;
	VertexBufferID screenAlignedQuadVB = pUIScene->GetScreenAlignedQuadVB();

	float textHeight = m_uFontSize * pRenderer->getTextHeight(m_rTextFontID, m_Text.c_str(), m_Text.size());
	float textWidth = m_uFontSize * pRenderer->getTextWidth(m_rTextFontID, m_Text.c_str(), m_Text.size());

	float x = (m_HorizontalAlignment == LEFT_ALIGN) ? aabbT.vcMin.x : (m_HorizontalAlignment == CENTER_ALIGN) ? aabbT.vcCenter.x - textWidth*0.5f : aabbT.vcMax.x - textWidth;
	float y = (m_VerticalAlignment == TOP_ALIGN) ? aabbT.vcMin.y : (m_VerticalAlignment == MIDDLE_ALIGN) ? aabbT.vcCenter.y - textHeight*0.5f : aabbT.vcMax.y - textHeight;

	pRenderer->drawText(m_Text.c_str(), x, y, (float)m_uFontSize, (float)m_uFontSize, m_rTextFontID, pUIScene->m_trilinearClamp, pUIScene->m_blendSrcAlpha, pUIScene->m_noDepthTest, &m_FontColor);

	return true;
}