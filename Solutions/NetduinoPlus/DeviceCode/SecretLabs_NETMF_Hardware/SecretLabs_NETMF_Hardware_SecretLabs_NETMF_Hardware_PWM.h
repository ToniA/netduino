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


#ifndef _SECRETLABS_NETMF_HARDWARE_SECRETLABS_NETMF_HARDWARE_PWM_H_
#define _SECRETLABS_NETMF_HARDWARE_SECRETLABS_NETMF_HARDWARE_PWM_H_

namespace SecretLabs
{
    namespace NETMF
    {
        namespace Hardware
        {
            struct PWM
            {
                // Helper Functions to access fields of managed object
                static INT8& Get__disposed( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT8( pMngObj, Library_SecretLabs_NETMF_Hardware_SecretLabs_NETMF_Hardware_PWM::FIELD___disposed ); }

                static UINT32& Get__pin( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UINT32( pMngObj, Library_SecretLabs_NETMF_Hardware_SecretLabs_NETMF_Hardware_PWM::FIELD___pin ); }

                // Declaration of stubs. These functions are implemented by Interop code developers
                static void PWM_Enable( CLR_RT_HeapBlock* pMngObj, UINT32 param0, HRESULT &hr );
                static void PWM_Disable( CLR_RT_HeapBlock* pMngObj, UINT32 param0, HRESULT &hr );
                static void PWM_SetDutyCycle( CLR_RT_HeapBlock* pMngObj, UINT32 param0, UINT32 param1, HRESULT &hr );
                static void PWM_SetPulse( CLR_RT_HeapBlock* pMngObj, UINT32 param0, UINT32 param1, UINT32 param2, HRESULT &hr );
            };
        }
    }
}
#endif  //_SECRETLABS_NETMF_HARDWARE_SECRETLABS_NETMF_HARDWARE_PWM_H_
