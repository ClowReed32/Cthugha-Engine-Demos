//========================================================================
// UIFunctionButton.cpp - implement the class for manager GUI button element in the scene                 
//
//========================================================================

#include "UIFunctionCheckBox.h"
#include "UIScene.h"
#include "..\Material.h"
#include "..\Shader.h"

//---------------------------------------------------------------------------------------------------------------------
// UIFunctionImage Implementation
//---------------------------------------------------------------------------------------------------------------------

UIFunctionCheckBox::UIFunctionCheckBox(WeakGUIComponentPtr pGUIComponent, const bool interactable, const ButtonColors CheckBoxColors, const std::string backgroundImageSource,
	Color backgroundImageColor, const std::string checkImageSource, Color checkImageColor)
{
	// Load Shader from file if it`s necessary
	Resource resource("shaders\\ImageGUIShader.shd");
	shared_ptr<ResHandle> handle = g_pApp->m_ResCache->GetHandle(&resource);

	if (handle.get() == NULL)
	{
		CHG_ERROR("Not found shader resource: ImageGUIShader.shd");
	}

	m_rButtonGUIRenderingShaderID = ((ShaderResourceExtraData*)handle->GetExtra().get())->m_ShaderID;

	FormatDesc *vertexDesc = CHG_NEW FormatDesc[1];

	vertexDesc[0].stream = 0;
	vertexDesc[0].format = FORMAT_FLOAT;
	vertexDesc[0].type = TYPE_VERTEX;
	vertexDesc[0].size = 3;

	m_rButtonGUIRenderingShaderVFID = g_pApp->m_pRenderer->addVertexFormat(vertexDesc, 1, m_rButtonGUIRenderingShaderID);

	delete vertexDesc;

	// Load Images from file if it`s necessary /////////////////////////////////////////////////////

	Resource imageBackResource(backgroundImageSource);
	shared_ptr<ResHandle> imageBackHandle = g_pApp->m_ResCache->GetHandle(&imageBackResource);

	if (imageBackHandle.get() == NULL)
	{
		CHG_ERROR("Not found image resource: " + backgroundImageSource);
	}

	m_rBackgroundTextureID = ((TextureResourceExtraData*)imageBackHandle->GetExtra().get())->GetTexture();

	Resource imageCheckResource(checkImageSource);
	shared_ptr<ResHandle> imageCheckHandle = g_pApp->m_ResCache->GetHandle(&imageCheckResource);

	if (imageCheckHandle.get() == NULL)
	{
		CHG_ERROR("Not found image resource: " + checkImageSource);
	}

	m_rCheckTextureID = ((TextureResourceExtraData*)imageCheckHandle->GetExtra().get())->GetTexture();

	///////////////////////////////////////////////////////////////////////////////////////////////////

	m_Interactable = interactable;
	m_CheckBoxColors[0] = CheckBoxColors.NormalColor;
	m_CheckBoxColors[1] = CheckBoxColors.HighlightedColor;
	m_CheckBoxColors[2] = CheckBoxColors.PressedColor;
	m_CheckBoxColors[3] = CheckBoxColors.DisabledColor;
	m_fFadeDuration = CheckBoxColors.fFadeDuration;
	m_LastColor = CheckBoxColors.NormalColor;

	m_fTimeCounter = 0.0f;
	m_CheckBoxState = m_LastCheckBoxState = STATE_CHECKBOX_NORMAL;
	m_fBlendCoefficient = 0.0f;

	m_Id = 0;
	m_bActive = false;

	m_tOnActionPerformed = NULL;
	m_pUserContext = NULL;

	m_Type = UIFUNCTION_CHECKBOX;
}

