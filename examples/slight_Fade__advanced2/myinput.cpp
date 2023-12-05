// NOLINT(legal/copyright)
/******************************************************************************

    some description...

    libraries used:
        ~ slight_Fade
            written by stefan krueger (s-light),
                git@s-light.eu, http://s-light.eu, https://github.com/s-light/
            license: MIT

    written by stefan krueger (s-light),
        git@s-light.eu, http://s-light.eu, https://github.com/s-light/

******************************************************************************/
/******************************************************************************
The MIT License (MIT)

Copyright (c) 2023 Stefan Krüger

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

// include Core Arduino functionality
#include <Arduino.h>

#include <slight_Fade.h>

// include own headerfile
// NOLINTNEXTLINE(build/include)
#include "./myinput.h"

// this pollutes the global namespace..
// using namespace std::placeholders;  // for `_1`
// so we use std::placeholders::_1

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// functions

// MyInput::MyInput(Stream &out): out(out) {
MyInput::MyInput() { ready = false; }

MyInput::~MyInput() { end(); }

void MyInput::begin(Stream &out) {
    // clean up..
    end();
    // start up...
    if (ready == false) {
        // setup
        out.println("MyInput begin:");

        myFader.begin();

        out.println("done:");
        // enable
        ready = true;
    }
}

void MyInput::end() {
    if (ready) {
        // nothing to do..
    }
}

void MyInput::update() {
    if (ready) {
        myFader.update();
    }
}

// ------------------------------------------
// slight_Fade things

void MyInput::myFader_valuesChanged(slight_Fade *instance, float value) {
    analogWrite(LED_BUILTIN, value * 255);
}

void MyInput::myFader_event(slight_Fade *instance) {
    Serial.print(F("instance:"));
    Serial.print((*instance).getID());
    Serial.print(F(" - event: "));
    (*instance).printEventLast(Serial);
    Serial.println();

    // react on events:
    switch (instance->getEventLast()) {
    case slight_Fade::event_fading_Finished: {
        Serial.print(F("led on pin "));
        Serial.print((*instance).getID());
        Serial.print(F(": fading Finished."));
        Serial.println();
    } break;
    } // end switch
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// THE END
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
