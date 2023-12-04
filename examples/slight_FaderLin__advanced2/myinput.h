/******************************************************************************

    some description...

    libraries used:
        ~ slight_FaderLin
            written by stefan krueger (s-light),
                git@s-light.eu, http://s-light.eu, https://github.com/s-light/
            license: MIT

    written by stefan krueger (s-light),
        git@s-light.eu, http://s-light.eu, https://github.com/s-light/

******************************************************************************/
/******************************************************************************
The MIT License (MIT)

Copyright (c) 2019 Stefan Krüger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/



#ifndef myinput_H_
#define myinput_H_

// include Core Arduino functionality
#include <Arduino.h>

#include <slight_FaderLin.h>


class MyInput {
public:

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // constructor

    // MyInput(Stream &out);
    MyInput();
    ~MyInput();

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // public functions

    // basic library api
    void begin(Stream &out);
    void update();
    void end();

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // button

    void myFader_valuesChanged(slight_FaderLin *instance, uint16_t *values, uint8_t count);
    void myFader_event(slight_FaderLin *instance);

    slight_FaderLin myFader = slight_FaderLin(
        // uint8_t id_new
        LED_BUILTIN,
        // uint8_t count,
        1,
        // values changed
        // https://stackoverflow.com/questions/14189440/c-callback-using-class-member#comment110410484_14189561
        std::bind(&MyInput::myFader_valuesChanged, this, std::placeholders::_1,
                   std::placeholders::_2, std::placeholders::_3),
        // event
        std::bind(&MyInput::myFader_event, this, std::placeholders::_1));

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // configurations

private:

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // private functions

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // attributes
    bool ready;
    // Stream &out;

    uint8_t state = 0;
    uint8_t state_last = 0;

};  // class MyInput

#endif  // myinput_H_
