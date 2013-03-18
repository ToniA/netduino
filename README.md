Netduino Plus with HW watchdog
==============================

This is the Netduino Plus (1) firmware 4.2.0.1 source with CW2's 1-wire extensions and hardware watchdog. 
The binaries are available here: [http://forums.netduino.com/index.php?/topic/6968-hardware-watchdog-on-netduino-plus/?p=47334](http://forums.netduino.com/index.php?/topic/6968-hardware-watchdog-on-netduino-plus/?p=47334)


Build
-----
Build command with RVDS 4.1 installed:

    call setenv_rvds.cmd 4.1
    msbuild solutions\netduinoplus\dotnetmf.proj /p:flavor=release;tcp_ip_stack=lwip /m:2

Installation
------------
* Connect 3.3V to the golden square to reset the board
* Disconnect and reconnect the USB
* Install the TinyBooterDecompressor.bin with SAM-BA
* Disconnect ND+ and reconnect the USB while PRESSING THE ONBOARD BUTTON at the same time -> the onboard led should turn off and then blink (to signal that the watchdog has been turned off for this bootup)
* Deploy the firmware images using MFDeploy
* Disconnect ND+ and reconnect the USB while PRESSING THE ONBOARD BUTTON at the same time -> the onboard led should turn off and then blink
* Set the network configuration with MFDeploy 

Usage
-----

The watchdog is enabled by default in the firmware. The behaviour can be changed exactly once, 
as the watchdog control register on the AT91SAM7X512 is a write-once type register.

    Watchdog.Enabled = true;

or

    Watchdog.Enabled = false;

Once the watchdog is enabled, it must be fed by the application. If the watchdog behaviour is not set,
the watchdog is on, and is fed by the CLR.

A longer example:

    using System;
    using System.Net;
    using System.Threading;
    using Microsoft.SPOT;
    using Microsoft.SPOT.Hardware;
    using SecretLabs.NETMF.Hardware;
    using SecretLabs.NETMF.Hardware.NetduinoPlus;
    
    namespace WatchdogDemo
    {
        public class Program
        {
            static void FeedWatchdog(object o)
            {
                // Each time the watchdog is enabled, it's also fed
                // This is a bit of a workaround, but at least it didn't
                // require any interface changes
                Watchdog.Enabled = true;
            }
    
            public static void Main()
            {
                OutputPort led = new OutputPort(Pins.ONBOARD_LED, false);
    
                // When the watchdog is enabled, it also must be fed from the managed app
                Watchdog.Enabled = true;
    
                Timer WatchdogTimer = new Timer(new TimerCallback(FeedWatchdog), null, 1000, 1000);
    
                while (true)
                {
                    Thread.Sleep(50);
                    led.Write(!led.Read());
                }
            }
        }
    }
