Netduino Plus with HW watchdog
==============================

This is the Netduino Plus (1) firmware 4.2.0.1 source with CW2's 1-wire extensions and hardware watchdog. 
The binaries are available here: [http://forums.netduino.com/index.php?/topic/6968-hardware-watchdog-on-netduino-plus/?p=46768](http://forums.netduino.com/index.php?/topic/6968-hardware-watchdog-on-netduino-plus/?p=46768)


Build
-----
Build command with RVDS 3.1 installed:

    call setenv_rvds.cmd 3.1
    msbuild solutions\netduinoplus\dotnetmf.proj /p:flavor=release;tcp_ip_stack=lwip /m:2

Installation
------------
* Install the TinyBooterDecompressor.bin with SAM-BA
* Disconnect ND+ and reconnect while PRESSING THE ONBOARD BUTTON at the same time -> the onboard led should turn off (to signal that the watchdog has been turned off for this bootup)
* Deploy the firmware images using MFDeploy

Usage
-----

The watchdog is enabled by default in the firmware. The behaviour can be changed exactly once, 
as the watchdog control register on the AT91SAM7X512 is a write-once type register.

    Watchdog.Enabled = true;

or

    Watchdog.Enabled = false;

A longer example:

    using System;
	using System.Net;
	using System.Threading;
	using System.Text;
	using Microsoft.SPOT;
	using Microsoft.SPOT.Hardware;
	using SecretLabs.NETMF.Hardware;
	using SecretLabs.NETMF.Hardware.NetduinoPlus;
	
	namespace NetduinoPlusApplication1
	{
		public class Program
		{
	    	public static void Main()
	    	{
	        	int i = 0;
	        	int delay = 250;
	        	OutputPort led = new OutputPort(Pins.ONBOARD_LED, false);
	
	        	Watchdog.Enabled = true;
	//      	Watchdog.Enabled = false;   // If the watchdog is disabled, the software will just get stuck
	
	        	while (true)
	        	{
	            	Debug.Print("Round " + i++);
	            	Debug.Print("Watchdog enabled: " + Watchdog.Enabled.ToString());
	          	
	            	led.Write(true);
	
	            	Thread.Sleep(delay);
	            	led.Write(false);
	            	Thread.Sleep(delay);
	
	            	// This simulates the case when the software gets stuck
	            	if (i > 20)
	            	{
	                	TimeSpan foo =  Watchdog.Timeout;
	            	}
	        	}
	    	}
		}
	}
