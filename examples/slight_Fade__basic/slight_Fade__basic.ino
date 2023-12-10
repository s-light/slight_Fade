/******************************************

    slight_FaderLin__basic.ino
        basic example for slight_Fader lib usage.
        debugport: serial interface 115200baud

    hardware:
        Arduino board of any typ
        LED_BUILTIN

    libraries used:
        ~ slight_Fader

    written by stefan krueger (s-light),
        git@s-light.eu, http://s-light.eu, https://github.com/s-light/

******************************************/
/******************************************

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
https://opensource.org/licenses/mit-license.php
******************************************/

#include <Arduino.h>

#include <slight_Fade.h>

// ------------------------------------------
// slight_Fade things

uint32_t valueChanged_debugout_lastAction;

void myFader_valueChanged(slight_Fade* instance, float value) {
    analogWrite(LED_BUILTIN, value * 255);
    if (millis() > valueChanged_debugout_lastAction + 330) {
        valueChanged_debugout_lastAction = millis();
        Serial.println(value, 5);
    }
}

void myFader_event(slight_Fade* instance) {
    Serial.print(F("instance:"));
    Serial.print((*instance).getID());
    // Serial.print(F(" - "));
    Serial.println();
    // Serial.print("event: ");
    // (*instance).printEventLast(Serial);
    // // Serial.println();
    // Serial.print(F(" - "));
    // Serial.print("state: ");
    // (*instance).printState(Serial);
    // Serial.println();

    switch (instance->getEventLast()) {
        case slight_Fade::event_fading_Paused:
        case slight_Fade::event_fading_Stop:
        case slight_Fade::event_fading_Finished: {
            (*instance).printEventLast(Serial);
            Serial.println();
        } break;
        case slight_Fade::event_StateChanged: {
            Serial.print("state: ");
            (*instance).printState(Serial);
            Serial.println();
        } break;
    }  // end switch
}

// this way we use the easeLinear default.
// slight_Fade myFaderLinear = slight_Fade(
//     0, // id
//     myFader_valueChanged,
//     myFader_event
// );

// use one of the easing functions from
// https://github.com/s-light/slight_easing/
// a visual helper can be found at
// https://easings.net/

slight_Fade myFader = slight_Fade(
    0,  // id
    myFader_valueChanged,
    myFader_event,
    easeCubicIn
);

// ------------------------------------------
// helpers
// ------------------------------------------

uint32_t liveSign_lastAction;

void handle_serial_input(Stream& out) {
    if (out.available() > 0) {
        char input = out.read();

        // check what to do:
        switch (input) {
            case 'u': {
                out.println("fadeUp");
                myFader.fadeUp();
            } break;
            case 'd': {
                out.println("fadeDown");
                myFader.fadeDown();
            } break;
            case 's': {
                out.println("fadePause");
                myFader.fadePause();
            } break;
            case '1': {
                out.println("fadeTo 0.5 in 0s");
                myFader.fadeTo(0.5, 0 * 1000);
            } break;
            case '2': {
                out.println("fadeTo 0.1 in 10s");
                myFader.fadeTo(0.1, 10 * 1000);
            } break;
            case '3': {
                out.println("fadeTo 0.8 in 5s");
                myFader.fadeTo(0.8, 5 * 1000);
            } break;
            default: {
                out.println("options:");
                out.println(" u: fadeUp");
                out.println(" d: fadeDown");
                out.println(" s: fadePause");
                out.println(" 1: fadeTo 0.5 in 0s");
                out.println(" 2: fadeTo 0.1 in 10s");
                out.println(" 3: fadeTo 0.8 in 5s");
            }
        }

        // clear input buffer
        while (out.available()) {
            out.read();
        }
    }
}

// ------------------------------------------
// main
// ------------------------------------------
void setup() {
    // ------------------------------------------
    // init serial
    // wait for arduino IDE to release all serial ports after upload.
    delay(1000);
    Serial.begin(115200);
    delay(500);
    Serial.println();

    // ------------------------------------------
    // print short welcome text
    Serial.println(F("slight_FaderLin__basic.ino"));
    Serial.println(F("example for library usage."));

    // ------------------------------------------
    myFader.begin();
    myFader.setDurationDefault(5 * 1000);
    pinMode(LED_BUILTIN, OUTPUT);

    // ------------------------------------------
    Serial.println(F("Loop:"));
}

// ------------------------------------------
void loop() {
    handle_serial_input(Serial);
    myFader.update();

    if (millis() > liveSign_lastAction + 999) {
        liveSign_lastAction = millis();
        Serial.println(millis());
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// THE END
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
