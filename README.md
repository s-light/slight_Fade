# slight_Fade
An Arduino library for simple linear fading (LED or other things)

the library can fade from the 'current' value to a new one in a given time.

i have implemented two version:
- the Original one `slight_FaderLin`
    - supports multiple channels with 16bit values
    - this [example sketch](/examples/slight_FaderLin__test/slight_FaderLin__test.ino) shows most of the functionality.
    - The library is relative ram hungry - it generates internal arrays for all values;
    so there is big room to optimize this..
- the New `slight_Fade`
    - the [example sketch `slight_Fade__basic.ino`](/examples/slight_Fade__basic/slight_Fade__basic.ino) shows the basic usage. see the other `slight_Fade__*` examples for extended usage..
    - currently supports 1 floating point value.
    - idea here is to use this as a base and do a maybe needed multiple channels mapping by yourself.

This library was developed for fading leds, but it might work for other things...
I think it is relative stable and most of the corner cases are caught ;-)

it is intentionally left *relative* basic.
if you need more advanced animation ques or similar have a look at  