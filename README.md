
### Why I have made this library?
This library is delivered with OPENSMART TFT screens.

I have added support for the ILI9326 with the ATmega1284p for my own use

### History
#### fdufnews 7/2018

Screen interface is now splitted on PORTC and PORTB

Device      |            Bus              | Bus
------------|-----------------------------|----------
ILI9326     | DB8 DB7 DB6 DB5 DB4 DB3 DB2 | DB1 DB0
ATmega1284p | PC8 PC7 PC6 PC5 PC4 PC3 PC2 | PB1 PB0

It will slow down pixel transfert a little bit but will free I²C interface wich is located on PortC D1 and D0.


#### fdufnews 2/2018

Library delivered with OPENSMART TFT screen. It seems to be a copy (with some modification) of an old version of MCUFRIEND library.
It is not the up-to-date MCUFRIEND library as the screen doesn't work with the current MCUFRIEND one.
Added support for the ILI9326 on the ATmega1284p. The data bus of the display is connected to PORTC.
 
signal | port
-------|---------
D7-0   | PORTC7-0
RD     | PORTF0
WR     | PORTF1
CD     | PORTF2 also named RS on some shields
CS     | PORTF3
RS     | not used (connected to Reset of ATmega)

----------------------------------------------------------------------------------------------------
# MCUFRIEND_kbv

Following is comment in original readme file.

Library for Uno 2.4, 2.8, 3.5, 3.6, 3.95 inch mcufriend  Shields

1. Install "Adafruit_GFX.h" library from GitHub into your User libraries folder if not already there.

2. Unzip the attached "MCUFRIEND_kbv.zip" into your User libraries folder.

3. Insert your Mcufriend style display shield into UNO.   Only 28-pin shields are supported.

4. Start your Arduino IDE.

5. Build any of the Examples from the File->Examples->Mcufriend_kbv menu.  e.g.

graphictest_kbv.ino: shows all the methods.

LCD_ID_readreg.ino:  diagnostic check to identify unsupported controllers.


HOW TO INSTALL AND USE: is now in "mcufriend_how_to.txt"

CHANGE HISTORY:         is now in "mcufriend_history.txt"
