#pragma once
//========================================================================
// CthughaApp.h : Defines the entry point for the application.
//
//
//========================================================================

#pragma warning (disable : 4146)

#include "..\CthughaEngine\BaseGameLogic.h"
#include "..\MainLoop\Initialization.h"
#include "..\Graphics\SceneNodes.h"
#include "..\Graphics\GUI\UINode.h"
#include "..\Graphics\TextHelper.h"
#include "..\EventManager\EventManagerImpl.h"
#include "..\Graphics\ShadowMap.h"
#include "..\Graphics\AditionalShadowDataManager.h"
#include "..\Graphics\VoxelManager.h"

class Renderer;

//
// class GameCodeApp							- Chapter X, page Y
//
class CthughaApp
{
protected:

	bool m_bWindowedMode;					// true if the app is windowed, false if fullscreen
	bool m_bIsRunning;						// true if everything is initialized and the game is in the main loop
	bool m_bQuitRequested;					// true if the app should run the exit sequence
	bool m_bQuitting;						// true if the app is running the exit sequence
	Vec4 m_rcDesktop;						// current desktop size - not necessarilly the client window size
	Vec2 m_screenSize;						// game screen size
	int m_iColorDepth;						// current color depth (16 or 32)
	bool m_bIsEditorRunning;				// true if the game editor is running
	SDL_Window *m_pWindowHandle;			// handler window
	SDL_GLContext m_MainContext;

	//Time member
	clock_t m_lastUpdateTime;				// last Update time in milliseconds
    float m_fElapsedTime;
	int m_iFrameNumber;

	clock_t m_lastDraw;


public:
	const Vec2 GetScreenSize()  { return m_screenSize; }
	const int GetFrameNumber() { return m_iFrameNumber; }

protected:
	std::map<std::wstring,std::wstring> m_textResource;
	std::map<std::wstring,UINT> m_hotkeys;

	int m_HasModalDialog;					// determines if a modal dialog is up

public:

	CthughaApp();

	// Game Application Data
	virtual char *VGetGameTitle()=0;

	SDL_Window* GetHwnd();
	virtual bool InitInstance(int argc, char *argv[], int screenWidth = SCREEN_WIDTH, int screenHeight = SCREEN_HEIGHT, bool windowedMode = true);	
	bool CreateWindowApp( const char* strWindowTitle, int x = -2147483648, int y = -2147483648 );
	bool InitRenderer();

	bool LoadStrings(std::string language);
	std::wstring GetString(std::wstring sID);				
	int GetHotKeyForString(std::wstring sID);
	UINT MapCharToKeycode(const char pHotkey);

	void getTimeValues(double* fTime, float* fElapsedTime);
    float getElapsedTime() { return m_fElapsedTime; }
	Uint32 getTick();

	shared_ptr<Renderer> m_pRenderer;
	shared_ptr<AdditionalShadowDataManager> m_pAditionalShadowDataManager;
	shared_ptr<SDFShadowManager> m_pSDFShadowManager;
	shared_ptr<VoxelManager> m_pVoxelManager;
	shared_ptr<CHGTextHelper> m_pTextHelper;

	static shared_ptr<Renderer> GetRendererImpl();

	int MainLoop();
	int OnProcessEvent(SDL_Event* Event);
	int OnUpdateGameAndRendering();
	void OnUpdateGame(double fTime, float fElapsedTime);
	void OnFrameRender(double fTime, float fElapsedTime);
	void OnShutDownGame();

	// GameCode Specific Stuff
	BaseGameLogic *m_pGame;
	struct GameOptions m_Options;

	// You must define these functions to initialize your game.
	virtual BaseGameLogic *VCreateGameAndView()=0;
	virtual bool VLoadGame(void);

	// File and Resource System
	class ResCache *m_ResCache;

	bool IsEditorRunning() { return m_bIsEditorRunning; }

	// Event manager
	EventManager *m_pEventManager;

	void SetMouseMode(CHG_MouseMode mode);

public:

	// Main loop processing
	void AbortGame() { m_bQuitting = true; }
	bool IsRunning() { return m_bIsRunning; }
	void SetQuitting(bool quitting) { m_bQuitting = quitting; }

    BaseGameLogic* GetGameLogic(void) const { return m_pGame; }

protected:
    virtual void VRegisterGameEvents(void) {}

private:
    void RegisterEngineEvents(void);
};



extern CthughaApp *g_pApp;