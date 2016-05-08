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
// class CthughaApp							
//
class CthughaApp
{
protected:

	bool m_bWindowedMode;					// true if the app is windowed, false if fullscreen
	bool m_bIsRunning;						// true if everything is initialized and the game is in the main loop
	bool m_bQuitRequested;					// true if the app should run the exit sequence
	bool m_bQuitting;						// true if the app is running the exit sequence
	std::vector<Vec2> m_screenSize;			// game screen size
	SDL_Window *m_pWindowHandle;			// SDL window handler

	//Time member
	clock_t m_lastUpdateTime;				// last Update time in milliseconds
    float m_fElapsedTime;
	int m_iFrameNumber;
	clock_t m_lastDraw;

public:
	const Vec2 GetScreenSize() 	{	return m_screenSize.size() > 0 ? m_screenSize[0] : Vec2(0,0);	}
	const Vec2 GetScreenSize(UINT uWindowId) { return m_screenSize.size() > uWindowId ? m_screenSize[uWindowId] : Vec2(0, 0); }
	const int GetFrameNumber() { return m_iFrameNumber; }

protected:
	std::map<std::wstring,std::wstring> m_textResource;
	std::map<std::wstring,UINT> m_hotkeys;

	int m_HasModalDialog;					// determines if a modal dialog is up

public:
	// GameCode Specific Stuff
	BaseGameLogic *m_pGame;
	struct GameOptions m_Options;

	// Graphics 
	shared_ptr<Renderer> m_pRenderer;
	shared_ptr<AdditionalShadowDataManager> m_pAditionalShadowDataManager;
	shared_ptr<SDFShadowManager> m_pSDFShadowManager;
	shared_ptr<VoxelManager> m_pVoxelManager;
	shared_ptr<CHGTextHelper> m_pTextHelper; //WARNING. render letters very vlow.

	// File and Resource System
	class ResCache *m_ResCache;

	// Event manager
	EventManager *m_pEventManager;

	//------------------------------------------------------------------------------------------------------------------------------------------------
	CthughaApp();

	// Game Application Data
	virtual char *VGetGameTitle()=0;

	// Windows handler and renderer methods
	SDL_Window* GetHwnd();
	virtual bool InitInstance(int argc, char *argv[], HWND *hwndArray, int *screenWidth, int *screenHeight, UINT numWindows = 1, bool windowedMode = true);
	bool CreateWindowApp( const char* strWindowTitle, int x = -2147483648, int y = -2147483648 );
	bool InitRenderer(/*Several windows*/HWND *hwndArray = NULL, UINT *screenWidth = NULL, UINT *screenHeight = NULL, UINT numWindows = 1);

	// Text management methods
	bool LoadStrings(std::string language);
	std::wstring GetString(std::wstring sID);				
	int GetHotKeyForString(std::wstring sID);
	UINT MapCharToKeycode(const char pHotkey);

	// Time methods
	void getTimeValues(double* fTime, float* fElapsedTime);
    float getElapsedTime() { return m_fElapsedTime; }
	Uint32 getTick();

	static shared_ptr<Renderer> GetRendererImpl();

	// Game loop 
	int MainLoop();
	int OnProcessEvent(SDL_Event* Event);	// SDL Event
	int OnProcessEvent(UINT msg, UINT wParam, UINT lParam);	// Windows Event
	int OnUpdateGameAndRendering();
	void OnUpdateGame(double fTime, float fElapsedTime);
	void OnFrameRender(double fTime, float fElapsedTime);
	void OnShutDownGame();

	// You must define these functions to initialize your game.
	virtual BaseGameLogic *VCreateGameAndView()=0;
	virtual bool VLoadGame(void);

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
