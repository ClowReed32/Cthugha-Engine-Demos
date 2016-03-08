#pragma once

//=============================================================================
// UIScene.h - defines the class for manager the element which compose the
//                  user interface.           
//
//=============================================================================

#include "UINode.h"

typedef std::map<EntityId, shared_ptr<UINode> > UISceneActorMap;

//////////////////////////////////////////////////////////////////////////
//
//	UIScene class definition
//
//////////////////////////////////////////////////////////////////////////

class UIScene
{
protected:
	shared_ptr<Renderer> m_pRenderer;
	GameViewId			 m_GameViewId;

	UINodeList			 m_UINodes;
	UISceneActorMap 	 m_EntityMap;

	VertexBufferID		 m_ScreenAlignedQuadVBID;
	TextureID			 m_GUIRT;

    ShaderID             m_rAddShaderID;
    VertexFormatID       m_rAddShaderVFID;

public:
	//Sampler States
	SamplerStateID m_linearClamp;
    SamplerStateID m_bilinearClamp;
	SamplerStateID m_trilinearClamp;
	SamplerStateID m_trilinearWrap;
	SamplerStateID m_trilinearMirror;
	SamplerStateID m_pointClamp;
	SamplerStateID m_linearWrap;

    //Rasterize States
    RasterizerStateID	 m_backFaceCulling;
    RasterizerStateID	 m_noneFaceCulling;

    //Depth States
    DepthStateID		 m_noDepthTest;
	DepthStateID		 m_depthTest;

    //Blend States
    BlendStateID		 m_blendAdd;
    BlendStateID		 m_blendSrcAlpha;

	UIScene(shared_ptr<Renderer> renderer, GameViewId gameViewId);
    ~UIScene();

	bool OnRender();
	bool OnRestore();
	bool OnLostDevice();
	bool OnUpdate(const int deltaMilliseconds);
	bool OnMsgProc(CHG_Event msg);

	bool AddGUIFunction(EntityId id, shared_ptr<Entity> pEntity, shared_ptr<UIFunction> uiFunction);
	shared_ptr<UINode> FindEntity(EntityId id);

	// event delegates
	void NewGUIComponentDelegate(IEventDataPtr pEventData);
	void ModifiedGUIComponentDelegate(IEventDataPtr pEventData);			// added post-press!
	void DestroyGUIActorDelegate(IEventDataPtr pEventData);
	void MoveGUIActorDelegate(IEventDataPtr pEventData);

	shared_ptr<Renderer> GetRenderer() { return m_pRenderer; }
    VertexBufferID GetScreenAlignedQuadVB() { return m_ScreenAlignedQuadVBID; }
};