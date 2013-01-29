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
//  ownet.C - Network functions for 1-Wire net devices.
//
//  Version: 3.00
//
//  History: 1.00 -> 1.01  Change to owFamilySearchSetup, LastDiscrepancy[portnum]
//                         was set to 64 instead of 8 to enable devices with
//                         early contention to go in the '0' direction first.
//           1.02 -> 1.03  Initialized goodbits in  owVerify
//           1.03 -> 2.00  Changed 'MLan' to 'ow'. Added support for
//                         multiple ports.
//           2.00 -> 2.01  Added support for owError library
//           2.01 -> 3.00  Make search functions consistent with AN187
//

#include "ownet.h"

static GPIO_PIN _arPins[] = {
#if defined(PLATFORM_ARM_Netduino) || defined(PLATFORM_ARM_NetduinoPlus) || defined(PLATFORM_ARM_NetduinoMini)
  AT91_GPIO_Driver::PA0,  // PA0/RXD0
  AT91_GPIO_Driver::PA1,  // PA1/TXD0
#if !defined(PLATFORM_ARM_NetduinoMini)
  AT91_GPIO_Driver::PA3,  // PA3/RTS0
  AT91_GPIO_Driver::PA4,  // PA4/CTS0
#endif
  AT91_GPIO_Driver::PA10, // PA10/TWD
  AT91_GPIO_Driver::PA11, // PA11/TWCK
  AT91_GPIO_Driver::PA12, // PA12/SPI_NPCS0
  AT91_GPIO_Driver::PA16, // PA16/SPI0_MISO
  AT91_GPIO_Driver::PA17, // PA17/SPI0_MOSI
  AT91_GPIO_Driver::PA18, // PA18/SPI0_SPCK
#if !defined(PLATFORM_ARM_NetduinoMini)
  AT91_GPIO_Driver::PA27, // PA27/DRXD/PCK3
  AT91_GPIO_Driver::PA28, // PA28/DTXD
#endif
  AT91_GPIO_Driver::PB19, // PB19/PWM0/TCLK1
  AT91_GPIO_Driver::PB20, // PB20/PWM1/PCK0
  AT91_GPIO_Driver::PB21, // PB21/PWM2/PCK1
  AT91_GPIO_Driver::PB22, // PB22/PWM3/PCK2
  AT91_GPIO_Driver::PB27, // PB27/TIOA2/PWM0/AD0
  AT91_GPIO_Driver::PB28, // PB28/TIOB2/PWM1/AD1
  AT91_GPIO_Driver::PB29, // PB29/PCK1/PWM2/AD2
  AT91_GPIO_Driver::PB30, // PB30/PCK2/PWM3/AD3
#else
  #error OneWire: Unsupported Netduino platform
#endif
};

// Maps pin to portnum index (0 .. MAX_PORTNUM - 1)
static int _owPinToPortNum(int pin)
{
  for(int i = 0; i < ARRAYSIZE(_arPins); i++)
  {
    if(_arPins[i] == pin)
    {
      return i;
    }
  }
  return -1;  // Not found
}

#undef MAX_PORTNUM
#define MAX_PORTNUM ARRAYSIZE(_arPins)

// global variables for this module to hold search state information
static SMALLINT LastDiscrepancy[MAX_PORTNUM];
static SMALLINT LastFamilyDiscrepancy[MAX_PORTNUM];
static SMALLINT LastDevice[MAX_PORTNUM];
uchar SerialNum[MAX_PORTNUM][8];

// exportable functions defined in ownet.c
SMALLINT bitacc(SMALLINT,SMALLINT,SMALLINT,uchar *);

//--------------------------------------------------------------------------
// The 'owFirst' finds the first device on the 1-Wire Net  This function
// contains one parameter 'alarm_only'.  When
// 'alarm_only' is TRUE (1) the find alarm command 0xEC is
// sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
// 'pin'        - GPIO pin
// 'do_reset'   - TRUE (1) perform reset before search, FALSE (0) do not
//                perform reset before search.
// 'alarm_only' - TRUE (1) the find alarm command 0xEC is
//                sent instead of the normal search command 0xF0
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's
//                        Serial Number placed in the global SerialNum[portnum]
//            FALSE (0): There are no devices on the 1-Wire Net.
//
SMALLINT owFirst(int pin, SMALLINT do_reset, SMALLINT alarm_only)
{
  int portnum;
  if((portnum = _owPinToPortNum(pin)) < 0)
  {
    return FALSE;
  }
  // reset the search state
  LastDiscrepancy[portnum] = 0;
  LastDevice[portnum] = FALSE;
  LastFamilyDiscrepancy[portnum] = 0;

  return owNext(pin, do_reset, alarm_only);
}

