#pragma once
//========================================================================
// UserInterface.h : Defines UI elements of the GameCode4 application
//
//========================================================================


#include "..\Graphics\Scene.h"
#include "..\Graphics\GUI\UIScene.h"

const DWORD g_QuitNoPrompt = MAKELPARAM(-1,-1);
const UINT g_MsgEndModal = (WM_USER+100);

enum
{
	// Lower numbers get drawn first
	ZORDER_BACKGROUND, 
	ZORDER_LAYER1, 
	ZORDER_LAYER2, 
	ZORDER_LAYER3, 
	ZORDER_TOOLTIP
};


enum MessageBox_Questions {
	QUESTION_WHERES_THE_CD,
	QUESTION_QUIT_GAME,
};

class FontHandler;
class BaseUI;

// This class is a group of user interface controls.
// It can be modal or modeless.

//
// class BaseUI								
//
// This was factored to create a common class that
// implements some of the IScreenElement class common
// to modal/modeless dialogs
//
class BaseUI : public IScreenElement
{
protected:
	int					m_PosX, m_PosY;
	int					m_Width, m_Height;
	optional<int>		m_Result;
	bool				m_bIsVisible;
public:
	BaseUI() { m_bIsVisible = true; m_PosX = m_PosY = 0; m_Width = 100; m_Height = 100; }
	virtual void VOnUpdate(int) { };
	virtual bool VOnLostDevice() { return S_OK; }
	virtual bool VIsVisible() const { return m_bIsVisible; }
	virtual void VSetVisible(bool visible) { m_bIsVisible = visible; }
};


//
// class ScreenElementScene						
//
class ScreenElementScene : public IScreenElement, public Scene
{
public:
	ScreenElementScene(shared_ptr<Renderer> renderer) : Scene(renderer) { }
    virtual ~ScreenElementScene(void)
    {
        CHG_WARNING("~ScreenElementScene()");
    }

	// IScreenElement Implementation
	virtual void VOnUpdate(int deltaMS) { OnUpdate(deltaMS); };
	virtual bool VOnRestore() 
		{ OnRestore(); return true; }
	virtual bool VOnRender(double fTime, float fElapsedTime)
	{
		OnRender(); return true;
	}
	virtual bool VOnLostDevice()
	{
		OnLostDevice(); return true;
	}
	virtual int VGetZOrder() const { return 0; }
	virtual void VSetZOrder(int const zOrder) { }

	// Don't handle any messages
	virtual bool VOnMsgProc( CHG_Event msg ) { return 0; }

	virtual bool VIsVisible() const { return true; }
	virtual void VSetVisible(bool visible) { }
	virtual bool VAddChild(EntityId id, shared_ptr<ISceneNode> kid) { return Scene::AddChild(id, kid); }
};

//
// class ScreenElementUIScene						
//
class ScreenElementUIScene : public IScreenElement, public UIScene
{
public:
	ScreenElementUIScene(shared_ptr<Renderer> renderer, GameViewId gameViewId) : UIScene(renderer, gameViewId) { }
	virtual ~ScreenElementUIScene(void)
	{
		CHG_WARNING("~ScreenElementUIScene()");
	}

	// IScreenElement Implementation
	virtual void VOnUpdate(int deltaMS) { OnUpdate(deltaMS); };
	virtual bool VOnRestore()
	{
		OnRestore(); return true;
	}
	virtual bool VOnRender(double fTime, float fElapsedTime)
	{
		OnRender(); return true;
	}
	virtual bool VOnLostDevice()
	{
		OnLostDevice(); return true;
	}
	virtual int VGetZOrder() const { return 0; }
	virtual void VSetZOrder(int const zOrder) { }

	virtual bool VOnMsgProc(CHG_Event msg) { return OnMsgProc(msg); }

	virtual bool VIsVisible() const { return true; }
	virtual void VSetVisible(bool visible) { }
    virtual bool VAddGUIFunction(EntityId id, shared_ptr<Entity> pEntity, shared_ptr<UIFunction> uiFunction) { return UIScene::AddGUIFunction(id, pEntity, uiFunction); }
};



