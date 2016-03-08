#pragma once
//========================================================================
// CthughaStd.h : include file for standard system include files,
//            or project specific include files that are used frequently, but
//            are changed infrequently
//
//========================================================================

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:

#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

#include <crtdbg.h>

#define _VS2005_
#if _MSC_VER < 1400
	#undef _VS2005_
#endif

 // C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory>
#include <tchar.h>

#include <mmsystem.h>

#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <sstream>
#include <queue>
#include <map>
#include <ctime>

using std::shared_ptr;
using std::weak_ptr;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;

class CHG_noncopyable 
{  
     private:  
      CHG_noncopyable(const CHG_noncopyable& x);  
      CHG_noncopyable& operator=(const CHG_noncopyable& x);  
     public:  
      CHG_noncopyable() {}; // Default constructor  
};  

#if defined(_DEBUG)
#	define CHG_NEW new(_NORMAL_BLOCK,__FILE__, __LINE__)
#else
#	define CHG_NEW new
#endif

// Graphics Includes
#include <GL\glew.h> 
#include <SDL\SDL.h>
#include <SDL\SDL_syswm.h>
#include <SDL_ttf.h>

#include <d3d11_1.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>

#include <tinyxml.h>

// fast delegate stuff
#include "../3rdParty/FastDelegate/FastDelegate.h"
using fastdelegate::MakeDelegate;

#pragma warning( disable : 4996 ) // 'function' declared deprecated - gets rid of all those 2005 warnings....


// GameCode #includes
#include "..\Debugging\Logger.h"  // this should be the first of the gcc includes since it defines GCC_ASSERT()
#include "..\Utilities\types.h"
#include "..\Utilities\templates.h"
#include "..\Utilities\Array.h"
#include "..\Utilities\Queue.h"
#include "..\Graphics\geometry.h"
#include "..\Graphics\Image.h"

typedef Vec4 Color;

extern Color g_White;
extern Color g_Black;
extern Color g_Cyan;
extern Color g_Red;
extern Color g_Green;
extern Color g_Blue;
extern Color g_Yellow;
extern Color g_Gray40;
extern Color g_Gray25;
extern Color g_Gray65;
extern Color g_Transparent;

extern Vec3 g_Up;
extern Vec3 g_Right;
extern Vec3 g_Forward;

extern Vec4 g_Up4;
extern Vec4 g_Right4;
extern Vec4 g_Forward4;

//Event message declarations
#define MAX_EVENT_TYPES 14

#define MAX_OCTREE_NODES 50

//Specific map for SDL Event
enum CHG_EventType
{
	CHG_WINDOWEVENT = SDL_WINDOWEVENT, 
	CHG_KEYDOWN = SDL_KEYDOWN, 
	CHG_KEYUP = SDL_KEYUP, 
	CHG_MOUSEMOTION = SDL_MOUSEMOTION, 
	CHG_MOUSEBUTTONDOWN = SDL_MOUSEBUTTONDOWN, 
	CHG_MOUSEBUTTONUP = SDL_MOUSEBUTTONUP, 
	CHG_JOYAXISMOTION = SDL_JOYAXISMOTION, 
	CHG_JOYBALLMOTION = SDL_JOYBALLMOTION, 
	CHG_JOYHATMOTION = SDL_JOYHATMOTION, 
	CHG_JOYBUTTONDOWN = SDL_JOYBUTTONDOWN, 
	CHG_JOYBUTTONUP = SDL_JOYBUTTONUP, 
	CHG_QUIT = SDL_QUIT, 
	CHG_SYSWMEVENT = SDL_SYSWMEVENT, 
	CHG_WINDOWEVENT_RESIZED = SDL_WINDOWEVENT_RESIZED
};

enum CHG_KeyVal
{

};

// Platform independent key codes
#define KEY_0 int('0')
#define KEY_1 int('1')
#define KEY_2 int('2')
#define KEY_3 int('3')
#define KEY_4 int('4')
#define KEY_5 int('5')
#define KEY_6 int('6')
#define KEY_7 int('7')
#define KEY_8 int('8')
#define KEY_9 int('9')

#define KEY_LEFT      VK_LEFT
#define KEY_RIGHT     VK_RIGHT
#define KEY_UP        VK_UP
#define KEY_DOWN      VK_DOWN
#define KEY_CTRL      VK_CONTROL
#define KEY_SHIFT     VK_SHIFT
#define KEY_ENTER     VK_RETURN
#define KEY_SPACE     VK_SPACE
#define KEY_TAB       VK_TAB
#define KEY_ESCAPE    VK_ESCAPE
#define KEY_BACKSPACE VK_BACK
#define KEY_HOME      VK_HOME
#define KEY_END       VK_END
#define KEY_INSERT    VK_INSERT
#define KEY_DELETE    VK_DELETE

