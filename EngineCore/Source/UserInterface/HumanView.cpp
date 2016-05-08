//========================================================================
// HumanView.cpp - Implements the class HumanView, which provides a Human interface into the game
//
//========================================================================

#include "CthughaStd.h"

#include "HumanView.h"

//#include "../Actors/AudioComponent.h"
#include "../Entities/RenderComponentInterface.h"
//#include "../Audio/DirectSoundAudio.h"
//#include "../Audio/SoundProcess.h"
#include "../EventManager/Events.h"
#include "../EventManager/EventManagerImpl.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/Scene.h"
#include "../MainLoop/Process.h"
#include "../Utilities/String.h"
//#include "../LUAScripting/LuaStateManager.h"

const unsigned int SCREEN_REFRESH_RATE(1000/60);
const GameViewId gc_InvalidGameViewId = 0xffffffff;

//
// HumanView::HumanView - Chapter 10, page 272
//
HumanView::HumanView(shared_ptr<Renderer> renderer, bool bScene, bool bGUI)
{
	m_pProcessManager = CHG_NEW ProcessManager;
	m_runFullSpeed = true;

	m_PointerRadius = 1;	// we assume we are on a mouse enabled machine - if this were a tablet we should detect it here.
	m_ViewId = g_pApp->m_pGame->GetNewGameViewID();
	m_bPause = false;
	m_bActive = false;

	// Added post press for move, new, and destroy actor events and others
	RegisterAllDelegates();
	m_BaseGameState = BGS_Initializing;		// what is the current game state

    if (renderer)
    {
        if (bScene)
        {        
            // Moved to the HumanView class post press
            m_pScene.reset(CHG_NEW ScreenElementScene(renderer));

            Frustum frustum;
            frustum.Init(CHG_PI / 4.0f, 1.0f, 0.1f, 100.0f);
            m_pCamera.reset(CHG_NEW CameraNode(&Mat4x4::g_Identity, frustum));
            CHG_ASSERT(m_pScene && m_pCamera && _T("Out of memory"));

            m_pScene->VAddChild(INVALID_ENTITY_ID, m_pCamera);
            m_pScene->SetCamera(m_pCamera);
        }

		if (bGUI)
		{
			// Init GUI scene
			m_pGUI.reset(CHG_NEW ScreenElementUIScene(renderer, m_ViewId));

			VPushElement(m_pGUI);
		}		
	}
}


HumanView::~HumanView()
{
	//RemoveAllDelegates was added post press to handle move, new, and destroy actor events.
	RemoveAllDelegates();

	while (!m_ScreenElements.empty())
	{
		m_ScreenElements.pop_front();		
	}

	SAFE_DELETE(m_pProcessManager);
}

bool HumanView::LoadGame(TiXmlElement* pLevelData)
{
    // call the delegate method
    return VLoadGameDelegate(pLevelData);
}

//
// HumanView::VOnRender							
//
void HumanView::VOnRender(double fTime, float fElapsedTime )
{	
	m_currTick = g_pApp->getTick();
	if (m_currTick == m_lastDraw || !m_bActive)
		return;

	// It is time to draw ?
	if( m_runFullSpeed || ( (m_currTick - m_lastDraw) > SCREEN_REFRESH_RATE) )
	{
		//PreRender
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        g_pApp->m_pRenderer->changeToMainFramebuffer();
		g_pApp->m_pRenderer->clear(true, true, false, ClearColor, 1.0f);
		g_pApp->m_pRenderer->reset();

		m_ScreenElements.sort(SortBy_SharedPtr_Content<IScreenElement>());

		for(ScreenElementList::iterator i=m_ScreenElements.begin(); i!=m_ScreenElements.end(); ++i)
		{
			if ( (*i)->VIsVisible() )
			{
				(*i)->VOnRender(fTime, fElapsedTime);
			}
		}

        VRenderText();

		// Let the console render.
		m_Console.Render();

		// record the last successful paint
		m_lastDraw = m_currTick;	
    }
}


