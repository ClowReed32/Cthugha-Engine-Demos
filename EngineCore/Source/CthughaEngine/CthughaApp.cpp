//========================================================================
// GameCode.cpp : Defines the Application layer for GameCode4
//
//========================================================================

#include "CthughaStd.h"

#include "../Graphics/D3DX11Renderer.h"
#include "../EventManager/EventManagerImpl.h"
#include "../ResourceCache/ResCache.h"
#include "../ResourceCache/XmlResource.h"
#include "../UserInterface/UserInterface.h"
#include "../UserInterface/HumanView.h"
#include "../Utilities/String.h"
#include "../Graphics/ShadowMap.h"
#include "..\Graphics\AditionalShadowDataManager.h"
#include "..\Graphics\SDFShadowManager.h"

#include "../EventManager/Events.h"

#pragma warning (disable : 4244)

const unsigned int CHG_SCREEN_REFRESH_RATE(CLOCKS_PER_SEC / 60);

CthughaApp *g_pApp = NULL;

CthughaApp::CthughaApp()
{
	g_pApp = this;
	m_pGame = NULL;

	m_bIsRunning = false;

	m_pEventManager = NULL;
	m_ResCache = NULL;

	m_bQuitRequested = false;
	m_bQuitting = false;
	m_HasModalDialog = 0;

	m_lastUpdateTime = 0;
	m_lastDraw = 0;
}


SDL_Window* CthughaApp::GetHwnd()
{ 
	return m_pWindowHandle;
}

bool CthughaApp::CreateWindowApp( const char* strWindowTitle, int x, int y )
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
        CHG_ERROR("Unable to initialize SDL");

		return false;
	}

	/*if (TTF_Init() < 0)
	{
		CHG_ERROR("Unable to initialize TTF");
	}*/
	
	Uint32 flags = SDL_WINDOW_SHOWN;

	if(!m_bWindowedMode)
		flags |= SDL_WINDOW_FULLSCREEN;

	m_pWindowHandle = SDL_CreateWindow(strWindowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_screenSize[0].x, m_screenSize[0].y, flags);

    if (!m_pWindowHandle)
	{
        CHG_ERROR("Unable to create window");

		return false;
	}
	
    return true;
}

bool CthughaApp::InitRenderer(HWND *hwndArray, UINT *screenWidth, UINT *screenHeight, UINT numWindows)
{
	if (!hwndArray)
	{
		// Obtain window handle from sdl window
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(m_pWindowHandle, &wmInfo);
		HWND hwnd = wmInfo.info.win.window;

		HWND hwndArray[] = { hwnd };
		UINT heightArray[] = { m_screenSize[0].y };
		UINT widthArray[] = { m_screenSize[0].x };

		m_pRenderer.reset(CHG_NEW Direct3D11Renderer(hwndArray, heightArray, widthArray, 1, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D24_UNORM_S8_UINT, 1, !m_bWindowedMode));
	}
	else
	{
		m_pRenderer.reset(CHG_NEW Direct3D11Renderer(hwndArray, screenHeight, screenWidth, numWindows, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D24_UNORM_S8_UINT, 1, !m_bWindowedMode));
	}	

	return true;
}

