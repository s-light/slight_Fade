# slight_FaderLin
arduino library for simple linear fading (LED or other things)

the library can fade from the 'current' value to a new one in an given time.

there is an [example sketch](/examples/slight_FaderLin__test/slight_FaderLin__test.ino) that shows most of the functionality.

this library was developed for fading leds. but it might work for other thinks...
i think it is relative stable and most of the corner cases are caught ;-)

the library is relative ram hungry - it generates internal arrays for all values.
so there is big room to optimize this..
