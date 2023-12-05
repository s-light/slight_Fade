# slight_Fade
An Arduino library for simple linear fading (LED or other things)

Thw library can fade from the 'current' value to a new one in a given time.

There is an [example sketch](/examples/slight_FaderLin__test/slight_FaderLin__test.ino) that shows most of the functionality.

This library was developed for fading leds, but it might work for other things...
I think it is relative stable and most of the corner cases are caught ;-)

The library is relative ram hungry - it generates internal arrays for all values;
so there is big room to optimize this..