bool CthughaApp::InitInstance(int argc, char *argv[], HWND *hwndArray, int *screenWidth, int *screenHeight, UINT numWindows, bool windowedMode)
{
	// Check for adequate machine resources. I don't use this
	/*bool resourceCheck = false;
	while (!resourceCheck)
	{
		const DWORDLONG physicalRAM = 512 * MEGABYTE;
		const DWORDLONG virtualRAM = 1024 * MEGABYTE;
		const DWORDLONG diskSpace = 10 * MEGABYTE;
		if (!CheckStorage(diskSpace))
			return false;

		const DWORD minCpuSpeed = 1300;			// 1.3Ghz
		DWORD thisCPU = ReadCPUSpeed();
		if (thisCPU < minCpuSpeed)
		{
			CHG_ERROR("GetCPUSpeed reports CPU is too slow for this game.");
			return false;
		}

		resourceCheck = true;
	}*/

    // register all events 
    RegisterEngineEvents();
    VRegisterGameEvents();

	// Initialize the ResCache
	IResourceFile *zipFile = (m_Options.m_useDevelopmentDirectories) ? 
		CHG_NEW DevelopmentResourceZipFile(L"Assets.zip", DevelopmentResourceZipFile::Editor) :
		CHG_NEW ResourceZipFile(L"Assets.zip");

	m_ResCache = CHG_NEW ResCache(50, zipFile);

	if (!m_ResCache->Init())
	{
        CHG_ERROR("Failed to initialize resource cache!  Are your paths set up correctly?");
		return false;
	}
	
    extern shared_ptr<IResourceLoader> CreateRAWResourceLoader();
	extern shared_ptr<IResourceLoader> CreateDDSResourceLoader();
	extern shared_ptr<IResourceLoader> CreateJPGResourceLoader();
	extern shared_ptr<IResourceLoader> CreatePNGResourceLoader();
    extern shared_ptr<IResourceLoader> CreateXmlResourceLoader();
	extern shared_ptr<IResourceLoader> CreateCOLLADAMeshResourceLoader();
	extern shared_ptr<IResourceLoader> CreateBitMapFontResourceLoader();
	extern shared_ptr<IResourceLoader> CreateTTFFontResourceLoader();
	extern shared_ptr<IResourceLoader> CreateShaderResourceLoader();
	extern shared_ptr<IResourceLoader> CreateShaderLibraryResourceLoader();
	extern shared_ptr<IResourceLoader> CreateMeshColliderResourceLoader();
	extern shared_ptr<IResourceLoader> CreateEntityResourceLoader();
	// Signed Distance Field loader //////////////////////////////////////
	extern shared_ptr<IResourceLoader> CreateShadowCoordResourceLoader();
	extern shared_ptr<IResourceLoader> CreateSDFResourceLoader();
	//////////////////////////////////////////////////////////////////////
	extern shared_ptr<IResourceLoader> CreateMaterialResourceLoader();

    m_ResCache->RegisterLoader(CreateRAWResourceLoader());
	m_ResCache->RegisterLoader(CreateDDSResourceLoader());
	m_ResCache->RegisterLoader(CreateJPGResourceLoader());
    m_ResCache->RegisterLoader(CreateXmlResourceLoader());
	m_ResCache->RegisterLoader(CreateCOLLADAMeshResourceLoader());
	m_ResCache->RegisterLoader(CreateBitMapFontResourceLoader());
	m_ResCache->RegisterLoader(CreatePNGResourceLoader());
	m_ResCache->RegisterLoader(CreateTTFFontResourceLoader());
	m_ResCache->RegisterLoader(CreateShaderResourceLoader());
	m_ResCache->RegisterLoader(CreateShaderLibraryResourceLoader());
	m_ResCache->RegisterLoader(CreateMeshColliderResourceLoader());
	m_ResCache->RegisterLoader(CreateEntityResourceLoader());
	m_ResCache->RegisterLoader(CreateShadowCoordResourceLoader());
	m_ResCache->RegisterLoader(CreateSDFResourceLoader());
	m_ResCache->RegisterLoader(CreateMaterialResourceLoader());	

	if(!LoadStrings("English"))
	{
        CHG_ERROR("Failed to load strings");
		return false;
	}

	// The event manager should be created next so that subsystems can hook in as desired.
	m_pEventManager = CHG_NEW EventManager("CthughaEngine Event Mgr", true);
	if (!m_pEventManager)
	{
		CHG_ERROR("Failed to create EventManager.");
		return false;
	}

	// Init SDL window and graphics stuff ///////////////////////////////////////////////
	if (!hwndArray)
	{
		m_screenSize.push_back(Vec2(screenWidth[0], screenHeight[0]));
		m_bWindowedMode = windowedMode;

		CreateWindowApp(VGetGameTitle());

		if (!GetHwnd())
		{
			return FALSE;
		}

		//Init Renderer
		if (!InitRenderer())
		{
			CHG_ERROR("Renderer not Created");
			return false;
		}
	}
	else
	{
		for (UINT i = 0; i < numWindows; ++i)
			m_screenSize.push_back(Vec2(screenWidth[i], screenHeight[i]));
		
		m_bWindowedMode = true;

		//Init Renderer
		if (!InitRenderer(hwndArray, (UINT*)screenWidth, (UINT*)screenHeight, m_screenSize.size()))
		{
			CHG_ERROR("Renderer not Created");
			return false;
		}
	}

	m_iFrameNumber = 0;

	m_pRenderer->resetToDefaults();
	m_pRenderer->reset();
	m_pRenderer->setViewport(screenWidth[0], screenHeight[0]);
	m_pRenderer->changeToMainFramebuffer();

	//Init ShadowMap Manager
	m_pAditionalShadowDataManager.reset(CHG_NEW AdditionalShadowDataManager());

	//Init SDF Shadow Manager
	m_pSDFShadowManager.reset(CHG_NEW SDFShadowManager());

	//Init Voxel Manager
	m_pVoxelManager.reset(CHG_NEW VoxelManager());

	m_pVoxelManager->Initialize(true, true, 128, Vec3(160, 90, 256), 1, 0.5f);
	m_pVoxelManager->ActiveDebugMode(false);	// Draw voxel cubes

	//Init TextHelper for console
	//m_pTextHelper.reset(CHG_NEW CHGTextHelper("font\\arial.ttf"));

	////////////////////////////////////////////////////////////////////////////////////////

	// You usually must have an HWND to initialize your game views...
	//    VCreateGameAndView

	m_pGame = VCreateGameAndView();
	if (!m_pGame)
		return false;

	// now that all the major systems are initialized, preload resources 
	m_ResCache->Preload("*.dds", NULL);

	m_bIsRunning = true;

	return TRUE;
}

