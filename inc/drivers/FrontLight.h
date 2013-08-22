////////////////////////////////////////////////////////////////////////
// FrontLight.h
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __FRONTLIGHT_H__
#define __FRONTLIGHT_H__

class FrontLight
{
public:
	static void InitDevice(void);
	static void CloseDevice(void);
	
	// 0 - 24 lelvel
	static int GetLevel();
	static bool SetLevel(int level);
	static bool WriteSystemLevel();


	static void Reset();
	static bool SetLevelKindle(int level);

	// read /write config 
private:
#ifdef _X86_LINUX
    static int s_level;
#else
	static int ReadSystemLevel();
	static int m_fl;
    static bool SetIntensity(int intensity);
	static int GetMaxIntensity(void);
	// 0 - 255 intensity
	static int GetIntensity(void);
#endif
};


#endif
