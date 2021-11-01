# OpenPTC
Open Source Protothrottle Compatible Controller

This is a simple, Protothrottle Compatible Transmitter that I have been working on.  It should work with any of the default base stations etc 
but I have only tested it with my own deadrail receiver.  For now, it is a work in progress. I am planning on a simple python/windows app to program 
the function codes for each button and switch, but for now it's somewhat hardcoded.  The code is all C, no Arduino Libraries or anything but it is
fairly simple.  There are five possible Locomotive Storage 'slots', on the faceplate there is an LED indicator for each.  There are three large buttons, I
typically set these to Function codes for the brake (red) Aux (blue) and Horn (yellow).  The row of switches, from left to right is bell, lights one, lights two and engine start.
The black push button selects the loco.  Note that these are just my personal defaults, you can set any of the buttons or switches to any function code you like.
The byte array in functions.c controls this:

    void defaultFunctionCodes()
    {
        uint8_t defaultFn[7] = { 10,   // BUTTON0 - Brake 
                                  3,   // BUTTON1 - Couplers
                                  2,   // BUTTON2 - Horn
                                  1,   // SWITCH0 - Bell
                                  0,   // SWITCH1 - Lights
                                  6,   // SWITCH2 - DitchLights
                                  8    // SWITCH3 - Engine Start
        };

        writeAllGroupDataToEEPROM(0, defaultFn);
        writeAllGroupDataToEEPROM(1, defaultFn);
        writeAllGroupDataToEEPROM(2, defaultFn);
        writeAllGroupDataToEEPROM(3, defaultFn);
        writeAllGroupDataToEEPROM(4, defaultFn);
    }

Note that this method is normally disabled, setting these values will eventually be done using a windows/python program and an Xbee/USB interface. However, for testing you can set these
values to be programmed into EEPROM by enabling this commented out call in the main code.