bool CthughaApp::VLoadGame(void)
{
    // Read the game options and see what the current game
    // needs to be - all of the game graphics are initialized by now, too...
	return m_pGame->VLoadGame(m_Options.m_Level.c_str());
}

int CthughaApp::OnProcessEvent(SDL_Event* Event)
{
	CHG_Event CHGEvent;
	bool result = false;
	bool processEvent = false;

	switch(Event->type) 
	{
		case SDL_WINDOWEVENT: 
		{
			switch(Event->window.event) 
			{				
				 case SDL_WINDOWEVENT_ENTER:
					
					break;

				case SDL_WINDOWEVENT_LEAVE:
					
					break;

				case SDL_WINDOWEVENT_FOCUS_GAINED:
									
					break;

				case SDL_WINDOWEVENT_FOCUS_LOST:
					
					break;

			}
			break;
		}

		case SDL_KEYDOWN: 
		case SDL_KEYUP: 
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN: 
		case SDL_MOUSEBUTTONUP:	
		{
			CHGEvent.eventType = (CHG_EventType)Event->type;
			CHGEvent.keyboardEvent.asciiCode = Event->key.keysym.sym;
			CHGEvent.mouseEvent.x = Event->button.x;
			CHGEvent.mouseEvent.y = Event->button.y;
			CHGEvent.mouseEvent.relX = Event->motion.xrel;
			CHGEvent.mouseEvent.relY = Event->motion.yrel;
			CHGEvent.mouseEvent.pressed = Event->button.state == SDL_PRESSED;

			if(CHGEvent.eventType == SDL_MOUSEBUTTONUP || CHGEvent.eventType == SDL_MOUSEBUTTONDOWN)
			{
				switch(Event->button.button) 
				{
					case SDL_BUTTON_LEFT: 
					{
						CHGEvent.mouseEvent.mouseButton = CHG_BUTTON_LEFT;
						break;
					}
					case SDL_BUTTON_RIGHT:
					{
						CHGEvent.mouseEvent.mouseButton = CHG_BUTTON_RIGHT;
						break;
					}
					case SDL_BUTTON_MIDDLE: 
					{
						CHGEvent.mouseEvent.mouseButton = CHG_BUTTON_MIDDLE;
						break;
					}
				}
			}

			if (g_pApp->m_pGame)
			{
				BaseGameLogic *pGame = g_pApp->m_pGame;
			
				for(GameViewList::reverse_iterator i=pGame->m_gameViews.rbegin(); i!=pGame->m_gameViews.rend(); ++i)
				{
					if ( (*i)->VOnMsgProc( CHGEvent ) )
					{
						result = true;
						break;				// WARNING! This breaks out of the for loop.
					}
				}
			}
		}

		case SDL_QUIT: 
		{
			
			break;
		}

		case SDL_SYSWMEVENT: 
		{
			//Ignore
			break;
		}

		case SDL_WINDOWEVENT_RESIZED: 
		{
			
			break;
		}

		default: 
		{
			
			break;
		}
	}

	return 0;
}

