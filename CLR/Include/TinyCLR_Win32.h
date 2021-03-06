////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _TINYCLR_WIN32_H_
#define _TINYCLR_WIN32_H_

////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_Windows_FastSleep( INT64 ticks );

BOOL HAL_Windows_IsShutdownPending();

void HAL_Windows_AcquireGlobalLock();
void HAL_Windows_ReleaseGlobalLock();
BOOL HAL_Windows_HasGlobalLock();

UINT64 HAL_Windows_GetPerformanceTicks();

void HAL_Windows_Debug_Print( LPSTR szText );
                        
////////////////////////////////////////////////////////////////////////////////////////////////////

class HAL_Mutex
{
    CRITICAL_SECTION m_data;

public:
    HAL_Mutex();
    ~HAL_Mutex();

    //--//

    void Lock  ();
    void Unlock();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _TINYCLR_WIN32_H_