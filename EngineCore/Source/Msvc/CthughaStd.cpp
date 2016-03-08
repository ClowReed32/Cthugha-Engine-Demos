//========================================================================
// GameCodeStd.cpp : source file that includes just the standard includes
//
//========================================================================



// GameCodeStd.pch will be the pre-compiled header
#include "CthughaStd.h"


//
// Colors are discussed in Chapter 13, page 453
//
Color g_White( 1.0f, 1.0f, 1.0f, fOPAQUE );	
Color g_Black( 0.0f, 0.0f, 0.0f, fOPAQUE );
Color g_Cyan( 0.0f, 1.0f, 1.0f, fOPAQUE );	
Color g_Red( 1.0f, 0.0f, 0.0f, fOPAQUE );
Color g_Green( 0.0f, 1.0f, 0.0f, fOPAQUE );
Color g_Blue( 0.0f, 0.0f, 1.0f, fOPAQUE );
Color g_Yellow( 1.0f, 1.0f, 0.0f, fOPAQUE );	
Color g_Gray40( 0.4f, 0.4f, 0.4f, fOPAQUE );
Color g_Gray25( 0.25f, 0.25f, 0.25f, fOPAQUE );
Color g_Gray65( 0.65f, 0.65f, 0.65f, fOPAQUE );
Color g_Transparent (1.0f, 0.0f, 1.0f, fTRANSPARENT );

// Note! If you change these direction vectors around to suit yourself
// Make sure you change it so it retains a left handed
// coordinate system 
Vec3 g_Right(1.0f, 0.0f, 0.0f);
Vec3 g_Up(0.0f, 1.0f, 0.0f);
Vec3 g_Forward(0.0f, 0.0f, 1.0f);


Vec4 g_Up4(g_Up.x, g_Up.y, g_Up.z, 0.0f);
Vec4 g_Right4(g_Right.x, g_Right.y, g_Right.z, 0.0f);
Vec4 g_Forward4(g_Forward.x, g_Forward.y, g_Forward.z, 0.0f);

const float fOPAQUE = 1.0f;
const int iOPAQUE = 1; 
const float fTRANSPARENT = 0.0f;
const int iTRANSPARENT = 0;

const int MEGABYTE = 1024 * 1024;
const float SIXTY_HERTZ = 16.66f;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

size_t memread ( void* ptrDest, size_t size, size_t count, memoryStream* bufferOrig )
{
	if((bufferOrig->offset + size*count) >= bufferOrig->size)
	{
		long oldOffset = bufferOrig->offset;
		memcpy(ptrDest, bufferOrig->buffer, bufferOrig->size - bufferOrig->offset);
		bufferOrig->buffer = (char*)bufferOrig->buffer + bufferOrig->size - bufferOrig->offset;
		bufferOrig->offset = bufferOrig->size;

		return bufferOrig->size - oldOffset;
	}
	else
	{
		memcpy(ptrDest, bufferOrig->buffer, size*count);
		bufferOrig->buffer = (char*)bufferOrig->buffer + size*count;
		bufferOrig->offset += size*count;

		return size*count;
	}
}

int memseek ( memoryStream* bufferOrig, long int offset, UINT origin )
{
	switch(origin)
	{
	case MEM_SEEK_CUR:
		if((bufferOrig->offset + offset) >= (int)bufferOrig->size)
		{
			return 1;
		}
		else
		{
			bufferOrig->buffer += offset;
			bufferOrig->offset += offset;
		
			return 0;
		}

		break;

	case MEM_SEEK_SET:
		if(offset >= (int)bufferOrig->size)
		{
			return 1;
		}
		else
		{
			bufferOrig->buffer = bufferOrig->initialPtr + offset;
			bufferOrig->offset = offset;
		
			return 0;
		}

		break;

	case MEM_SEEK_END:
		if(bufferOrig->size - 1 + offset >= bufferOrig->size)
		{
			return 1;
		}
		else
		{
			bufferOrig->buffer = bufferOrig->initialPtr + bufferOrig->size - 1 + offset;
			bufferOrig->offset = bufferOrig->size - 1 + offset;
		
			return 0;
		}

		break;

	default:
		return 0;

	}
}

float GaussianDistribution(float x, float rho)
{
	float g = 1.0f / sqrtf(2.0f * CHG_PI * rho * rho);
	g *= expf(-(x * x) / (2 * rho * rho));

	return g;
}

// [rez] Uncomment this #define to dump every call to new to the output window
//#define MEM_LOG
#if defined(_DEBUG) && defined(MEM_LOG)

void* operator new(size_t size, int memType, const char* filename, int lineNum)
{
    // We have to do this old-school since we're not allowed to dynamically allocate memory here.
    char buffer[2048];
    int index = 0;
    index += strlen(ultoa(size, buffer, 10));
    strcpy(buffer+index, " -> ");
    index += 4;
    strcpy(buffer+index, filename);
    index += strlen(filename);
    buffer[index] = ':';
    ++index;
    index += strlen(itoa(lineNum, buffer+index, 10));
    buffer[index] = '\n';
    ++index;
    buffer[index] = '\0';
    ++index;
    OutputDebugStringA(buffer);

    return _malloc_dbg(size, memType, filename, lineNum);
}

void operator delete(void* pMemory)
{
    _free_dbg(pMemory, 1);
}

void* operator new[](size_t size, int memType, const char* filename, int lineNum)
{
    // We have to do this old-school since we're not allowed to dynamically allocate memory here.
    char buffer[2048];
    int index = 0;
    index += strlen(ultoa(size, buffer, 10));
    strcpy(buffer+index, " -> ");
    index += 4;
    strcpy(buffer+index, filename);
    index += strlen(filename);
    buffer[index] = ':';
    ++index;
    index += strlen(itoa(lineNum, buffer+index, 10));
    buffer[index] = '\n';
    ++index;
    buffer[index] = '\0';
    ++index;
    OutputDebugStringA(buffer);

    return _malloc_dbg(size, 1, filename, lineNum);
}

void operator delete[](void* pMemory)
{
    _free_dbg(pMemory, 1);
}

#endif
