//===========================================================================
// Summary:
//		KernelDef.h
//      常用宏的定义 
// Usage:
//	    Null
// Remarks:
//		Null
// Date:
//		2011-08-30
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================

#ifndef	__KERNEL_COMMONLIBS_KERNELBASE_KERNELDEF_H__
#define	__KERNEL_COMMONLIBS_KERNELBASE_KERNELDEF_H__

#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <cassert>
#include "KernelBaseType.h"
#include <stdlib.h>

#ifdef _LINUX
	#ifndef NOT_SUPPORT_FVISIBILITY
		#define KERNEL_API					__attribute__((visibility("default")))
		#define KERNELSTDMETHODCALLTYPE		__attribute__((stdcall))
		#define KERNELCALLBACK				__attribute__((stdcall))
	#else
		#define KERNEL_API					
		#define KERNELSTDMETHODCALLTYPE	
		#define KERNELCALLBACK				
	#endif
#else
	/*#ifdef KERNELLIB_EXPORTS
		#define KERNEL_API __declspec(dllexport)
	#else
		#define KERNEL_API __declspec(dllimport)
	#endif*/

    #define KERNEL_API					
	#define KERNELSTDMETHODCALLTYPE		__stdcall
	#define KERNELCALLBACK				__stdcall
#endif

#define KERNELAPI				extern "C" KERNEL_API bool
#define KERNELAPI_(type)		extern "C" KERNEL_API type

template <typename T>
inline T const& dk_max (T const& a, T const& b)
{
    return  a < b? b: a;
}

template <typename T>
inline T const& dk_min (T const& a, T const& b)
{
    return  a < b? a: b;
}

template <typename T>
inline DK_VOID dk_swap (T & a, T & b)
{
    T t = a;
    a = b;
    b = t;
}

// TODO(zhaigh): 根据处理器可以优化
inline DK_UINT16 GetU16BE(const DK_UINT8 *pSrc)
{
    return ((DK_UINT16)pSrc[0] << 8) 
          + (DK_UINT16)pSrc[1];
}

inline DK_UINT16 GetU16LE(const DK_UINT8 *pSrc)
{
    return ((DK_UINT16)pSrc[1] << 8) 
          + (DK_UINT16)pSrc[0];
}

inline DK_UINT32 GetU32BE(const DK_UINT8 *pSrc)
{
    return ((DK_UINT32)pSrc[0] << 24) 
         + ((DK_UINT32)pSrc[1] << 16) 
         + ((DK_UINT32)pSrc[2] << 8) 
          + (DK_UINT32)pSrc[3];
}

inline DK_UINT32 GetU32LE(const DK_UINT8 *pSrc)
{
    return ((DK_UINT32)pSrc[3] << 24) 
         + ((DK_UINT32)pSrc[2] << 16) 
         + ((DK_UINT32)pSrc[1] << 8) 
          + (DK_UINT32)pSrc[0];
}

inline DK_VOID PutU16BE(DK_UINT16 src, DK_UINT8 *pDest)
{
    pDest[0] = (DK_UINT8)(src >> 8);
    pDest[1] = (DK_UINT8)src;
}

inline DK_VOID PutU16LE(DK_UINT16 src, DK_UINT8 *pDest)
{
    pDest[1] = (DK_UINT8)(src >> 8);
    pDest[0] = (DK_UINT8)src;
}

inline DK_VOID PutU32BE(DK_UINT32 src, DK_UINT8 *pDest)
{
    pDest[0] = (DK_UINT8)(src >> 24);
    pDest[1] = (DK_UINT8)(src >> 16);
    pDest[2] = (DK_UINT8)(src >>  8);
    pDest[3] = (DK_UINT8)src;
}

inline DK_VOID PutU32LE(DK_UINT32 src, DK_UINT8 *pDest)
{
    pDest[3] = (DK_UINT8)(src >> 24);
    pDest[2] = (DK_UINT8)(src >> 16);
    pDest[1] = (DK_UINT8)(src >>  8);
    pDest[0] = (DK_UINT8)src;
}

#ifdef _DEBUG

#ifdef _WIN32
// redefine, add type conversion to avoid warning C4365, signed/unsigned mismatch
#define DK_ASSERT(_Expression) (void)( (!!(_Expression)) || (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), (unsigned int)__LINE__), 0) )
#endif

#ifdef _LINUX 
DK_VOID DK_EnableAssert(bool enable);
DK_UINT DK_AssertFailedLine(const DK_CHAR* szFileName, DK_UINT nLine);
#define DK_ASSERT(f) \
	do \
	{ \
		if (!(f) && DK_AssertFailedLine(__FILE__, __LINE__)) \
        { \
        } \
	} while (0)
