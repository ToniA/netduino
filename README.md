Netduino Plus (1) firmware 4.2.0.1 source with CW2's 1-wire extensions and hardware watchdog
============================================================================================

Build command with RVDS 3.1 installed:

    call setenv_rvds.cmd 3.1
    msbuild solutions\netduinoplus\dotnetmf.proj /p:flavor=release;tcp_ip_stack=lwip /m:2

Installation:
* Install the TinyBooterDecompressor.bin with SAM-BA
* Disconnect ND+ and reconnect while PRESSING THE ONBOARD BUTTON at the same time -> the onboard led should turn off (to signal that the watchdog has been turned off for this bootup)
* Deploy the firmware images using MFDeploy

Usage:

The watchdog is enabled by default in the firmware. The behaviour can be changed exactly once, 
as the watchdog control register on the AT91SAM7X512 is a write-once type register.

    Watchdog.Enabled = true;

or

    Watchdog.Enabled = false;
