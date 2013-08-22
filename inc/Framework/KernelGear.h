//===========================================================================
// Summary:
//      KernelGear.h
//      负责调用 Kernel 公共接口的初始化、销毁、字体注册、字体设置等
// Version:
//      0.1 draft - creating.
//
// Usage:
//      Null
// Remarks:
//      Null
// Date:
//      2011-12-28
// Author:
//      liuhj@duokan.com
//===========================================================================

#ifndef __KINDLEAPP_INC_FRAMEWORK_KERNELGEAR_H__
#define __KINDLEAPP_INC_FRAMEWORK_KERNELGEAR_H__

//-------------------------------------------
// Summary:
//        初始化各格式解析引擎
//-------------------------------------------
void InitializeKernel ();

//-------------------------------------------
// Summary:
//        销毁各格式解析引擎
//-------------------------------------------
void DestroyKernel ();

//-------------------------------------------
// Summary:
//        为各格式解析引擎注册字体
// Remarks:
//        当用户添加、删除字体文件时，需要再次注册字体
//-------------------------------------------
void RegisterKernelFonts ();
void UnRegisterKernelFonts ();

//-------------------------------------------
// Summary:
//        为各格式解析引擎设置默认字体
// Remarks:
//        当用户自定义字体时， 需要再次设置默认字体
//-------------------------------------------
void SetKernelDefaultFonts ();



#endif // #ifndef__KINDLEAPP_INC_FRAMEWORK_KERNELGEAR_H__
