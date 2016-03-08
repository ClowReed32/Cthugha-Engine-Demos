#pragma once
//========================================================================
// Initialization.h : Defines utility functions for game initialization
//

extern bool CheckStorage(const DWORDLONG diskSpaceNeeded);
extern DWORD ReadCPUSpeed();
extern bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded);
extern bool IsOnlyInstance(LPCTSTR gameTitle);
extern const TCHAR *GetSaveGameDirectory(HWND hWnd, const TCHAR *gameAppDirectory);
extern bool CheckForJoystick(HWND hWnd);

//extern DWORD GetFreeVRAM();		// deprecated!!!!

struct GameOptions
{
	// Level option
	std::string m_Level;

	// Rendering options
	std::string m_Renderer;
	bool m_runFullSpeed;
	Point m_ScreenSize;
	bool m_bWindowedMode;

	// Sound options
	float m_soundEffectsVolume;			
	float m_musicVolume;				

	// Multiplayer options
	int m_expectedPlayers;				
	int m_listenPort;					
	std::string m_gameHost;				
	int m_numAIs;
	int m_maxAIs;
	int m_maxPlayers;

    // resource cache options
    bool m_useDevelopmentDirectories;

	// TiXmlElement - look at this to find other options added by the developer
	TiXmlDocument *m_pDoc;

	GameOptions();
	~GameOptions() { SAFE_DELETE(m_pDoc); }

	void Init(const char* xmlFilePath, int argc, char *argv[]);
};