int CthughaApp::OnProcessEvent(UINT msg, UINT wParam, UINT lParam)
{
	CHG_Event CHGEvent;

	switch (msg)
	{
		case WM_POWERBROADCAST:
		{
			break;
		}

		case WM_DISPLAYCHANGE:
		{
			/*int colorDepth = (int)wParam;
			int width = (int)(short)LOWORD(lParam);
			int height = (int)(short)HIWORD(lParam);

			result = g_pApp->OnDisplayChange(colorDepth, width, height);*/
			break;
		}

		case WM_SYSCOMMAND:
		{
			break;
		}

		case WM_SYSKEYDOWN:
		{
			/*if (wParam == VK_RETURN)
			{
				*pbNoFurtherProcessing = true;
				return g_pApp->OnAltEnter();
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);*/
		}


		case WM_CLOSE:
		{
			break;
		}


		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		{
			if (msg == WM_KEYDOWN || msg == WM_KEYUP)
			{
				CHGEvent.eventType = (CHG_EventType)(msg + 512);
				CHGEvent.keyboardEvent.asciiCode = (char)tolower(wParam);
			}
			else if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
			{
				CHGEvent.eventType = (msg == WM_LBUTTONDOWN) ? CHG_MOUSEBUTTONDOWN : CHG_MOUSEBUTTONUP;
				CHGEvent.mouseEvent.x = GET_X_LPARAM(lParam);
				CHGEvent.mouseEvent.y = GET_Y_LPARAM(lParam);
				CHGEvent.mouseEvent.relX = 0;
				CHGEvent.mouseEvent.relY = 0;
				CHGEvent.mouseEvent.pressed = (wParam & 0x0001) > 0;
				CHGEvent.mouseEvent.mouseButton = CHG_BUTTON_LEFT;

			}
			else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
			{
				CHGEvent.eventType = (msg == WM_RBUTTONDOWN) ? CHG_MOUSEBUTTONDOWN : CHG_MOUSEBUTTONUP;
				CHGEvent.mouseEvent.x = GET_X_LPARAM(lParam);
				CHGEvent.mouseEvent.y = GET_Y_LPARAM(lParam);
				CHGEvent.mouseEvent.relX = 0;
				CHGEvent.mouseEvent.relY = 0;
				CHGEvent.mouseEvent.pressed = (wParam & 0x0002) > 0;
				CHGEvent.mouseEvent.mouseButton = CHG_BUTTON_RIGHT;

			}
			else if (msg == WM_MOUSEMOVE)
			{
				CHGEvent.eventType = CHG_MOUSEMOTION;
				CHGEvent.mouseEvent.x = GET_X_LPARAM(lParam);
				CHGEvent.mouseEvent.y = GET_Y_LPARAM(lParam);
				CHGEvent.mouseEvent.relX = 0;
				CHGEvent.mouseEvent.relY = 0;
				CHGEvent.mouseEvent.pressed = (wParam & 0x0001)  > 0 || (wParam & 0x0002)  > 0;
			}
						
			if (g_pApp->m_pGame)
			{
				BaseGameLogic *pGame = g_pApp->m_pGame;

				for (GameViewList::reverse_iterator i = pGame->m_gameViews.rbegin(); i != pGame->m_gameViews.rend(); ++i)
				{
					if ((*i)->VOnMsgProc(CHGEvent))
					{
						break;				// WARNING! This breaks out of the for loop.
					}
				}
			}
			break;
		}
	}

	return 0;
}

void CthughaApp::OnUpdateGame(double fTime, float fElapsedTime)
{
	if (g_pApp->m_bQuitting)
	{
		m_bIsRunning = false;
	}

	if (g_pApp->m_pGame)
	{
        IEventManager::Get()->VUpdate(20); // allow event queue to process for up to 20 ms

		g_pApp->m_pGame->VOnUpdate(float(fTime), fElapsedTime);
	}
}

void CthughaApp::OnFrameRender(double fTime, float fElapsedTime)
{
	BaseGameLogic *pGame = g_pApp->m_pGame;

	for(GameViewList::iterator i=pGame->m_gameViews.begin(),
		end=pGame->m_gameViews.end(); i!=end; ++i)
	{
		(*i)->VOnRender(fTime, fElapsedTime);
	}

	g_pApp->m_pGame->VRenderDiagnostics();

	m_pRenderer->endFrame();

	m_iFrameNumber++;
}

