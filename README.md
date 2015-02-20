# slight_FaderLin
arduino library for simple linear fadings (LED or other things)

the library can fade from the 'current' value to a new one in an given time.

there is an [example sketch](/examples/slight_FaderLin__test/slight_FaderLin__test.ino) that shows most of the functionality.

this library was developed for fading leds. but it might work for other thinks...

i think it is realtive stable and most of the corner cases are catched ;-) 

the library needs relative much ram - it generates interal arrays for all values..
so there is a big room to optimize this..