#define KEY_F1  VK_F1
#define KEY_F2  VK_F2
#define KEY_F3  VK_F3
#define KEY_F4  VK_F4
#define KEY_F5  VK_F5
#define KEY_F6  VK_F6
#define KEY_F7  VK_F7
#define KEY_F8  VK_F8
#define KEY_F9  VK_F9
#define KEY_F10 VK_F10
#define KEY_F11 VK_F11
#define KEY_F12 VK_F12

#define KEY_NUMPAD0 VK_NUMPAD0
#define KEY_NUMPAD1 VK_NUMPAD1
#define KEY_NUMPAD2 VK_NUMPAD2
#define KEY_NUMPAD3 VK_NUMPAD3
#define KEY_NUMPAD4 VK_NUMPAD4
#define KEY_NUMPAD5 VK_NUMPAD5
#define KEY_NUMPAD6 VK_NUMPAD6
#define KEY_NUMPAD7 VK_NUMPAD7
#define KEY_NUMPAD8 VK_NUMPAD8
#define KEY_NUMPAD9 VK_NUMPAD9

#define KEY_ADD        VK_ADD
#define KEY_SUBTRACT   VK_SUBTRACT
#define KEY_MULTIPLY   VK_MULTIPLY
#define KEY_DIVIDE     VK_DIVIDE
#define KEY_SEPARATOR  VK_SEPARATOR
#define KEY_DECIMAL    VK_DECIMAL
#define KEY_PAUSE      VK_PAUSE

#define KEY_A int('A')
#define KEY_B int('B')
#define KEY_C int('C')
#define KEY_D int('D')
#define KEY_E int('E')
#define KEY_F int('F')
#define KEY_G int('G')
#define KEY_H int('H')
#define KEY_I int('I')
#define KEY_J int('J')
#define KEY_K int('K')
#define KEY_L int('L')
#define KEY_M int('M')
#define KEY_N int('N')
#define KEY_O int('O')
#define KEY_P int('P')
#define KEY_Q int('Q')
#define KEY_R int('R')
#define KEY_S int('S')
#define KEY_T int('T')
#define KEY_U int('U')
#define KEY_V int('V')
#define KEY_W int('W')
#define KEY_X int('X')
#define KEY_Y int('Y')
#define KEY_Z int('Z')

enum CHG_MouseMode{CHG_ABSOLUTE_MODE, CHG_RELATIVE_MODE};

enum CHG_MouseButton{CHG_BUTTON_LEFT, CHG_BUTTON_RIGHT, CHG_BUTTON_MIDDLE};

extern int EventTypesMap[MAX_EVENT_TYPES];
extern int MouseButtonMap[3];

struct CHG_MouseEvent
{
	int x, y, relX, relY;
	CHG_MouseButton mouseButton;
	bool pressed;
};

struct CHG_KeyboardEvent
{
	BYTE asciiCode;
};

struct CHG_Event
{
	CHG_MouseEvent mouseEvent;
	CHG_KeyboardEvent keyboardEvent;
	CHG_EventType eventType;
};

#include "..\CthughaEngine\interfaces.h"


// Useful #defines

extern const float fOPAQUE;
extern const int iOPAQUE;
extern const float fTRANSPARENT;
extern const int iTRANSPARENT;

extern const int MEGABYTE;
extern const float SIXTY_HERTZ;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

#if !defined(SAFE_DELETE)
	#define SAFE_DELETE(x) if(x) delete x; x=NULL;
#endif

#if !defined(SAFE_DELETE_ARRAY)
	#define SAFE_DELETE_ARRAY(x) if (x) delete [] x; x=NULL; 
#endif

#if !defined(SAFE_RELEASE)
	#define SAFE_RELEASE(x) if(x) x->Release(); x=NULL;
#endif

#ifdef UNICODE
	#define _tcssprintf wsprintf
	#define tcsplitpath _wsplitpath
#else
	#define _tcssprintf sprintf
	#define tcsplitpath _splitpath
#endif

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Warning Msg: "

#define MCHAR2(a, b) (a | (b << 8))
#define MCHAR4(a, b, c, d) (a | (b << 8) | (c << 16) | (d << 24))

template <typename T, size_t N> char (&ArraySizeHelper(T (&arr)[N]))[N];
#define elementsOf(arr) (sizeof(ArraySizeHelper(arr)))

inline bool isPowerOf2(const int x)
{
	return (x & (x - 1)) == 0;
}

inline int pow2roundup(int x)
{
    if (x < 0)
        return 0;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}

float GaussianDistribution(float x, float rho);

#include "..\CthughaEngine\CthughaApp.h"

extern int CthughaAppEntry(int argc, char *argv[]);


//Auxiliar function for read stream memory as file stream

#define MEM_SEEK_SET	0
#define MEM_SEEK_CUR	1
#define MEM_SEEK_END	2

struct memoryStream
{
	char* buffer;
	char* initialPtr;
	long offset;
	size_t size;
};

size_t memread ( void* ptrDest, size_t size, size_t count, memoryStream* bufferOrig );
int memseek ( memoryStream* bufferOrig, long int offset, UINT origin );