int CthughaApp::OnUpdateGameAndRendering()
{
	double fTime;
	float fElapsedTime;

	getTimeValues(&fTime, &fElapsedTime);

	OnUpdateGame(fTime, fElapsedTime);

	if (m_Options.m_runFullSpeed || (m_lastUpdateTime - m_lastDraw) > CHG_SCREEN_REFRESH_RATE)
	{
		OnFrameRender(fTime, fElapsedTime);
		m_lastDraw = m_lastUpdateTime;
	}
	
	return 0;
}

//
// CthughaApp::ShutDownGame
//

void CthughaApp::OnShutDownGame()
{
	SAFE_DELETE(m_pGame);
	m_pRenderer.reset();
	m_pVoxelManager.reset();
	m_pAditionalShadowDataManager.reset();

	/* Delete our opengl context, destroy our window, and shutdown SDL */
    SDL_DestroyWindow(m_pWindowHandle);
    SDL_Quit();

	SAFE_DELETE(m_pEventManager);

	SAFE_DELETE(m_ResCache);
}

//
// CthughaApp::MainLoop
//

int CthughaApp::MainLoop()
{
	SDL_Event Event;

    while(m_bIsRunning) 
	{
        while(SDL_PollEvent(&Event)) 
		{
            OnProcessEvent(&Event);
        }

        OnUpdateGameAndRendering();
    }

	OnShutDownGame();

	return 0;
}

void CthughaApp::getTimeValues(double* fTime, float* fElapsedTime)
{
	clock_t utime = clock();

	clock_t uElapsedTime = utime - m_lastUpdateTime;

	*fTime = (double)utime / CLOCKS_PER_SEC;
	*fElapsedTime = (double)uElapsedTime / CLOCKS_PER_SEC;
    m_fElapsedTime = *fElapsedTime;

	m_lastUpdateTime = utime;
}

void CthughaApp::SetMouseMode(CHG_MouseMode mode)
{
	if (mode == CHG_ABSOLUTE_MODE)
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}		
	else if (mode == CHG_RELATIVE_MODE)
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}		
}

Uint32 CthughaApp::getTick()
{
	return clock();
}

//
// GameCodeApp::LoadStrings										
//
bool CthughaApp::LoadStrings(std::string language)
{
	std::string languageFile = "Strings\\";
	languageFile += language;
	languageFile += ".xml";

	TiXmlElement* pRoot = XmlResourceLoader::LoadAndReturnRootXmlElement(languageFile.c_str());
	if (!pRoot)
	{
		CHG_ERROR("Strings are missing.");
		return false;
	}

    // Loop through each child element and load the component
    for (TiXmlElement* pElem = pRoot->FirstChildElement(); pElem; pElem = pElem->NextSiblingElement())
    {
		const char *pKey=pElem->Attribute("id");
		const char *pText=pElem->Attribute("value");
		const char *pHotkey=pElem->Attribute("hotkey");
		if (pKey && pText) 
		{
			wchar_t wideKey[64];
			wchar_t wideText[1024];
			AnsiToWideCch(wideKey, pKey, 64);
			AnsiToWideCch(wideText, pText, 1024);
			m_textResource[std::wstring(wideKey)] = std::wstring(wideText);

			if (pHotkey)
			{
				m_hotkeys[std::wstring(wideKey)] = MapCharToKeycode(*pHotkey);
			}
		}
	}
	return true;
}

UINT CthughaApp::MapCharToKeycode(const char pHotKey)
{
	if (pHotKey >= '0' && pHotKey <= '9')
		return 0x30 + pHotKey - '0';

	if (pHotKey >= 'A' && pHotKey <= 'Z')
		return 0x41 + pHotKey - 'A';

	CHG_ASSERT(0 && "Platform specific hotkey is not defined");
	return 0;	
}

// GameCodeApp::GetString								

std::wstring CthughaApp::GetString(std::wstring sID)
{
	auto localizedString = m_textResource.find(sID);
	if(localizedString == m_textResource.end())
	{
		CHG_ASSERT(0 && "String not found!");
		return L"";
	}
	return localizedString->second;
}

void CthughaApp::RegisterEngineEvents(void)
{
    REGISTER_EVENT(EvtData_Environment_Loaded);
    REGISTER_EVENT(EvtData_New_Actor);
    REGISTER_EVENT(EvtData_Move_Actor);
    REGISTER_EVENT(EvtData_Destroy_Actor);
	REGISTER_EVENT(EvtData_Request_New_Actor);
}