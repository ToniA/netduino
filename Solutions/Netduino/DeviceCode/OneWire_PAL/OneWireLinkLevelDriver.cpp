//---------------------------------------------------------------------------
// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//---------------------------------------------------------------------------
//
//  DS520LNK.C - Link Layer functions required by general 1-Wire drive
//           implementation for DS80C520 microcontroller.
//
//  Version: 2.00
//
//  History: 1.00 -> 1.01  Added function msDelay.
//           1.02 -> 1.03  Added function msGettick.
//           1.03 -> 2.00  Changed 'MLan' to 'ow'. Added support for
//                         multiple ports.
//           2.10 -> 3.00  Added owReadBitPower and owWriteBytePower
//

#include "ownet.h"
#include <CPU_GPIO_decl.h>

/////////////////////////////////////////////////////////////////////////////
// 1-Wire Standard Mode Delays
//                                             Min   Rec   Max
  const UINT32 OneWire_Delay_A =   6;       //   5     6    15
//const UINT32 OneWire_Delay_B = -not used- //  59    64   n/a
  const UINT32 OneWire_Delay_C =  60;       //  60    60   120
  const UINT32 OneWire_Delay_D =  10;       //   8    10   n/a
  const UINT32 OneWire_Delay_E =   9;       //   5     9    12
  const UINT32 OneWire_Delay_F =  55;       //  50    55   n/a
//const UINT32 OneWire_Delay_G = -not used- //   0     0     0
  const UINT32 OneWire_Delay_H = 480;       // 480   480   640
  const UINT32 OneWire_Delay_I =  70;       //  63    70    78
  const UINT32 OneWire_Delay_J = 410;       // 410   410   n/a


//--------------------------------------------------------------------------
// Reset all of the devices on the 1-Wire Net and return the result.
//
// 'pin'        - GPIO pin
//
// Returns: TRUE(1):  presense pulse(s) detected, device(s) reset
//          FALSE(0): no presense pulses detected
//
SMALLINT owTouchReset(int pin)
{
   // Code from appnote 126.
   CPU_GPIO_EnableOutputPin(pin, FALSE);  // impulse start OW_PORT = 0; // drive bus low.
   usDelay(OneWire_Delay_H - 6);

   // OW_PORT = 1; // bus high. 
   // Note that the 1Wire bus will need external pullup to supply the required current
   CPU_GPIO_EnableInputPin(pin, FALSE, NULL, GPIO_INT_NONE, RESISTOR_PULLUP);
   usDelay(OneWire_Delay_I - 2); 

   // get presence detect pulse.
   uchar result = !CPU_GPIO_GetPinState(pin);
   usDelay(OneWire_Delay_J - 2);

   return result;
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net and return the
// result 1 bit read from the 1-Wire Net.  The parameter 'sendbit'
// least significant bit is used and the least significant bit
// of the result is the return bit.
//
// 'pin'        - GPIO pin
// 'sendbit'    - the least significant bit is the bit to send
//
// Returns: 0:   0 bit read from sendbit
//          1:   1 bit read from sendbit
//
SMALLINT owTouchBit(int pin, SMALLINT sendbit)
{
   unsigned char result=0;

   //timing critical, so I'll disable interrupts here
   GLOBAL_LOCK(irq);

   // start timeslot.
   CPU_GPIO_EnableOutputPin(pin, FALSE);

   if(sendbit == 1)
   {
     usDelay(OneWire_Delay_A - 5);

     // send 1 bit out.
     // sample result @ 15 us.
     CPU_GPIO_EnableInputPin(pin, FALSE, NULL, GPIO_INT_NONE, RESISTOR_PULLUP);
     usDelay(OneWire_Delay_E - 4);
     result = CPU_GPIO_GetPinState(pin);
     usDelay(OneWire_Delay_F - 2);
   }
   else
   {
     // The level is '0' (the first EnableOutputPin call)
     usDelay(OneWire_Delay_C - 4);

     ::CPU_GPIO_SetPinState(pin, TRUE);
     usDelay(OneWire_Delay_D - 2);
   }
   // timeslot done. Set output high.
   CPU_GPIO_SetPinState(pin, TRUE);

   //restore interrupts
   irq.Release();

   return result;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and return the
// result 8 bits read from the 1-Wire Net.  The parameter 'sendbyte'
// least significant 8 bits are used and the least significant 8 bits
// of the result is the return byte.
//
// 'pin'        - GPIO pin
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  8 bits read from sendbyte
//
SMALLINT owTouchByte(int pin, SMALLINT sendbyte)
{
  uchar i;
  uchar result = 0;

  for(i = 0; i < 8; i++)
  {
    result |= (owTouchBit(pin, sendbyte & 1) << i);
    sendbyte >>= 1;
  }

  return result;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).
// The parameter 'sendbyte' least significant 8 bits are used.
//
// 'pin'        - GPIO pin
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same
//
SMALLINT owWriteByte(int pin, SMALLINT sendbyte)
{
  return (owTouchByte(pin, sendbyte) == sendbyte) ? TRUE : FALSE;
}

//--------------------------------------------------------------------------
// Send 8 bits of read communication to the 1-Wire Net and and return the
// result 8 bits read from the 1-Wire Net.
//
// 'pin'        - GPIO pin
//
// Returns:  8 bytes read from 1-Wire Net
//
SMALLINT owReadByte(int pin)
{
  return owTouchByte(pin, 0xFF);
}
