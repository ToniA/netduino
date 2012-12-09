//-----------------------------------------------------------------------------
//
//                   ** WARNING! ** 
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------


#include "SecretLabs_NETMF_Hardware.h"
#include "SecretLabs_NETMF_Hardware_SecretLabs_NETMF_Hardware_AnalogInput.h"

using namespace SecretLabs::NETMF::Hardware;

//--//

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section zidata = "g_SecretLabs_ADC_Initialized"
#endif

BOOL g_SecretLabs_ADC_Initialized = FALSE;

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section zidata
#endif

//--//

#define ADC_CHANNEL_OUT_OF_RANGE (999)	// channel out of range (this acts as an error code -- the channel is out of range)

static UINT32 PinToChannel( UINT32 pin )
{
    if ((pin >= 59) && (pin <= 62))
    {
        // AD0-AD3 (peripheral B on pins PB27-PB30)
        return pin - 59;
    }
#if defined(PLATFORM_ARM_Netduino) || defined(PLATFORM_ARM_NetduinoPlus)
    else if ((pin >= 10) && (pin <= 11))
    {
        // AD4-AD5 (multiplexed externally to MCU with TWD/TWCK)
        return pin - 6;
    }
#endif
    else
    {
        return ADC_CHANNEL_OUT_OF_RANGE; // channel out of range (this acts as an error code -- the channel is out of range)
    }
}

static UINT32 ChannelToPin( UINT32 channel )
{
    if ((channel >= 0) && (channel <= 3))
    {
        // AD0-AD3 (peripheral B on pins PB27-PB30)
        return channel + 59;
    }
#if defined(PLATFORM_ARM_Netduino) || defined(PLATFORM_ARM_NetduinoPlus)
    else if ((channel >= 4) && (channel <= 5))
    {
        // AD4-AD5 (multiplexed externally to MCU with TWD/TWCK)
        return channel + 6;
    }
#endif
    else
    {
        return ADC_CHANNEL_OUT_OF_RANGE; // channel out of range (this acts as an error code -- the channel is out of range)
    }
}

void AnalogInput::ADC_Enable( CLR_RT_HeapBlock* pMngObj, UINT32 param0, HRESULT &hr )
{
    BOOL pinAllocated = FALSE;
    UINT32 channel = PinToChannel(param0);

    // verify that this is a valid ADC pin; otherwise, throw ArgumentOutOfRangeException
#if defined(PLATFORM_ARM_Netduino) || defined(PLATFORM_ARM_NetduinoPlus)
    if (channel > 5)
#elif defined(PLATFORM_ARM_NetduinoMini)
    if (channel > 3)
#endif
    {
        hr = CLR_E_OUT_OF_RANGE;
        return;
    }

#if defined(PLATFORM_ARM_Netduino) || defined(PLATFORM_ARM_NetduinoPlus)
    // Reserve our MUX pin.  If reservation fails: no luck, exit with error CLR_E_PIN_UNAVAILABLE
    if (channel == 4 || channel == 5)
    {
        BOOL muxReserved;
        if (channel == 4)
	    muxReserved = ::CPU_GPIO_ReservePin(AT91_GPIO_Driver::PA14, TRUE); // reserve MUX1
	else if (channel == 5)
	    muxReserved = ::CPU_GPIO_ReservePin(AT91_GPIO_Driver::PA15, TRUE); // reserve MUX2
	if (muxReserved == FALSE)
	{
	    hr = CLR_E_PIN_UNAVAILABLE;
	    return;
	}
    }
#endif

#if defined(PLATFORM_ARM_Netduino) || defined(PLATFORM_ARM_NetduinoPlus)
    // Reserve the pin. 
    if (channel == 4 || channel == 5)
        pinAllocated = TRUE; // no GPIO pin to allocate for AD4/AD5
    else
#endif
        pinAllocated = ::CPU_GPIO_ReservePin( param0, TRUE );
		
    // Check if pin reservation suceeded
    if(pinAllocated == TRUE)
    {
#if defined(PLATFORM_ARM_Netduino) || defined(PLATFORM_ARM_NetduinoPlus)
        // pin allocation was successful; activate MUXes as appropriate.
	if (channel == 4)
            ::CPU_GPIO_EnableOutputPin(AT91_GPIO_Driver::PA14, TRUE); // MUX1 -- select AD4
	else if (channel == 5)
	    ::CPU_GPIO_EnableOutputPin(AT91_GPIO_Driver::PA15, TRUE); // MUX2 -- select AD5
#endif
    }
    else
    {
#if defined(PLATFORM_ARM_Netduino) || defined(PLATFORM_ARM_NetduinoPlus)
	// reservation failed: clean up our MUX reservations and exit with error CLR_E_PIN_UNAVAILABLE
	if (channel == 4)
	    ::CPU_GPIO_ReservePin(AT91_GPIO_Driver::PA14, FALSE); // un-reserve MUX1
    else if (channel == 5)
        ::CPU_GPIO_ReservePin(AT91_GPIO_Driver::PA15, FALSE); // un-reserve MUX2
#endif
			
        hr = CLR_E_PIN_UNAVAILABLE;
        return;
    }
	
    if (channel < 4)
    {
        // switch this GPIO pin to the alternate B function (ADC)
        AT91_GPIO_Driver::DisablePin(param0, RESISTOR_DISABLED, 0, GPIO_ALT_MODE_2);
    }
	
    AT91_ADC& ADC = AT91::ADC();

    if (!g_SecretLabs_ADC_Initialized)
    {
        // enable ADC peripheral clock
        AT91_PMC &pmc = AT91::PMC();
        pmc.EnablePeriphClock(AT91C_ID_ADC);
    
        // reset the ADC
        ADC.ADC_CR = AT91_ADC::ADC_SWRST;    

	// configure the adc...
        const unsigned int adcfreq = 5000000;
        unsigned int prescal = ((SYSTEM_CLOCK_HZ / (2 * adcfreq)) - 1) & 0x3F;
        unsigned int startup = (((adcfreq / 1000000) * 10 / 8) - 1) & 0x1F;
        unsigned int shtim = ((((adcfreq / 1000000) * 1200) / 1000) - 1) & 0xF;
        ADC.ADC_MR = (prescal << 8) | (startup << 16) | (shtim << 24);

        g_SecretLabs_ADC_Initialized = TRUE;
    }

    // enable ADC channel
    ADC.ADC_CHER = (1 << channel);
}