//
// HumanView::VOnRestore						
//
bool HumanView::VOnRestore()
{
	for(ScreenElementList::iterator i=m_ScreenElements.begin(); i!=m_ScreenElements.end(); ++i)
	{
		if(!(*i)->VOnRestore())
			return false;
	}

	return true;
}


//
// HumanView::VOnLostDevice						
//
//    Recursively calls VOnLostDevice for everything attached to the HumanView. 

bool HumanView::VOnLostDevice() 
{
	for(ScreenElementList::iterator i=m_ScreenElements.begin(); i!=m_ScreenElements.end(); ++i)
	{
		if(!(*i)->VOnLostDevice())
			return false;
	}

	return true;
}

void HumanView::TogglePause(bool active)
{
	m_bPause = active;
}

//
// HumanView::VOnMsgProc						
//
bool HumanView::VOnMsgProc( CHG_Event msg )
{
	if (!m_bActive)
		return 0;

	bool bMouseGUIActive = false;

	// Iterate through the screen layers first
	// In reverse order since we'll send input messages to the 
	// screen on top
	for(ScreenElementList::reverse_iterator i=m_ScreenElements.rbegin(); i!=m_ScreenElements.rend(); ++i)
	{
		if ( (*i)->VIsVisible() )
		{
			if ( (*i)->VOnMsgProc( msg ) )
			{
				bMouseGUIActive = true;
			}
		}
	}

	bool result = 0;
	
	if (!m_bPause)
	{	
		switch (msg.eventType) 
		{
			case CHG_KEYDOWN:
				if (m_Console.IsActive())
				{
					m_Console.HandleKeyboardInput( msg.keyboardEvent.asciiCode, true );
				}
				else if(('º' == msg.keyboardEvent.asciiCode) || ('`' == msg.keyboardEvent.asciiCode)) //See if it was the console key.
				{
					//m_Console.SetActive(true);
				}
				else if (m_KeyboardHandler)
				{
					result = m_KeyboardHandler->VOnKeyDown(msg.keyboardEvent.asciiCode);
				}
				break;
	
			case CHG_KEYUP:
				if (m_Console.IsActive())
				{
					// Let the console eat this.
				}
				else if (m_KeyboardHandler)
					result = m_KeyboardHandler->VOnKeyUp(msg.keyboardEvent.asciiCode);
				break;

			case CHG_MOUSEMOTION:
				if (m_PointerHandler && !bMouseGUIActive)
					result = m_PointerHandler->VOnPointerMove(msg.mouseEvent.x, msg.mouseEvent.y, msg.mouseEvent.relX, msg.mouseEvent.relY, 1);
				break;

			case CHG_MOUSEBUTTONDOWN:
				if (m_PointerHandler && !bMouseGUIActive)
				{
					//SetCapture(msg.m_hWnd);
					result = m_PointerHandler->VOnPointerButtonDown(msg.mouseEvent.x, msg.mouseEvent.y, msg.mouseEvent.relX, msg.mouseEvent.relY, 1, msg.mouseEvent.mouseButton);
				}	
				break;

			case CHG_MOUSEBUTTONUP:
				if (m_PointerHandler && !bMouseGUIActive)
				{
					//SetCapture(NULL);
					result = m_PointerHandler->VOnPointerButtonUp(msg.mouseEvent.x, msg.mouseEvent.y, msg.mouseEvent.relX, msg.mouseEvent.relY, 1, msg.mouseEvent.mouseButton);
				}
				break;

			default:
				return 0;
		}
	}

	return 0;
}

//
// HumanView::VOnRestore						
//
void HumanView::VOnUpdate(const int deltaMilliseconds)
{
	if (!m_bActive)
		return;

	m_pProcessManager->UpdateProcesses(deltaMilliseconds);

	m_Console.Update(deltaMilliseconds);

	// This section of code was added post-press. It runs through the screenlist
	// and calls VOnUpdate. Some screen elements need to update every frame, one 
	// example of this is a 3D scene attached to the human view.
	//
	for(ScreenElementList::iterator i=m_ScreenElements.begin(); i!=m_ScreenElements.end(); ++i)
	{
		(*i)->VOnUpdate(deltaMilliseconds);
	}
}