#endif

#else
#define DK_ASSERT(_Expression)     ((void)0)
#endif // _DEBUG

template <int N>
struct dk_array_size_struct
{
    char c[N];
};
template <class T, int N>
const dk_array_size_struct<N> & dk_static_array_size(T (&)[N]);

#define DK_DIM(arr) sizeof(dk_static_array_size(arr).c)
#define DK_ARRAY_END(arr) &(arr)[DK_DIM(arr)]

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4668)
#include <windows.h>
#pragma warning(pop)
#define unlink _unlink
#define stat   _stat
#define DK_MAX_PATH MAX_PATH
#endif

#ifdef _LINUX
#define DK_MAX_PATH PATH_MAX
#endif

const DK_SIZE_T DK_MAX_URI = 2048;

#ifdef _WIN32
#define DK_MALLOC(s)       _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
inline DK_VOID DK_FREE(DK_VOID* p)
{
    _free_dbg(p, _NORMAL_BLOCK);
}

#else
inline DK_VOID* DK_MALLOC(DK_SIZE_T size)
{
    return malloc(size);
}
inline DK_VOID DK_FREE(DK_VOID* p)
{
    free(p);
}
#endif

#define DK_REALLOC realloc
#define DK_MALLOC_OBJ_N(t, n) ((t*)DK_MALLOC(sizeof(t) * (n)))
#define DK_MALLOC_OBJ(t) ((t*)DK_MALLOC(sizeof(t)))

#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#define DK_MALLOCZ(size) memset(DK_MALLOC(size), 0, size)
#else
inline void* DkMallocZ(DK_SIZE_T size)
{
    void* p = DK_MALLOC(size);
    if (DK_NULL != p)
    {
        memset(p, 0, size);
    }
    return p;
}
#define DK_MALLOCZ DkMallocZ
#endif

#define DK_MALLOCZ_OBJ_N(t, n) ((t*)DK_MALLOCZ(sizeof(t) * (n)))
#define DK_MALLOCZ_OBJ(t) ((t*)DK_MALLOCZ(sizeof(t)))

template <typename Pointer>
inline DK_VOID SafeReleasePointer(Pointer& ptr)
{
	if (ptr != DK_NULL)
	{
		ptr->Release();
 		ptr = DK_NULL;
	}
}

template <typename T>
inline DK_VOID SafeDeletePointer(T* & pointer)
{
	if (pointer)
	{
		delete pointer;
		pointer = DK_NULL;
	}
}


template <typename Pointer>
inline DK_VOID SafeDeleteArrayPointer(Pointer& pointer)
{
	if (pointer)
	{
		delete[] pointer;
		pointer = DK_NULL;
	}
}


template<typename T>
inline DK_VOID SafeFreePointer(T*& pointer)
{
	if (pointer)
	{
		DK_FREE(pointer);
		pointer = DK_NULL;
	}
}

inline DK_VOID SafeAssignWString(DK_WSTRING& str, const DK_WCHAR* s)
{
    if (DK_NULL != s)
    {
        str = s;
    }
}

template <typename Iterator>
inline DK_VOID DeleteObjInRange(const Iterator& iterBegin, const Iterator& iterEnd)
{
    for (Iterator iter = iterBegin; iter != iterEnd; iter++)
    {
        if (*iter)
        {
            delete (*iter);
        }
    }
}

template <typename Pointer>
inline DK_VOID SafeDeleteVector(Pointer& pointer)
{
    if (pointer)
    {
        DeleteObjInRange(pointer->begin(), pointer->end());
        delete pointer;
        pointer = DK_NULL;
    }
}

template <typename Vector>
inline DK_VOID ClearObjectInVector(Vector& vec)
{
    DeleteObjInRange(vec.begin(), vec.end());
    vec.clear();
}


#ifdef _WIN32
#define DK_AUTO(x, y) auto x = y
#define DK_TYPEOF(x)    decltype(x)
#define TR1_FUNCTIONAL <functional>
#define TR1_TUPLE <tuple>
#define TR1_MEMORY <memory>
#define DK_UNUSED(x) x
#endif

#ifdef _LINUX
#define DK_AUTO(x, y) typeof(y) x = y
#define DK_TYPEOF(x)    typeof(x)
#define DK_UNUSED(x)
#define TR1_FUNCTIONAL <tr1/functional> 
#define TR1_TUPLE <tr1/tuple> 
#define TR1_MEMORY <tr1/memory>
#endif