void AnalogInput::ADC_Disable( CLR_RT_HeapBlock* pMngObj, UINT32 param0, HRESULT &hr )
{
    UINT32 channel = PinToChannel(param0);

    AT91_ADC& ADC = AT91::ADC();

    // disable channel
    ADC.ADC_CHDR = (1 << channel);

    // unreserve the pin
    ::CPU_GPIO_ReservePin( param0, FALSE );
	
#if defined(PLATFORM_ARM_Netduino) || defined(PLATFORM_ARM_NetduinoPlus)
    // deactivate and unreserve our MUX lines
    if (channel == 4)
    {
        ::CPU_GPIO_SetPinState(AT91_GPIO_Driver::PA14, FALSE); // deactivate MUX1 (switch to I2C GPIO)
        ::CPU_GPIO_ReservePin(AT91_GPIO_Driver::PA14, FALSE); // unreserve MUX1
    }
    else if (channel == 5)
    {
		::CPU_GPIO_SetPinState(AT91_GPIO_Driver::PA15, FALSE); // deactivate MUX2 (switch to I2C GPIO)
		::CPU_GPIO_ReservePin(AT91_GPIO_Driver::PA15, FALSE); // unreserve MUX2
    }
	
#endif
}

UINT32 AnalogInput::ADC_Read( CLR_RT_HeapBlock* pMngObj, UINT32 param0, HRESULT &hr )
{
    UINT32 channel = PinToChannel(param0);

    AT91_ADC& ADC = AT91::ADC();

    // start the adc conversion
    ADC.ADC_CR = (0x1 <<  1);

    // wait while the conversion is completed
    while((ADC.ADC_SR & (1 << channel)) == 0);

    // return the conversion value
    switch(channel) {
        case 0:
            return ADC.ADC_CDR0;    
        case 1: 
            return ADC.ADC_CDR1;
        case 2:
            return ADC.ADC_CDR2;
        case 3: 
            return ADC.ADC_CDR3;
#if defined(PLATFORM_ARM_Netduino) || defined(PLATFORM_ARM_NetduinoPlus)
        case 4:
            return ADC.ADC_CDR4;
        case 5:
            return ADC.ADC_CDR5;
#endif
//        case 6:
//            return ADC.ADC_CDR6;
//        case 7:
//            return ADC.ADC_CDR7;
        default:
            return 0; // we will never get here
    }
}
