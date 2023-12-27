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

#include "./mysun.h"

// include Core Arduino functionality
#include <Arduino.h>

#include <cstdint>
#include <slight_Fade.h>



#ifndef ARDUINO_ARCH_ESP32

#warning this example is partly written for ESP32 boards..

#endif




#include "esp32-hal.h"
#include "esp_private/system_internal.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// functions

MySun::MySun(Stream& out_) : out(out_) {
    ready = false;
}

MySun::~MySun() {
    end();
}

void MySun::begin() {
    // clean up..
    end();
    // start up...
    if (ready == false) {
        // setup
        out.println("MySun begin:");

        myFader.begin();

        out.println("done:");
        // enable
        ready = true;
    }
}

void MySun::end() {
    if (ready) {
        // nothing to do..
    }
}

void MySun::update() {
    if (ready) {
        handle_serial_input();
        myFader.update();
    }
}

void MySun::wait_with_update(uint32_t duration) {
    uint32_t end = millis() + duration;
    uint32_t last_action = millis();
    uint32_t update_last_action = millis();
    while (millis() < end) {
        if (millis() > last_action + 995) {
            last_action = millis();
            out.println(".");
        }
        // if (millis() > update_last_action + 100) {
        //     update_last_action = millis();
        //     myFader.update();
        // }
        myFader.update();
    }
}

// ------------------------------------------
// helpers

void MySun::reboot_to_uf2(void) {
    // https://github.com/adafruit/tinyuf2/blob/8a54e0ed4c3373765903ca6d80eeb54dbcfc54d5/ports/esp32s2/README.md#usage
    // Check out esp_reset_reason_t for other Espressif pre-defined values
    enum {
        APP_REQUEST_UF2_RESET_HINT = 0x11F2
    };

    // call esp_reset_reason() is required for idf.py to properly links
    // esp_reset_reason_set_hint()
    (void)esp_reset_reason();
    esp_reset_reason_set_hint(static_cast<esp_reset_reason_t>(APP_REQUEST_UF2_RESET_HINT
    ));
    esp_restart();
}

void MySun::handle_serial_input() {
    if (out.available() > 0) {
        char input = out.read();

        // check what to do:
        switch (input) {
            case 'r': {
                out.println("reboot_to_uf2");
                reboot_to_uf2();
            } break;
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
                out.println(" r: reboot_to_uf2");
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
// slight_Fade things

void MySun::myFader_valueChanged(slight_Fade* instance, float value) {
    analogWrite(LED_BUILTIN, value * 255);
    if (millis() > valueChanged_debugout_lastAction + 330) {
        valueChanged_debugout_lastAction = millis();
        out.println(value, 5);
    }
}

void MySun::myFader_event(slight_Fade* instance) {
    // out.print(F("instance:"));
    // out.print((*instance).getID());
    // out.print(F(" - "));
    // out.println();
    // out.print("event: ");
    // (*instance).printEventLast(Serial);
    // // out.println();
    // out.print(F(" - "));
    // out.print("state: ");
    // (*instance).printState(Serial);
    // out.println();

    switch (instance->getEventLast()) {
        case slight_Fade::event_fading_Paused:
        case slight_Fade::event_fading_Stop:
        case slight_Fade::event_fading_Finished: {
            // out.print(F("led on pin "));
            // out.print((*instance).getID());
            // out.print(F(": "));
            // out.print(F("fading Finished."));
            // out.println();
            // out.print("event: ");
            (*instance).printEventLast(Serial);
            out.println();
        } break;
        case slight_Fade::event_StateChanged: {
            out.print("state: ");
            (*instance).printState(Serial);
            out.println();
        } break;
    }  // end switch
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// THE END
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
