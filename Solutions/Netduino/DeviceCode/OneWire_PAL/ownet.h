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
// ownet.h - Include file for 1-Wire Net library
//
// Version: 2.10
//
// History: 1.02 -> 1.03 Make sure uchar is not defined twice.
//          1.03 -> 2.00 Changed 'MLan' to 'ow'.
//          2.00 -> 2.01 Added error handling. Added circular-include check.
//          2.01 -> 2.10 Added raw memory error handling and SMALLINT
//          2.10 -> 3.00 Added memory bank functionality
//                       Added file I/O operations
//

#ifndef OWNET_H
#define OWNET_H

#define SMALL_MEMORY_TARGET

//--------------------------------------------------------------//
// Typedefs
//--------------------------------------------------------------//
#ifndef SMALLINT
   //
   // purpose of smallint is for compile-time changing of formal
   // parameters and return values of functions.  For each target
   // machine, an integer is alleged to represent the most "simple"
   // number representable by that architecture.  This should, in
   // most cases, produce optimal code for that particular arch.
   // BUT...  The majority of compilers designed for embedded
   // processors actually keep an int at 16 bits, although the
   // architecture might only be comfortable with 8 bits.
   // The default size of smallint will be the same as that of
   // an integer, but this allows for easy overriding of that size.
   //
   // NOTE:
   // In all cases where a smallint is used, it is assumed that
   // decreasing the size of this integer to something as low as
   // a single byte _will_not_ change the functionality of the
   // application.  e.g. a loop counter that will iterate through
   // several kilobytes of data should not be SMALLINT.  The most
   // common place you'll see smallint is for boolean return types.
   //
   #define SMALLINT int
#endif

#ifndef OW_UCHAR
   #define OW_UCHAR
   typedef unsigned char uchar;
   #if !defined(__MINGW32__) && (defined(__CYGWIN__) || defined(__GNUC__))
      typedef unsigned long ulong;
      //ushort already defined in sys/types.h
      #include <sys/types.h>
   #else
      #if defined(_WIN32) || defined(WIN32) || defined(__MC68K__) || defined(_WIN32_WCE) || defined(_DOS)  || defined(_WINDOWS) || defined(__MINGW32__)
         typedef unsigned short ushort;
         typedef unsigned long ulong;
      #endif
   #endif
   #ifdef __sun__
      #include <sys/types.h>
   #endif
   #ifdef SDCC
      //intent of ushort is 2 bytes unsigned.
      //for ds390 in sdcc, an int, not a short,
      //is 2 bytes.
      typedef unsigned int ushort;
   #endif
#endif

#include <tinyhal.h>

typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned int UINT32;
 
// general defines
#define WRITE_FUNCTION 1
#define READ_FUNCTION  0

// One Wire functions defined in ownetu.c
SMALLINT  owFirst(int pin, SMALLINT do_reset, SMALLINT alarm_only);
SMALLINT  owNext(int pin, SMALLINT do_reset, SMALLINT alarm_only);
void      owSerialNum(int pin, uchar *serialnum_buf, SMALLINT do_read);

// external One Wire functions defined in owsesu.c
SMALLINT owAcquire(int pin, char *port_zstr);
void     owRelease(int pin);

// external One Wire functions from link layer owllu.c
SMALLINT owTouchReset(int pin);
SMALLINT owTouchBit(int pin, SMALLINT sendbit);
SMALLINT owTouchByte(int pin, SMALLINT sendbyte);
SMALLINT owWriteByte(int pin, SMALLINT sendbyte);
SMALLINT owReadByte(int pin);

// external One Wire functions from transaction layer in owtrnu.c
SMALLINT owBlock(int portnum, SMALLINT do_reset, uchar *tran_buf, SMALLINT tran_len);

// link functions
#define usDelay(delay) HAL_Time_Sleep_MicroSeconds_InterruptEnabled(delay)

// external functions defined in crcutil.c
void setcrc16(int portnum, ushort reset);
ushort docrc16(int portnum, ushort cdata);
void setcrc8(int portnum, uchar reset);
uchar docrc8(int portnum, uchar x);

#endif //OWNET_H