//--------------------------------------------------------------------------
// The 'owNext' function does a general search.  This function
// continues from the previos search state. The search state
// can be reset by using the 'owFirst' function.
// This function contains one parameter 'alarm_only'.
// When 'alarm_only' is TRUE (1) the find alarm command
// 0xEC is sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
// 'pin'        - GPIO pin
// 'do_reset'   - TRUE (1) perform reset before search, FALSE (0) do not
//                perform reset before search.
// 'alarm_only' - TRUE (1) the find alarm command 0xEC is
//                sent instead of the normal search command 0xF0
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's
//                       Serial Number placed in the global SerialNum[portnum]
//            FALSE (0): when no new device was found.  Either the
//                       last search was the last device or there
//                       are no devices on the 1-Wire Net.
//
SMALLINT owNext(int pin, SMALLINT do_reset, SMALLINT alarm_only)
{
  uchar bit_test, search_direction, bit_number;
  uchar last_zero, serial_byte_number, next_result;
  uchar serial_byte_mask;
  uchar lastcrc8;

  int portnum;
  if((portnum = _owPinToPortNum(pin)) < 0)
  {
    return FALSE;
  }

  // initialize for search
  lastcrc8=0;
  bit_number = 1;
  last_zero = 0;
  serial_byte_number = 0;
  serial_byte_mask = 1;
  next_result = 0;
  setcrc8(portnum,0);

  // if the last call was not the last one
  if (!LastDevice[portnum])
  {
    // check if reset first is requested
    if (do_reset)
    {
      // reset the 1-wire
      // if there are no parts on 1-wire, return FALSE
      if (!owTouchReset(pin))
      {
        // reset the search
        LastDiscrepancy[portnum] = 0;
        LastFamilyDiscrepancy[portnum] = 0;
        //OWERROR(OWERROR_NO_DEVICES_ON_NET);
        return FALSE;
      }
    }

    // If finding alarming devices issue a different command
    if (alarm_only)
      owWriteByte(pin, 0xEC);  // issue the alarming search command
    else
      owWriteByte(pin, 0xF0);  // issue the search command

    //pause before beginning the search
    usDelay(10);

    // loop to do the search
    do
    {
      // read a bit and its compliment
      bit_test = owTouchBit(pin, 1) << 1;
      bit_test |= owTouchBit(pin, 1);

      // check for no devices on 1-wire
      if (bit_test == 3)
      {
        break;
      }
      else
      {
        // all devices coupled have 0 or 1
        if (bit_test > 0)
        {
          search_direction = !(bit_test & 0x01);  // bit write value for search
        }
        else
        {
          // if this discrepancy if before the Last Discrepancy
          // on a previous next then pick the same as last time
          if (bit_number < LastDiscrepancy[portnum])
            search_direction = ((SerialNum[portnum][serial_byte_number] & serial_byte_mask) > 0);
          else
            // if equal to last pick 1, if not then pick 0
            search_direction = (bit_number == LastDiscrepancy[portnum]);

          // if 0 was picked then record its position in LastZero
          if (search_direction == 0)
          {
            last_zero = bit_number;

            // check for Last discrepancy in family
            if (last_zero < 9)
              LastFamilyDiscrepancy[portnum] = last_zero;
          }
        }

        // set or clear the bit in the SerialNum[portnum] byte serial_byte_number
        // with mask serial_byte_mask
        if (search_direction == 1)
          SerialNum[portnum][serial_byte_number] |= serial_byte_mask;
        else
          SerialNum[portnum][serial_byte_number] &= ~serial_byte_mask;

        // serial number search direction write bit
        owTouchBit(pin, search_direction);

        // increment the byte counter bit_number
        // and shift the mask serial_byte_mask
        bit_number++;
        serial_byte_mask <<= 1;

        // if the mask is 0 then go to new SerialNum[portnum] byte serial_byte_number
        // and reset mask
        if (serial_byte_mask == 0)
        {
          // The below has been added to accomidate the valid CRC with the
          // possible changing serial number values of the DS28E04.
          if (((SerialNum[portnum][0] & 0x7F) == 0x1C) && (serial_byte_number == 1))
            lastcrc8 = docrc8(portnum,0x7F);
          else
            lastcrc8 = docrc8(portnum,SerialNum[portnum][serial_byte_number]);  // accumulate the CRC

          serial_byte_number++;
          serial_byte_mask = 1;
        }
      }
    }
    while(serial_byte_number < 8);  // loop until through all SerialNum[portnum] bytes 0-7

    // if the search was successful then
    if (!((bit_number < 65) || lastcrc8))
    {
      // search successful so set LastDiscrepancy[portnum],LastDevice[portnum],next_result
      LastDiscrepancy[portnum] = last_zero;
      LastDevice[portnum] = (LastDiscrepancy[portnum] == 0);
      next_result = TRUE;
    }
  }

  // if no device found then reset counters so next 'next' will be
  // like a first
  if (!next_result || !SerialNum[portnum][0])
  {
    LastDiscrepancy[portnum] = 0;
    LastDevice[portnum] = FALSE;
    LastFamilyDiscrepancy[portnum] = 0;
    next_result = FALSE;
  }

  return next_result;
}

