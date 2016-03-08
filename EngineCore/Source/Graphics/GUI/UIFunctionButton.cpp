//========================================================================
// UIFunctionButton.cpp - implement the class for manager GUI button element in the scene                 
//
//========================================================================

#include "UIFunctionButton.h"
#include "UIFunctionImage.h"
#include "UIScene.h"
#include "..\Shader.h"

//---------------------------------------------------------------------------------------------------------------------
// UIFunctionImage Implementation
//---------------------------------------------------------------------------------------------------------------------

UIFunctionButton::UIFunctionButton(WeakGUIComponentPtr pGUIComponent, const bool interactable, const ButtonColors buttonColors)
{
    //Load Shader from file if it`s necessary
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

	m_Interactable = interactable;
    m_ButtonColors[0] = buttonColors.NormalColor;
    m_ButtonColors[1] = buttonColors.HighlightedColor;
    m_ButtonColors[2] = buttonColors.PressedColor;
    m_ButtonColors[3] = buttonColors.DisabledColor;
	m_fFadeDuration = buttonColors.fFadeDuration;
    m_LastColor = buttonColors.NormalColor;

    m_rButtonTextureID = TEXTURE_NONE;

    m_fTimeCounter = 0.0f;
    m_ButtonState = m_LastButtonState =STATE_BUTTON_NORMAL;
    m_fBlendCoefficient = 0.0f;

	m_Id = 0;

	m_tOnActionPerformed = NULL;
	m_pUserContext = NULL;

	m_Type = UIFUNTION_BUTTON;
}

bool UIFunctionButton::VOnRender(UIScene *pUIScene)
{
	UINT offset[] = { 0 };
	std::shared_ptr<UINode> pOwner = MakeStrongPtr(m_pOwner);

	//printf("Se dibuja el boton: %d\n", pOwner->GetEntityID());

	if (m_rButtonTextureID == TEXTURE_NONE)
    {
		std::shared_ptr<UIFunctionImage> pImage = std::static_pointer_cast<UIFunctionImage>(pOwner->GetUIFunction(UIFUNCTION_IMAGE));
        m_rButtonTextureID = pImage->m_rImageTextureID;
    }

    Vec4 color = (1.0f - m_fBlendCoefficient)*m_LastColor + m_fBlendCoefficient*m_ButtonColors[m_ButtonState];
    m_LastColor = color;

    Vec2 screenSize = g_pApp->GetScreenSize();
	Mat4x4 world = pOwner->ToWorld();

	bool invertedX = world.rows[0][0] < 0.0f;
	bool invertedY = world.rows[1][1] < 0.0f;

	world.rows[0][0] = abs(world.rows[0][0]);
	world.rows[1][1] = abs(world.rows[1][1]);

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
    pRenderer->setTexture("ImageGUITexture", m_rButtonTextureID);
    pRenderer->setShaderConstant4f("ImageGUIColor", color);
    pRenderer->setSamplerState("ImageGUISampler", pUIScene->m_bilinearClamp);
	pRenderer->setShaderConstant1i("iInvertedX", invertedX);
	pRenderer->setShaderConstant1i("iInvertedY", invertedY);

    pRenderer->apply();

    pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);

    return true;
}

bool UIFunctionButton::VOnUpdate(UIScene *pUIScene, DWORD elapsedMs)
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

bool UIFunctionButton::VOnMsgProc(CHG_Event msg)
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
                m_LastButtonState = m_ButtonState;
                m_ButtonState = STATE_BUTTON_HIGHLIGHT;
				bActive = true;
            }
            else
            {
                m_LastButtonState = m_ButtonState;
                m_ButtonState = STATE_BUTTON_NORMAL;
            }

            break;

        case CHG_MOUSEBUTTONDOWN:
            
			if (aabbT.Intersects(Vec3((float)msg.mouseEvent.x, (float)msg.mouseEvent.y, 0.0f)))
            {
                m_LastButtonState = m_ButtonState;
                m_ButtonState = STATE_BUTTON_PRESSED;
				bActive = true;
            }
            
            break;

        case CHG_MOUSEBUTTONUP:
            
			if (aabbT.Intersects(Vec3((float)msg.mouseEvent.x, (float)msg.mouseEvent.y, 0.0f)))
            {
                m_LastButtonState = m_ButtonState;
                m_ButtonState = STATE_BUTTON_HIGHLIGHT;
				bActive = true;
            }
            else
            {
                m_LastButtonState = m_ButtonState;
                m_ButtonState = STATE_BUTTON_NORMAL;
            }

            break;
    }

    m_fTimeCounter = (m_LastButtonState == m_ButtonState) ? m_fTimeCounter : 0.0f;

	if (m_tOnActionPerformed != NULL && m_pUserContext != NULL)
	if (aabbT.Intersects(Vec3((float)msg.mouseEvent.x, (float)msg.mouseEvent.y, 0.0f)))
			m_tOnActionPerformed(msg, m_pUserContext);

	return bActive;
}

void UIFunctionButton::SetActionPerformed(void(*OnActionPerformed)(CHG_Event msg, void* pUserContext), void* pUserContext)
{
	m_tOnActionPerformed = OnActionPerformed;
	m_pUserContext = pUserContext;
}