bool UIFunctionCheckBox::VOnRender(UIScene *pUIScene)
{
	UINT offset[] = { 0 };
	std::shared_ptr<UINode> pOwner = MakeStrongPtr(m_pOwner);

	Vec4 color = (1.0f - m_fBlendCoefficient)*m_LastColor + m_fBlendCoefficient*m_CheckBoxColors[m_CheckBoxState];
	m_LastColor = color;

	Vec2 screenSize = g_pApp->GetScreenSize();
	Mat4x4 world = pOwner->ToWorld();

	std::shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;
	VertexBufferID screenAlignedQuadVB = pUIScene->GetScreenAlignedQuadVB();

	pRenderer->reset();

	pRenderer->setShader(m_rButtonGUIRenderingShaderID);
	pRenderer->setVertexFormat(m_rButtonGUIRenderingShaderVFID);
	pRenderer->setVertexBuffer(0, 1, &screenAlignedQuadVB, offset);
	pRenderer->setDepthState(pUIScene->m_noDepthTest);
	pRenderer->setRasterizerState(pUIScene->m_noneFaceCulling);
	pRenderer->setBlendState(pUIScene->m_blendSrcAlpha);

	pRenderer->setup2DMode(0.0f, screenSize.x, 0.0f, screenSize.y);

	pRenderer->setShaderConstant4x4f("World", *((Mat4x4*)&transpose(world)));
	pRenderer->setTexture("ImageGUITexture", m_rBackgroundTextureID);
	pRenderer->setShaderConstant4f("ImageGUIColor", color);
	pRenderer->setSamplerState("ImageGUISampler", pUIScene->m_bilinearClamp);

	pRenderer->apply();

	pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);

	if (m_bActive)
	{
		pRenderer->setTexture("ImageGUITexture", m_rCheckTextureID);
		pRenderer->setShaderConstant4f("ImageGUIColor", Vec4(1.0f, 1.0f, 1.0f, 1.0f));

		pRenderer->apply();

		pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);
	}

	return true;
}

bool UIFunctionCheckBox::VOnUpdate(UIScene *pUIScene, DWORD elapsedMs)
{
	float d = elapsedMs*0.001f;

	if (m_fTimeCounter < m_fFadeDuration)
	{
		m_fTimeCounter += d;
		m_fBlendCoefficient = (m_fTimeCounter < m_fFadeDuration) ? m_fTimeCounter / m_fFadeDuration : 1.0f;
	}
	else
	{
		m_fBlendCoefficient = 1.0f;
	}

	return true;
}

bool UIFunctionCheckBox::VOnMsgProc(CHG_Event msg)
{
	std::shared_ptr<UINode> pOwner = MakeStrongPtr(m_pOwner);
	bool bActive = false;

	Mat4x4 world = translate(pOwner->ToWorld().GetPosition());
	Aabb aabbT = pOwner->getNodeAabb().Transform(world);

	switch (msg.eventType)
	{
	case CHG_MOUSEMOTION:

		if (aabbT.Intersects(Vec3((float)msg.mouseEvent.x, (float)msg.mouseEvent.y, 0.0f)))
		{
			m_LastCheckBoxState = m_CheckBoxState;
			m_CheckBoxState = STATE_CHECKBOX_HIGHLIGHT;
			bActive = true;
		}
		else
		{
			m_LastCheckBoxState = m_CheckBoxState;
			m_CheckBoxState = STATE_CHECKBOX_NORMAL;
		}

		break;

	case CHG_MOUSEBUTTONDOWN:

		if (aabbT.Intersects(Vec3((float)msg.mouseEvent.x, (float)msg.mouseEvent.y, 0.0f)))
		{
			m_LastCheckBoxState = m_CheckBoxState;
			m_CheckBoxState = STATE_CHECKBOX_PRESSED;
            bActive = true;
		}

		break;

	case CHG_MOUSEBUTTONUP:

		if (aabbT.Intersects(Vec3((float)msg.mouseEvent.x, (float)msg.mouseEvent.y, 0.0f)))
		{
			m_LastCheckBoxState = m_CheckBoxState;
			m_CheckBoxState = STATE_CHECKBOX_HIGHLIGHT;
            m_bActive = !m_bActive;
			bActive = true;
		}
		else
		{
			m_LastCheckBoxState = m_CheckBoxState;
			m_CheckBoxState = STATE_CHECKBOX_NORMAL;
		}

		break;
	}

	m_fTimeCounter = (m_LastCheckBoxState == m_CheckBoxState) ? m_fTimeCounter : 0.0f;

	if (m_tOnActionPerformed != NULL && m_pUserContext != NULL)
	if (aabbT.Intersects(Vec3((float)msg.mouseEvent.x, (float)msg.mouseEvent.y, 0.0f)))
		m_tOnActionPerformed(msg, m_pUserContext);

	return bActive;
}

void UIFunctionCheckBox::SetActionPerformed(void(*OnActionPerformed)(CHG_Event msg, void* pUserContext), void* pUserContext)
{
	m_tOnActionPerformed = OnActionPerformed;
	m_pUserContext = pUserContext;
}