#include "CthughaStd.h"


int CthughaAppEntry(int argc, char *argv[])
{
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;					

	_CrtSetDbgFlag(tmpDbgFlag);
	//_CrtSetBreakAlloc(63365); // breakpoint where create memory leaks

	//Initialize Logger
    Logger::Init("logging.xml");

	g_pApp->m_Options.Init("PlayerOptions.xml", argc, argv);

	int iWidth[] = { g_pApp->m_Options.m_ScreenSize.x };
	int iHeight[] = { g_pApp->m_Options.m_ScreenSize.y };

	// Perform application initialization
	if (!g_pApp->InitInstance(argc, argv, NULL, iWidth, iHeight, 1, g_pApp->m_Options.m_bWindowedMode))
	{
		return FALSE;
	}

    // Pass control to the sample framework for handling the message pump and 
    // dispatching render calls. The sample framework will call your FrameMove 
    // and FrameRender callback when there is idle time between handling window messages.
	g_pApp->MainLoop();

    //Destroy the logging system at the last possible moment
    Logger::Destroy();

	//_CrtDumpMemoryLeaks();

    return 0;	
}