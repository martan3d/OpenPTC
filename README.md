# OpenPTC
Open Source Protothrottle Compatible Controller

This is a simple, Protothrottle Compatible Transmitter that I have been working on.  It should work with any of the default base stations,  but I have only tested it with my own deadrail receiver.  

For now, it is a work in progress. I am planning on a simple python/windows app to program the function codes for each button and switch, but at the moment it is hardcoded, although easy to change.  

The code is all C, no Arduino Libraries are used but it is fairly simple.  There are five possible Locomotive Storage 'slots', on the faceplate there is an LED indicator for each.  There are three large buttons, I typically set these to Function codes for the Brake (red) Aux (blue) and Horn (yellow).  There are four programmable switches on the bottom, I generally set these to bell, lights, ditch lights and engine start/stop but these are just my personal defaults, you can set any of the buttons or switches to any function code you like.  The settings are stored in EEPROM and the code is setup to send and receive the configuration messages via Xbee Directed messages, however at this point I have not tested them.




