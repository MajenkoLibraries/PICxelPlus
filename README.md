PICxelPlus
==========

This is a complete rewrite of the PICxel library (https://github.com/mwingerson/PICxel) 
from the ground up.  At the moment it lacks some of the more advanced functionality of the 
PICxel library (global / per pixel brightness), but it also removes all dynamic allocation 
of memory, or the need to provide your own storage arrays.

Communication is also through SPI for "lighter" operation (DMA is also planned for the future).

This version does away with the memory allocation by being a *templated* library.  Instead of
passing the number of LEDs in the constructor as a parameter, you now pass it as a template
value to the class.  For example, to run 25 pixels on the DSPI0 SPI port, you would:

    DSPI0 spi;
    PICxelPlus<25> pixels(spi);

Another change with this version compared to the original PICxel library is there is no colour mode
configuration.  You no longer have to select between GRB or HSV mode. Internally the library works
completely in GRB mode. But you can still use `pixels.HSVsetLEDColor(...)` to work with HSV
colours.  It just converts those colours for you to GRB at the moment of calling the function and
stores them as GRB.