#define DK_ROUND(x) (DK_INT((x) >= 0.0 ? (x) + 0.5 : (x) - 0.5))
#define DK_DROUND(x) ((x) >= 0.0 ? (x) + 0.5 : (x) - 0.5)
#define DK_SQUARE(x) ((x) * (x))

// define for base type or custom type which overloades the operator '>' and '<'
#define FIELD_MORE_OPERATOR_COMPARE(x) if (x > rhs.x) {return DK_TRUE;} else if (x < rhs.x) {return DK_FALSE;}
#define FIELD_LESS_OPERATOR_COMPARE(x) if (x < rhs.x) {return DK_TRUE;} else if (x > rhs.x) {return DK_FALSE;}

// define for custom type which defines both of 'FieldMoreCompare' method and 'FieldLessCompare' method
#define FIELD_MORE_FUNCTION_COMPARE(x) if (x.FieldMoreCompare(rhs.x)) {return DK_TRUE;} else if (x.FieldLessCompare(rhs.x)) {return DK_FALSE;}
#define FIELD_LESS_FUNCTION_COMPARE(x) if (x.FieldLessCompare(rhs.x)) {return DK_TRUE;} else if (x.FieldMoreCompare(rhs.x)) {return DK_FALSE;}
//-------------------------------------------
//	Summary:
//	    Align byte to boundary in bytes
//	Parameters:
//		[in] byteLen            - byte length
//		[in] alignInBytes       - to align boundary in bytes, often 4 for image line
//	Remarks:
//	    Null	
//	Return Value:
//		Return count of bytes for byteLen align to alignInBytes
//-------------------------------------------
inline DK_SIZE_T DkByteAlign(const DK_SIZE_T byteLen, const DK_SIZE_T alignInBytes)
{
    return (byteLen + (alignInBytes - 1)) / alignInBytes * alignInBytes;
}


//-------------------------------------------
//	Summary:
//	    Align bit to boundary in bytes
//	Parameters:
//		[in] bitLen             - bit length
//		[in] alignInBytes       - to align boundary in bytes, often 4 for image line
//	Remarks:
//	    Null	
//	Return Value:
//		Return count of bytes for bitLen align to alignInBytes
//-------------------------------------------
inline DK_SIZE_T DkBitAlign(const DK_SIZE_T bitLen, const DK_SIZE_T alignInBytes)
{
    return (bitLen + alignInBytes * 8 - 1) / (alignInBytes * 8) * alignInBytes;
}

template<typename T> 
inline void DkZero(T& v)
{
    memset(&v, 0, sizeof(v));
}


#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
                TypeName(const TypeName&); \
                TypeName& operator=(const TypeName&)

template<typename T1, typename T2>
inline DK_WORD DkMakeWord(T1 lower, T2 upper)
{
    return ((upper & 0xFF )<< 8) | (lower & 0xFF);
}

#define DK_PI 3.1415926

#ifdef _DEBUG
#define DEBUG_POS do{printf("%s:%d:%s\n", __FILE__, __LINE__, __FUNCTION__);}while(0)
#else
#define DEBUG_POS
#endif



// a helper class to make resource release more easy
// in most cases you don't need to use this class directly,
// use MAKE_SCOPE_EXIT
template<typename T, typename D>
class DkScopeExit
{
public:
    DkScopeExit(T t, D d)
        :m_t(t), m_d(d)
    {
    }
    ~DkScopeExit()
    {
        m_d(m_t);
    }
private:
    DISALLOW_COPY_AND_ASSIGN(DkScopeExit);
    T m_t;
    D m_d;
};

#define MAKE_SCOPE_EXIT(name, t, d) DkScopeExit<DK_TYPEOF(t), DK_TYPEOF(d)> name(t, d);

typedef DK_BOOL(*SplitWordsCallBack)(const DK_CHAR* pTextUTF8, DK_INT* pResultArray, DK_UINT arrayLen, DK_UINT* pResultCount);

//-------------------------------------------
//	Summary:
//	    Set CallBack Function to Split Words 
//	Parameters:
//		[in] splitWordsCallBack            - Function
//	Remarks:
//	    The defination of SplitWordsCallBack is (const DK_CHAR* pTextUTF8, DK_INT* pResultArray, DK_UINT arrayLen, DK_UINT* pResultCount),
//      pTextUTF8 means input text string, pResultArray is the output parameter for the result of spliting words, 
//      splitting results should be the positiong of word edge, with unit of one char.
//	Return Value:
//		Return DK_TRUE means succeeded.
//-------------------------------------------
KERNELAPI SetSplitWordsCallBack(SplitWordsCallBack splitWordsCallBack);

#endif
