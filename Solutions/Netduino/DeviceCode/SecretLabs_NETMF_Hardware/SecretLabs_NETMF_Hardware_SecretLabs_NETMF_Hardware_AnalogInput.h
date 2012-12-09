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


#ifndef _SECRETLABS_NETMF_HARDWARE_SECRETLABS_NETMF_HARDWARE_ANALOGINPUT_H_
#define _SECRETLABS_NETMF_HARDWARE_SECRETLABS_NETMF_HARDWARE_ANALOGINPUT_H_

namespace SecretLabs
{
    namespace NETMF
    {
        namespace Hardware
        {
            struct AnalogInput
            {
                // Helper Functions to access fields of managed object
                static INT8& Get__disposed( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT8( pMngObj, Library_SecretLabs_NETMF_Hardware_SecretLabs_NETMF_Hardware_AnalogInput::FIELD___disposed ); }

                static UINT32& Get__pin( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UINT32( pMngObj, Library_SecretLabs_NETMF_Hardware_SecretLabs_NETMF_Hardware_AnalogInput::FIELD___pin ); }

                static INT32& Get__minValue( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_SecretLabs_NETMF_Hardware_SecretLabs_NETMF_Hardware_AnalogInput::FIELD___minValue ); }

                static INT32& Get__maxValue( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_SecretLabs_NETMF_Hardware_SecretLabs_NETMF_Hardware_AnalogInput::FIELD___maxValue ); }

                // Declaration of stubs. These functions are implemented by Interop code developers
                static void ADC_Enable( CLR_RT_HeapBlock* pMngObj, UINT32 param0, HRESULT &hr );
                static void ADC_Disable( CLR_RT_HeapBlock* pMngObj, UINT32 param0, HRESULT &hr );
                static UINT32 ADC_Read( CLR_RT_HeapBlock* pMngObj, UINT32 param0, HRESULT &hr );
            };
        }
    }
}
#endif  //_SECRETLABS_NETMF_HARDWARE_SECRETLABS_NETMF_HARDWARE_ANALOGINPUT_H_