//
// HumanView::VPushElement						
//
void HumanView::VPushElement(shared_ptr<IScreenElement> pElement)
{
	m_ScreenElements.push_front(pElement);
}

//
// HumanView::VPopElement						
//
//   
//
void HumanView::VRemoveElement(shared_ptr<IScreenElement> pElement)
{
	m_ScreenElements.remove(pElement);
}

//
// HumanView::VSetCameraOffset					
//
//   Sets a camera offset, useful for making a 1st person or 3rd person game
//
void HumanView::VSetCameraOffset(const Vec4 & camOffset )
{
	CHG_ASSERT(m_pCamera);
	if (m_pCamera)
	{
		m_pCamera->SetCameraOffset( camOffset );
	}
}



//
// HumanView::RegisterAllDelegates, HumanView::RemoveAllDelegates		
//
//   Aggregates calls to manage event listeners for the HumanView class.
//
void HumanView::RegisterAllDelegates(void)
{
    IEventManager* pGlobalEventManager = IEventManager::Get();
}

void HumanView::RemoveAllDelegates(void)
{
    IEventManager* pGlobalEventManager = IEventManager::Get();
}

//==============================================================
// HumanView::Console - defines the a class to manage a console to type in commands
//
//   Not described in the book. 
//
const int kCursorBlinkTimeMS = 500;

char const * const kExitString = "exit";
char const * const kClearString = "clear";

