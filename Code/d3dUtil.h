//=======================================================================================
// d3dUtil.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef D3DUTIL_H
#define D3DUTIL_H

//redefine new operator to track memory leaks
#if defined(DEBUG) || defined(_DEBUG)
	#ifndef DBG_NEW      
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )      
		//#define new DBG_NEW   
	#endif
#endif  
 
// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
#if defined(DEBUG) || defined(_DEBUG)
	#ifndef D3D_DEBUG_INFO
	#define D3D_DEBUG_INFO
	#endif
#endif

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h>
#endif

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx10d.lib" )
#else
#pragma comment( lib, "d3dx10.lib" )

#endif

#pragma comment( lib, "D3D10.lib" )
#pragma comment(lib, "dxgi.lib")


#include <d3dx10.h>
#include <dxerr.h>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


//*****************************************************************************
// Convenience macro for releasing COM objects.
//*****************************************************************************

#define ReleaseCOM(x) { if(x){ x->Release();x = nullptr; } }


//*****************************************************************************
// Convenience functions.
//*****************************************************************************

// Converts ARGB 32-bit color format to ABGR 32-bit color format.
D3DX10INLINE UINT ARGB2ABGR(UINT argb)
{
	BYTE A = (argb >> 24) & 0xff;
	BYTE R = (argb >> 16) & 0xff;
	BYTE G = (argb >>  8) & 0xff;
	BYTE B = (argb >>  0) & 0xff;

	return (A << 24) | (B << 16) | (G << 8) | (R << 0);
}

// Returns random float in [0, 1).
D3DX10INLINE float RandF()
{
	return (float)(rand()) / (float)RAND_MAX;
}

// Returns random float in [a, b).
D3DX10INLINE float RandF(float a, float b)
{
	return a + RandF()*(b-a);
}

// Returns random vector on the unit sphere.
D3DX10INLINE D3DXVECTOR3 RandUnitVec3()
{
	D3DXVECTOR3 v(RandF(), RandF(), RandF());
	D3DXVec3Normalize(&v, &v);
	return v;
}

// Returns random vector in the unit sphere.
D3DX10INLINE D3DXVECTOR3 RandUnitVecInside3()
{
	D3DXVECTOR3 v(RandF(), RandF(), RandF());
	return v;
}
 
template<typename T>
D3DX10INLINE T Min(const T& a, const T& b)
{
	return a < b ? a : b;
}

template<typename T>
D3DX10INLINE T Max(const T& a, const T& b)
{
	return a > b ? a : b;
}
 
template<typename T>
D3DX10INLINE T Lerp(const T& a, const T& b, float t)
{
	return a + (b-a)*t;
}

template<typename T>
D3DX10INLINE T Clamp(const T& x, const T& low, const T& high)
{
	return x < low ? low : (x > high ? high : x); 
}


//*****************************************************************************
// Constants
//*****************************************************************************

const float INFINITY = FLT_MAX;
const float PI       = 3.14159265358979323f;
const float MATH_EPS = 0.0001f;

const D3DXCOLOR WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR RED(1.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const D3DXCOLOR BLUE(0.0f, 0.0f, 1.0f, 1.0f);
const D3DXCOLOR YELLOW(1.0f, 1.0f, 0.0f, 1.0f);
const D3DXCOLOR CYAN(0.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR MAGENTA(1.0f, 0.0f, 1.0f, 1.0f);

const D3DXCOLOR BEACH_SAND(1.0f, 0.96f, 0.62f, 1.0f);
const D3DXCOLOR LIGHT_YELLOW_GREEN(0.48f, 0.77f, 0.46f, 1.0f);
const D3DXCOLOR DARK_YELLOW_GREEN(0.1f, 0.48f, 0.19f, 1.0f);
const D3DXCOLOR DARKBROWN(0.45f, 0.39f, 0.34f, 1.0f);


//*****************************************************************************
// Val's Math Class
//*****************************************************************************
static double SimplexNoise2D(double xin, double yin);

#endif // D3DUTIL_H