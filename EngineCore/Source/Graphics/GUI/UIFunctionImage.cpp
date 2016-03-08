//========================================================================
// UIFunctionImage.cpp - implement the class for manager GUI image element in the scene                 
//
//========================================================================

#include "UIFunctionImage.h"
#include "UIScene.h"
#include "..\Material.h"
#include "..\Shader.h"

//---------------------------------------------------------------------------------------------------------------------
// UIFunctionImage Implementation
//---------------------------------------------------------------------------------------------------------------------

UIFunctionImage::UIFunctionImage(WeakGUIComponentPtr pGUIComponent, std::string imageResourceName, Vec4 color)
{
	//Load Shader from file if it`s necessary
	Resource resource("shaders\\ImageGUIShader.shd");
	shared_ptr<ResHandle> handle = g_pApp->m_ResCache->GetHandle(&resource);

	if (handle.get() == NULL)
	{
		CHG_ERROR("Not found shader resource: ImageGUIShader.shd");
	}

	m_rImageGUIRenderingShaderID = ((ShaderResourceExtraData*)handle->GetExtra().get())->m_ShaderID;

	if (imageResourceName != "")
	{	
		//Load Shader from file if it`s necessary
		Resource imageResource(imageResourceName);
		shared_ptr<ResHandle> imageHandle = g_pApp->m_ResCache->GetHandle(&imageResource);

		if (imageHandle.get() == NULL)
		{
			CHG_ERROR("Not found image resource: " + imageResourceName);
		}

		m_rImageTextureID = ((TextureResourceExtraData*)imageHandle->GetExtra().get())->GetTexture();
		m_bUseTexture = true;
	}
	else
	{
		m_rImageTextureID = TEXTURE_NONE;
		m_bUseTexture = false;
	}

    m_ImageColor = color;
    m_Id = 0;

    FormatDesc *vertexDesc = CHG_NEW FormatDesc[1];

    vertexDesc[0].stream = 0;
    vertexDesc[0].format = FORMAT_FLOAT;
    vertexDesc[0].type = TYPE_VERTEX;
    vertexDesc[0].size = 3;

    m_rImageGUIRenderingShaderVFID = g_pApp->m_pRenderer->addVertexFormat(vertexDesc, 1, m_rImageGUIRenderingShaderID);

    delete vertexDesc;

    m_Type = UIFUNCTION_IMAGE;
}

bool UIFunctionImage::VOnRender(UIScene *pUIScene)
{
    UINT offset[] = {0};
	std::shared_ptr<UINode> pOwner = MakeStrongPtr(m_pOwner);
    
	if (pOwner->GetUIFunction(UIFUNTION_BUTTON) != NULL)
        return true;

    Vec2 screenSize = g_pApp->GetScreenSize();
	Mat4x4 world = pOwner->ToWorld();
    
	bool invertedX = world.rows[0][0] < 0.0f;
	bool invertedY = world.rows[1][1] < 0.0f;

	world.rows[0][0] = abs(world.rows[0][0]);
	world.rows[1][1] = abs(world.rows[1][1]);

    std::shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;
    VertexBufferID screenAlignedQuadVB = pUIScene->GetScreenAlignedQuadVB();

    pRenderer->reset();

    pRenderer->setShader(m_rImageGUIRenderingShaderID);
    pRenderer->setVertexFormat(m_rImageGUIRenderingShaderVFID);
    pRenderer->setVertexBuffer(0, 1, &screenAlignedQuadVB, offset);
    pRenderer->setDepthState(pUIScene->m_noDepthTest);
    pRenderer->setRasterizerState(pUIScene->m_noneFaceCulling);
	pRenderer->setBlendState(pUIScene->m_blendSrcAlpha);

    pRenderer->setup2DMode(0.0f, screenSize.x, 0.0f, screenSize.y);

	pRenderer->setShaderConstant4x4f("World", *((Mat4x4*)&transpose(world)));
    pRenderer->setTexture("ImageGUITexture", m_rImageTextureID);
    pRenderer->setShaderConstant4f("ImageGUIColor", m_ImageColor);
	pRenderer->setShaderConstant1i("bUseImage", (m_bUseTexture) ? 1 : 0);
    pRenderer->setSamplerState("ImageGUISampler", pUIScene->m_bilinearClamp);
	pRenderer->setShaderConstant1i("iInvertedX", invertedX);
	pRenderer->setShaderConstant1i("iInvertedY", invertedY);

    pRenderer->apply();

    pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);

    return true;
}