//--------------------------------------------------------------------------
// The 'owSerialNum' function either reads or sets the SerialNum buffer
// that is used in the search functions 'owFirst' and 'owNext'.
// This function contains two parameters, 'serialnum_buf' is a pointer
// to a buffer provided by the caller.  'serialnum_buf' should point to
// an array of 8 unsigned chars.  The second parameter is a flag called
// 'do_read' that is TRUE (1) if the operation is to read and FALSE
// (0) if the operation is to set the internal SerialNum buffer from
// the data in the provided buffer.
//
// 'pin'        - GPIO pin
// 'serialnum_buf' - buffer to that contains the serial number to set
//                   when do_read = FALSE (0) and buffer to get the serial
//                   number when do_read = TRUE (1).
// 'do_read'       - flag to indicate reading (1) or setting (0) the current
//                   serial number.
//
void owSerialNum(int pin, uchar *serialnum_buf, SMALLINT do_read)
{
  int portnum;
  if((portnum = _owPinToPortNum(pin)) < 0)
  {
    return;
  }

  uchar i;

  // read the internal buffer and place in 'serialnum_buf'
  if (do_read)
  {
    for (i = 0; i < 8; i++)
      serialnum_buf[i] = SerialNum[portnum][i];
  }
  // set the internal buffer from the data in 'serialnum_buf'
  else
  {
    for (i = 0; i < 8; i++)
      SerialNum[portnum][i] = serialnum_buf[i];
  }
}

//--------------------------------------------------------------------------
// Bit utility to read and write a bit in the buffer 'buf'.
//
// 'op'    - operation (1) to set and (0) to read
// 'state' - set (1) or clear (0) if operation is write (1)
// 'loc'   - bit number location to read or write
// 'buf'   - pointer to array of bytes that contains the bit
//           to read or write
//
// Returns: 1   if operation is set (1)
//          0/1 state of bit number 'loc' if operation is reading
//
SMALLINT bitacc(SMALLINT op, SMALLINT state, SMALLINT loc, uchar *buf)
{
  SMALLINT nbyt,nbit;

  nbyt = (loc / 8);
  nbit = loc - (nbyt * 8);

  if (op == WRITE_FUNCTION)
  {
    if (state)
      buf[nbyt] |= (0x01 << nbit);
    else
      buf[nbyt] &= ~(0x01 << nbit);

    return 1;
  }
  else
    return ((buf[nbyt] >> nbit) & 0x01);
}

//--------------------------------------------------------------------------
//
//  crcutil.c - Keeps track of the CRC for 16 and 8 bit operations
//  version 2.00

// Local global variables
ushort utilcrc16[MAX_PORTNUM];
uchar utilcrc8[MAX_PORTNUM];
static short oddparity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };
static uchar dscrc_table[] = {
        0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
      157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
       35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
      190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
       70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
      219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
      101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
      248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
      140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
       17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
      175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
       50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
      202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
       87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
      233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
      116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};

//--------------------------------------------------------------------------
// Reset crc16 to the value passed in
//
// 'reset' - data to set crc16 to.
//
void setcrc16(int portnum, ushort reset)
{
   utilcrc16[portnum&0x0FF] = reset;
   return;
}

//--------------------------------------------------------------------------
// Reset crc8 to the value passed in
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number is provided to
//              indicate the symbolic port number.
// 'reset'    - data to set crc8 to
//
void setcrc8(int portnum, uchar reset)
{
   utilcrc8[portnum&0x0FF] = reset;
   return;
}

//--------------------------------------------------------------------------
// Calculate a new CRC16 from the input data short.  Return the current
// CRC16 and also update the global variable CRC16.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number is provided to
//              indicate the symbolic port number.
// 'data'     - data to perform a CRC16 on
//
// Returns: the current CRC16
//
ushort docrc16(int portnum, ushort cdata)
{
   cdata = (cdata ^ (utilcrc16[portnum&0x0FF] & 0xff)) & 0xff;
   utilcrc16[portnum&0x0FF] >>= 8;

   if (oddparity[cdata & 0xf] ^ oddparity[cdata >> 4])
     utilcrc16[portnum&0x0FF] ^= 0xc001;

   cdata <<= 6;
   utilcrc16[portnum&0x0FF]   ^= cdata;
   cdata <<= 1;
   utilcrc16[portnum&0x0FF]   ^= cdata;

   return utilcrc16[portnum&0x0FF];
}

//--------------------------------------------------------------------------
// Update the Dallas Semiconductor One Wire CRC (utilcrc8) from the global
// variable utilcrc8 and the argument.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number is provided to
//              indicate the symbolic port number.
// 'x'        - data byte to calculate the 8 bit crc from
//
// Returns: the updated utilcrc8.
//
uchar docrc8(int portnum, uchar x)
{
   utilcrc8[portnum&0x0FF] = dscrc_table[utilcrc8[portnum&0x0FF] ^ x];
   return utilcrc8[portnum&0x0FF];
}