HumanView::Console::Console()
: m_bActive( false )
, m_bExecuteStringOnUpdate( false )
{
	m_ConsoleInputSize = 48;

	m_CurrentInputString = std::string("");

	m_InputColor = Color( 1.0f, 1.0f, 1.0f, 1.0f );
	m_OutputColor = Color( 1.0f, 1.0f, 1.0f, 1.0f );

	m_CursorBlinkTimer = kCursorBlinkTimeMS;
	m_bCursorOn = true;

	m_consoleBlendState = g_pApp->m_pRenderer->addBlendState(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
}

HumanView::Console::~Console()
{
}

void HumanView::Console::AddDisplayText( const std::string & newText )
{
	m_CurrentOutputString += newText;
	m_CurrentOutputString += '\n';
}

void HumanView::Console::SetDisplayText( const std::string & newText )
{
	m_CurrentOutputString = newText;
}

void HumanView::Console::Update( const int deltaMilliseconds )
{
	//Don't do anything if not active.
	if ( !m_bActive )
	{
		return;	//Bail!
	}

	//Do we have a string to execute?
	if (m_bExecuteStringOnUpdate)
	{
		const std::string renderedInputString = std::string(">") + m_CurrentInputString;
		if ( 0 == m_CurrentInputString.compare( kExitString ) )
		{
			SetActive( false );
			m_CurrentInputString.clear();
		}
		else if ( 0 == m_CurrentInputString.compare( kClearString ) )
		{
			m_CurrentOutputString = renderedInputString;	//clear
			m_CurrentInputString.clear();
		}
		else
		{
			//Put the input string into the output window.
			AddDisplayText( renderedInputString );

			const int retVal = true;

			//Attempt to execute the current input string...
            /*if (!m_CurrentInputString.empty())
			    LuaStateManager::Get()->VExecuteString(m_CurrentInputString.c_str());*/

			//Clear the input string
			m_CurrentInputString.clear();
		}

		//We're accepting input again.
		m_bExecuteStringOnUpdate = false;
        SetActive(false);
	}

	//Update the cursor blink timer...
	m_CursorBlinkTimer -= deltaMilliseconds;

	if ( m_CursorBlinkTimer < 0 )
	{
		m_CursorBlinkTimer = 0;

		m_bCursorOn = !m_bCursorOn;

		m_CursorBlinkTimer = kCursorBlinkTimeMS;
	}
}

void HumanView::Console::Render( )
{
	//Don't do anything if not active.
	if ( !m_bActive )
	{
		return;	//Bail!
	}

	g_pApp->m_pRenderer->setup2DMode(0.0f, g_pApp->GetScreenSize().x, 0.0f, g_pApp->GetScreenSize().y);

	Vec2 rect[] = 
	{
		Vec2(0.0f, g_pApp->GetScreenSize().y),
		Vec2(0.0f, g_pApp->GetScreenSize().y - 268.0f),
		Vec2(g_pApp->GetScreenSize().x, g_pApp->GetScreenSize().y),
		Vec2(g_pApp->GetScreenSize().x, g_pApp->GetScreenSize().y - 268.0f),
	};

	g_pApp->m_pRenderer->drawPlain(PRIM_TRIANGLE_STRIP, rect, elementsOf(rect), m_consoleBlendState, SS_NONE, &Color(0.1f, 0.1f, 0.1f, 0.4f));

	const Color white( 1.0f, 1.0f, 1.0f, 1.0f );
	const Color black( 0.0f, 0.0f, 0.0f, 1.0f );
	Vec4 inputTextRect, outputTextRect, shadowRect;

	//Display the console text at screen top, below the other text displayed.
	const std::string finalInputString = std::string( ">" ) + m_CurrentInputString + ( m_bCursorOn ? ' ' : '_' );
	inputTextRect.x = 10;
	inputTextRect.y = g_pApp->GetScreenSize().y - 268.0f;
	inputTextRect.z = g_pApp->GetScreenSize().x - 10;
	inputTextRect.w = g_pApp->GetScreenSize().y - 10;

	const int kNumWideChars = 4096;
	char wideBuffer[ kNumWideChars ];

	memcpy(wideBuffer, finalInputString.c_str(), finalInputString.size() < kNumWideChars ? finalInputString.size() : kNumWideChars);
	wideBuffer[finalInputString.size()] = 0;

	//Draw with shadow first.
	shadowRect = inputTextRect;
	++shadowRect.x;
	++shadowRect.y;
	g_pApp->m_pTextHelper->SetColor( black );
	g_pApp->m_pTextHelper->DrawTextLine( shadowRect, F_LEFT | F_TOP, wideBuffer );

	//Now bright text.
	g_pApp->m_pTextHelper->SetColor( white );
	g_pApp->m_pTextHelper->DrawTextLine( inputTextRect, F_LEFT | F_TOP, wideBuffer );

	//Now display the output text just below the input text.
	outputTextRect.x = inputTextRect.x + 15;
	outputTextRect.y = inputTextRect.y + 15;
	outputTextRect.z = g_pApp->GetScreenSize().x - 10;
	outputTextRect.w = g_pApp->GetScreenSize().y - 10;

	memcpy(wideBuffer, m_CurrentOutputString.c_str(), m_CurrentOutputString.size() < kNumWideChars ? m_CurrentOutputString.size() : kNumWideChars);
	wideBuffer[m_CurrentOutputString.size()] = 0;

	//Draw with shadow first.
	shadowRect = outputTextRect;
	++shadowRect.x;
	++shadowRect.y;
	g_pApp->m_pTextHelper->SetColor( black );
	g_pApp->m_pTextHelper->DrawTextLine( shadowRect, F_LEFT | F_TOP, wideBuffer );

	//Now bright text.
	g_pApp->m_pTextHelper->SetColor( white );
	g_pApp->m_pTextHelper->DrawTextLine( outputTextRect, F_LEFT | F_TOP, wideBuffer );
}

void HumanView::Console::HandleKeyboardInput( const BYTE keyVal, const bool bKeyDown )
{
	if ( true == m_bExecuteStringOnUpdate )
	{
		//We've already got a string to execute; ignore.
		return;
	}

	//See if it's a valid key press that we care about.
	switch( keyVal )
	{
	    case '\b':
		{
			const size_t strSize = m_CurrentInputString.size();
			if ( strSize > 0 )
			{
				m_CurrentInputString.erase((strSize - 1), 1);
			}
            break;
		}

    	case '\r':
		{
			m_bExecuteStringOnUpdate = true;	//Execute this string.
            break;
		}

    	default:
        {
		    m_CurrentInputString += (char)keyVal;
		    break;
        }
	}
}
