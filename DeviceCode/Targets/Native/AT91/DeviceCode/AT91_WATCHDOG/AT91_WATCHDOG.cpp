////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
// This file is part of the Microsoft .NET Micro Framerwork Porting Kit Code Samples and is unsupported. 
// Copyright (C) Microsoft Corporation. All rights reserved. Use of this sample source code is subject to 
// the terms of the Microsoft license agreement under which you licensed this sample source code. 
// 
// THIS SAMPLE CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSPOSE.
// 
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "Watchdog_driver.h"


#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section zidata = "g_AT91_WATCHDOG_Driver"
#endif

AT91_WATCHDOG_Driver g_AT91_WATCHDOG_Driver;

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section zidata
#endif

//--//

HRESULT Watchdog_Enable(UINT32 TimeoutMilliseconds, WATCHDOG_INTERRUPT_CALLBACK callback, void* context)
{
    NATIVE_PROFILE_HAL_PROCESSOR_WATCHDOG();

    // The power-up default value is 0x3FFF_2FFF
    // WMR_WDV       = 0xFFF (maximum value)
    // WMR_WDFIEN    = 0 (OFF)              A Watchdog fault (underflow or error) has no effect on interrupt
    // WMR_WDRSTEN   = 1 (ON) -> 0x2000     A Watchdog fault (underflow or error) triggers a Watchdog reset
    // WMR_WDRPROC   = 0 (OFF)              If WDRSTEN is 1, a Watchdog fault (underflow or error) activates all resets
    // WMR_WDDIS     = 0 (OFF)              Enables the Watchdog Timer
    // 
    // WMR_WDD       = 0xFFF0000 (maximum value)
    // WMR_WDDBGHLT  = 1 (ON) -> 0x10000000 The Watchdog stops when the processor is in debug state
    // WMR_WDIDLEHLT = 1 (ON) -> 0x20000000 The Watchdog stops when the system is in idle state
            
    //AT91_WATCHDOG &wtdg = AT91::WTDG();

    // Use the IDLE HALT flag to tell if the Managed App has enabled the watchdog
    *((volatile UINT32*) 0xFFFFFD44) = (UINT32)(AT91_WATCHDOG::WMR_WDV_mask | AT91_WATCHDOG::WMR_WDRSTEN | AT91_WATCHDOG::WMR_WDRPROC |
                                                AT91_WATCHDOG::WMR_WDD_mask | AT91_WATCHDOG::WMR_WDDBGHLT);

    // Reset the watchdog

    *((volatile UINT32*) 0xFFFFFD40) = (UINT32)(AT91_WATCHDOG::WCR__KEY | AT91_WATCHDOG::WCR__RESTART);

    return S_OK;
}

void Watchdog_Disable()
{
    NATIVE_PROFILE_HAL_PROCESSOR_WATCHDOG();

    // WMR_WDDIS     = 0 (OFF) -> 0x8000    Enables the Watchdog Timer

    //AT91_WATCHDOG &wtdg = AT91::WTDG();
    *((volatile UINT32*) 0xFFFFFD44) = AT91_WATCHDOG::WMR_WDDIS;
}

void Watchdog_ResetCounter()
{
    NATIVE_PROFILE_HAL_PROCESSOR_WATCHDOG();

    AT91_WATCHDOG &wtdg = AT91::WTDG();

    // Use the IDLE HALT flag to tell if the Managed App has enabled the watchdog
    if ( *((volatile UINT32*) 0xFFFFFD44) == (UINT32)0x3FFF2FFF ) {
        *((volatile UINT32*) 0xFFFFFD40) = (UINT32)(AT91_WATCHDOG::WCR__KEY | AT91_WATCHDOG::WCR__RESTART);

        // Debug code, turn off the onboard led every time the watchdog is fed
        // CPU_GPIO_EnableOutputPin(AT91_GPIO_Driver::PB23, false);
        // CPU_GPIO_SetPinState(AT91_GPIO_Driver::PB23, false);
    }
}

void Watchdog_ResetCpu()
{
    NATIVE_PROFILE_HAL_PROCESSOR_WATCHDOG();

    // disable interrupts and loop forever -> the watchdog bites
    GLOBAL_LOCK(irq);

    while(true);
}

//--//
