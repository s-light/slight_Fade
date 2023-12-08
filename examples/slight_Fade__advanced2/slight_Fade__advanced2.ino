/******************************************

    slight_FaderLin__advanced2.ino
        minimal example for slight_Fader lib usage.
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

#include "esp32-hal.h"
#include "esp_private/system_internal.h"

#include "myinput.h"

// MyInput myinput = MyInput(Serial);
MyInput myinput = MyInput();

uint32_t last_action = millis();
uint32_t update_last_action = millis();

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
    Serial.println(F("slight_FaderLin__advanced2.ino"));
    Serial.println(F("example for library usage."));

    // ------------------------------------------
    myinput.begin(Serial);

    myinput.myFader.setDurationDefault(5 * 1000);

    // ------------------------------------------
    Serial.println(F("Loop:"));
}

void reboot_to_uf2(void) {
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

void handle_serial_input() {
    if (Serial.available() > 0) {
        char input = Serial.read();

        // check what to do:
        switch (input) {
            case 'r': {
                Serial.println("reboot_to_uf2");
                reboot_to_uf2();
            } break;
            case 't': {
                Serial.println("run_tests");
                run_tests();
            } break;
            case '1': {
                Serial.println("fadeTo 0.5 in 0s");
                myinput.myFader.fadeTo(0.5, 0 * 1000);
            } break;
            case '2': {
                Serial.println("fadeTo 0.1 in 10s");
                myinput.myFader.fadeTo(0.1, 10 * 1000);
            } break;
            case '3': {
                Serial.println("fadeTo 0.8 in 5s");
                myinput.myFader.fadeTo(0.8, 5 * 1000);
            } break;
            default: {
                Serial.println("options:");
                Serial.println(" r: reboot_to_uf2");
                Serial.println(" t: run_tests");
                Serial.println(" 1: fadeTo 0.5 in 0s");
                Serial.println(" 2: fadeTo 0.1 in 10s");
                Serial.println(" 3: fadeTo 0.8 in 5s");
            }
        }

        // clear input buffer
        while (Serial.available()) {
            Serial.read();
        }
    }
}

void run_tests() {
    myinput.update();

    Serial.println();
    Serial.println("run tests:");
    // just do a bunch of test fades

    // Serial.println("fadeTo 0.5 in 3s");
    // myinput.myFader.fadeTo(0.5, 3 * 1000);
    // myinput.wait_with_update(6 * 1000);

    // Serial.println("fadeTo 0 in 4s");
    // myinput.myFader.fadeTo(0, 4 * 1000);
    // myinput.wait_with_update(10 * 1000);

    // Serial.println("fadeUp");
    // myinput.myFader.fadeUp();
    // myinput.wait_with_update(4 * 1000);
    // Serial.println("fadePause");
    // myinput.myFader.fadePause();
    // Serial.print("currentValue: ");
    // Serial.println(myinput.myFader.getValue());

    // myinput.wait_with_update(2 * 1000);

    // Serial.println("fadeDown");
    // myinput.myFader.fadeDown();
    // myinput.wait_with_update(3 * 1000);
    // Serial.println("fadePause");
    // myinput.myFader.fadePause();
    // Serial.println(myinput.myFader.getValue());

    myinput.wait_with_update(2 * 1000);

    Serial.println("fadeTo 0.8 in 0s");
    myinput.myFader.fadeTo(0.8, 0 * 1000);
    myinput.wait_with_update(4 * 1000);
    Serial.println(myinput.myFader.getValue());

    myinput.wait_with_update(2 * 1000);

    // Serial.println("fadeUp");
    // myinput.myFader.fadeUp();
    // myinput.wait_with_update(2 * 1000);
    // Serial.println("fadePause");
    // myinput.myFader.fadePause();
    // Serial.print("currentValue: ");
    // Serial.println(myinput.myFader.getValue());

    Serial.println("fadeTo 0.5 in 0s");
    myinput.myFader.fadeTo(0.5, 0 * 1000);
    myinput.wait_with_update(4 * 1000);
    Serial.println(myinput.myFader.getValue());

    // Serial.println("fadeDown");
    // myinput.myFader.fadeDown();
    // myinput.wait_with_update(3 * 1000);
    // Serial.println("fadePause");
    // myinput.myFader.fadePause();
    // Serial.println(myinput.myFader.getValue());

    // myinput.wait_with_update(2 * 1000);

    // Serial.println("fadeUp");
    // myinput.myFader.fadeUp();
    // myinput.wait_with_update(2 * 1000);
    // Serial.println("fadePause");
    // myinput.myFader.fadePause();
    // Serial.print("currentValue: ");
    // Serial.println(myinput.myFader.getValue());

    Serial.println("done.");
    Serial.println();
    Serial.println();
    Serial.println();
    delay(10 * 1000);
}

// ------------------------------------------
void loop() {
    handle_serial_input();

    if (millis() > last_action + 999) {
        last_action = millis();
        Serial.println(millis());
    }

    // if (millis() > update_last_action + 100) {
    //     update_last_action = millis();
    //     myinput.myFader.update();
    // }
    myinput.myFader.update();
}

// ------------------------------------------
// THE END :-)
// ------------------------------------------
