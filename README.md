# CDIP Announcer
Provides the turnout state over loconet for Digitrax systems. I have lots of Cobalt point control motors, and JMRI simply does not know how to know the state. If you ask them to change, it catches up, but anything automated won't be tracked. The purpose here is twofold.

* Learn about Loconet and continue to grow my hobby hardware skills.
* Solve the issue where at startup JMRI may have the wrong states.

This is based on RPi 2040's using the LocoNet2 library. Included in the repo is my Arduino ino file and the KiCad artifacts.

It can handle 4 Digital IP's at a time. I chose 4 for board size, and complexity reasons. I do have 5 free inputs now on the RPi I use, but I haven't decided what, if anything,
I will do with them. They will sit idle until I know the board works.

I chose the RPi 2040 because I don't want WiFi devices all over my layout. Where the layout exists in my home makes WiFi a bit troublesome, and having another 5-10 devices
vying for bandwidth in that area will hurt the rest of the house. The downside is that you need to program each Pico independently for the address being monitored.

## KiCad design
This is a work in progress. What I've got posted now is version 2. The first board allowed me to test some things, but I made a significant error with the
LocoNet RX part of the circuit. It won't receive data, so I need to work on that part before I build it. The current schematic is what I found for use with
an ESP32 which should be identical to an RPi. The board is designed to use either and ESP32 or RPi (or any other XIAO device honestly) but I choose the RPi as
I don't want a bunch of wifi under the layout.

![image](media/board.png)

