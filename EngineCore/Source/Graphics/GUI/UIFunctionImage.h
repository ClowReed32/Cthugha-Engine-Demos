#pragma once

//=============================================================================
// UIFunctionImage.h - defines the class for image element in GUI.           
//
//=============================================================================

#include "UINode.h"

//////////////////////////////////////////////////////////////////////////
//
//	UIFunctionImage class definition
//
//////////////////////////////////////////////////////////////////////////

class UIFunctionImage : public UIFunction
{
private:
    ShaderID m_rImageGUIRenderingShaderID;
    VertexFormatID m_rImageGUIRenderingShaderVFID;

    Vec4 m_ImageColor;
    TextureID m_rImageTextureID;
	bool m_bUseTexture;

    friend class UIFunctionButton;

public:
    UIFunctionImage(WeakGUIComponentPtr pGUIComponent, std::string imageResourceName, Vec4 color);
    ~UIFunctionImage() { };

    virtual bool VOnRender(UIScene *pUIScene) override;
    /*virtual bool VOnUpdate(UIScene *pUIScene, DWORD const elapsedMs) override;
    virtual bool VOnRestore(UIScene *pUIScene) override;
    virtual bool VOnMsgProc(CHG_Event msg) override;
    virtual bool VOnLostDevice(UIScene *pUIScene) override;*/
};