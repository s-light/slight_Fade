// NOLINT(legal/copyright)

/*****************************************

    slight_Fade
        concept for millis() based fading system

    written by stefan krueger (s-light),
        git@s-light.eu, http://s-light.eu, https://github.com/s-light/

    changelog / history
        05.09.2013 10:00  created.
        07.09.2013 17:00  clean up files
        16.09.2013 16:50  fading works. for one value.
        16.09.2013 23:15  multichannel support finished.
        17.09.2013 21:00  debug fade to zero bug
        18.09.2013 09:00  bug found: target = current ---> pointer!!
        18.09.2013 19:45  cleaned up a little bit its working now!
        12.11.2013 12:08  output now handled by callback function.
        12.11.2013 13:45  state events handled by callback function.
        13.11.2013 18:15  added ID
        23.11.2013 19:09  fixed getCurrentValues
        24.01.2013 11:03  changed library name to slight_Fade
        25.01.2014 15:46  added printState function
        09.03.2014 19:51  changed callback system to pInstance system.
        15.05.2014 16:06  added option to specify current and target values
outside of library (save ram)

    TODO:
        ~ test new added pointers.
        ~ develop new fading system thats resource friendly (see infos.ods)

*******************************************/
/*****************************************
    license
    CC BY SA
        This work is licensed under the
        Creative Commons Attribution-ShareAlike 3.0 Unported License.
        To view a copy of this license, visit
http://creativecommons.org/licenses/by-sa/3.0/.

    Apache License Version 2.0
        Copyright 2021 Stefan Krueger

        Licensed under the Apache License, Version 2.0 (the "License");
        you may not use this file except in compliance with the License.
        You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

        Unless required by applicable law or agreed to in writing, software
        distributed under the License is distributed on an "AS IS" BASIS,
        WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
        See the License for the specific language governing permissions and
        limitations under the License.

    The MIT License (MIT)
        Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions: The above copyright notice and
this permission notice shall be included in all copies or substantial portions
of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. http://opensource.org/licenses/mit-license.php

*******************************************/

/** Serial.print to Flash: Notepad++ Replace RegEx
    Find what:        Serial.print(.*)\("(.*)"\);
    Replace with:    Serial.print\1(F("\2"));
**/

// #define debug_slight_Fade

// include own headerfile
// NOLINTNEXTLINE(build/include)
#include "./slight_Fade.h"
// #include <slight_mapping.h>
#include <algorithm>
#include <cstdint>
// use "" for files in same directory as .ino

// ******************************************
// Constructor
// ******************************************

/** Constructor **/
// initialize chChannelCount
// http://forum.arduino.cc/index.php?topic=188261.msg1393390#msg1393390
slight_Fade::slight_Fade(
    uint8_t id_,
    tCallbackFunctionValuesChanged callbackValuesChanged_,
    tCallbackFunction callbackOnEvent_,
    tEasingFunction easing_fn_
)
    : id(id_),
      callbackValuesChanged(callbackValuesChanged_),
      callbackOnEvent(callbackOnEvent_),
      easing_fn(easing_fn_)
// NOLINTNEXTLINE(whitespace/braces)
{
    state = state_NotValid;

    position = 0.0;
    position_w_easing = 0.0;
    value_old = 0.0;
    value_current = 0.0;
    value_source = 0.0;
    value_target = 0.0;
    value_min = 0.0;
    value_max = 1.0;

    ready = false;
}

/** Destructor **/
slight_Fade::~slight_Fade() {
    //
}

/** Begin (activate object/instantce) **/
void slight_Fade::begin() {
    if (ready == false) {
        state = state_Standby;

        ready = true;
    }
}

bool slight_Fade::isReady() {
    return ready;
}

// ******************************************
// functions
// ******************************************

// / -----------------------------------------
//  Calc steps
// ------------------------------------------
bool slight_Fade::calculateValue() {
    bool flag_NewValue = 0;
    // TODO: apply easing function
    position_w_easing = easing_fn(position);
    value_current = map_range_01_to(position_w_easing, value_source, value_target);
    // check if there is a new value
    if (value_old != value_current) {
        value_old = value_current;
        flag_NewValue = 1;
    }
    return flag_NewValue;
}

void slight_Fade::fading_update() {
    fadeRuntime = (millis() - timestamp_FadeStart);
    position = normalize_to_01(millis(), timestamp_FadeStart, timestamp_FadeEnd);

    // if (millis() < timestamp_FadeEnd) {
    if (position < 1.0) {
        // Serial.print("update");
        // Serial.print(": ");
        // // Serial.print("position ");
        // Serial.print(position, 5);
        // Serial.print("; ");
        // // Serial.print("position_w_direction ");
        // Serial.print(position_w_easing, 5);
        // Serial.print("; ");
        // // Serial.print("position_w_d_easing ");
        // // Serial.print(position_w_d_easing, 5);
        // // Serial.print("; ");
        // Serial.print("old ");
        // Serial.print(value_old, 5);
        // Serial.print("; ");
        // Serial.print("calc..");
        bool flag_NewValue = calculateValue();
        // Serial.print("; ");
        // Serial.print("cur ");
        // Serial.print(value_current, 5);
        // Serial.print("; ");
        // Serial.print("old ");
        // Serial.print(value_old, 5);
        // Serial.print("; ");
        // Serial.println();
        if (flag_NewValue) {
            callbackValuesChanged(this, value_current);
        }
    } else {
        // set values to Target. Time is Over!!
        value_current = value_target;
        callbackValuesChanged(this, value_current);

        setState(state_Standby);
        flagFadingFinished = 1;
        generateEvent(event_fading_Finished);
    }
}

uint8_t slight_Fade::update() {
    if (ready == true) {
        if ((state == state_FadingUp) or (state == state_FadingDown)) {
            fading_update();
        }
    }  // end if ready
    return state;
}

// start new Fade
void slight_Fade::fadeStart() {
    if (ready == true) {
        // reset
        // fadePause();
        // Serial.print("fadeStart");
        // Serial.print(": ");
        // Serial.print("fadeDuration ");
        // Serial.print(fadeDuration);
        // Serial.print("; ");
        // Serial.print("position ");
        // Serial.print(position);
        // Serial.print("; ");
        // Serial.print("value_source ");
        // Serial.print(value_source);
        // Serial.print("; ");
        // Serial.print("value_target ");
        // Serial.print(value_target);
        // Serial.print("; ");
        // Serial.print("value_current ");
        // Serial.print(value_current);
        // Serial.println();
        // start system
        if (value_target != value_source) {
            if (value_target > value_source) {
                setState(state_FadingUp);
            } else {
                setState(state_FadingDown);
            }
            flagFadingFinished = 0;
            value_old = -1;
        } else {
            setState(state_Standby);
        }
        // just do the first calculation immediately
        update();
    }
}

void slight_Fade::fadeTo(float target, uint32_t duration, float source) {
    if (ready == true) {
        Serial.printf(
            "slight_Fade fadeTo(target %f, duration %d, source %f)\r\n",
            target,
            duration,
            source
        );
        // if (duration == NULL) {
        //     fadeDuration = fadeDurationDefault;
        // } else {
        //     fadeDuration = duration;
        // }
        fadeDuration = duration;
        if (fadeDuration == 0) {
            // fix overflow / inf / nan problems if start and end are
            // identical..
            fadeDuration = 1;
        }
        timestamp_FadeStart = millis();
        timestamp_FadeEnd = timestamp_FadeStart + fadeDuration;
        // if (source == NULL) {
        //     value_source = value_current;
        // } else {
        //     value_source = std::clamp(source, value_min, value_max);
        // }
        value_source = source;
        value_target = std::clamp(target, value_min, value_max);
        Serial.printf(
            "slight_Fade fadeTo: value_target %f    value_source %f \r\n",
            value_target,
            value_source
        );
        fadeStart();
    }
}

void slight_Fade::fadeTo(float target, uint32_t duration) {
    Serial.printf("slight_Fade fadeTo(target %f, duration %d)\r\n", target, duration);
    fadeTo(target, duration, value_current);
}
void slight_Fade::fadeTo(float target) {
    fadeTo(target, fadeDurationDefault);
}

void slight_Fade::fadeUp() {
    if (ready == true) {
        Serial.print("fadeUp ");
        Serial.print("fadeDurationDefault ");
        Serial.print(fadeDurationDefault);
        Serial.print("; ");
        Serial.print("position ");
        Serial.print(position);
        Serial.print("; ");

        uint32_t duration = fadeDurationDefault * position;
        Serial.print("duration ");
        Serial.print(duration);
        Serial.print("; ");
        duration = clamp(duration, static_cast<uint32_t>(0), fadeDurationDefault);
        Serial.print("duration ");
        Serial.print(duration);
        Serial.print("; ");
        Serial.println();
        fadeTo(value_max, duration);
    }
}

void slight_Fade::fadeDown() {
    if (ready == true) {
        Serial.print("fadeDown ");
        Serial.print("fadeDurationDefault ");
        Serial.print(fadeDurationDefault);
        Serial.print("; ");
        Serial.print("position ");
        Serial.print(position);
        Serial.print("; ");

        uint32_t duration = fadeDurationDefault * position;
        Serial.print("duration ");
        Serial.print(duration);
        Serial.print("; ");
        duration = clamp(duration, static_cast<uint32_t>(0), fadeDurationDefault);
        Serial.print("duration ");
        Serial.print(duration);
        Serial.print("; ");
        Serial.println();
        fadeTo(value_min, duration);
    }
}

void slight_Fade::fadePause() {
    if (ready == true) {
        Serial.println("slight_Fade fadePause");
        setState(state_Standby);
        update();
    }
}

void slight_Fade::fadeStop() {
    if (ready == true) {
        Serial.println("slight_Fade fadeStop");
        // if fading is active in some way..
        //  or only paused finished yet
        if ((state == state_FadingUp) or (state == state_FadingDown)
            or (!flagFadingFinished)) {
            setState(state_Standby);
            flagFadingFinished = 1;
            update();
        }
    }
}

void slight_Fade::setDurationDefault(uint32_t duration) {
    fadeDurationDefault = duration;
}

/*
uint16_t *slight_Fade::getCurrentValues() {
//uint16_t slight_Fade::getCurrentValues() {
    //return wValues_Current;
    return values_Current;
}*/

uint8_t slight_Fade::getID() {
    return id;
}

uint8_t slight_Fade::getState() {
    return state;
}

uint8_t slight_Fade::setState(uint8_t state_) {
    if (state != state_) {
        stateOld = state;
        state = state_;
        // call cbfunc for StateChange.
        generateEvent(event_StateChanged);
    }
    return state;
}

uint8_t slight_Fade::printState(Print& out) {
    switch (state) {
        case slight_Fade::state_Standby: {
            out.print(F("standby"));
        } break;
        case slight_Fade::state_NotValid: {
            out.print(F("NotValid"));
        } break;
        case slight_Fade::state_FadingUp: {
            out.print(F("fading up"));
        } break;
        case slight_Fade::state_FadingDown: {
            out.print(F("fading down"));
        } break;
        default: {
            out.print(F("error: '"));
            out.print(state);
            out.print(F(" ' is not a know state."));
        }
    }  // end switch
    return state;
}

uint8_t slight_Fade::getEventLast() {
    return eventLast;
}

uint8_t slight_Fade::printEvent(Print& out, uint8_t eventTemp) {
    switch (eventTemp) {
        case slight_Fade::event_NoEvent: {
            out.print(F("no event"));
        } break;

        case slight_Fade::event_StateChanged: {
            out.print(F("state changed"));
        } break;

        // fading
        case slight_Fade::event_fading_Finished: {
            out.print(F("fading finished"));
        } break;
        case slight_Fade::event_fading_Paused: {
            out.print(F("fading paused"));
        } break;
        case slight_Fade::event_fading_Stop: {
            out.print(F("fading stop"));
        } break;

        default: {
            out.print(F("error: '"));
            out.print(state);
            out.print(F(" ' is not a know event."));
        }
    }  // end switch
    return state;
}

uint8_t slight_Fade::printEventLast(Print& out) {
    printEvent(out, eventLast);
    return eventLast;
}

void slight_Fade::generateEvent(uint8_t eventNew) {
    event = eventNew;
    eventLast = event;
    // call event
    if (event != event_NoEvent) {
        callbackOnEvent(this);
    }
    event = event_NoEvent;
}

float slight_Fade::getValue() {
    return value_current;
}

void slight_Fade::setValue(float value_new) {
    // fadePause();
    // value_current = value_new;
    // callbackValuesChanged(this, value_current);
    fadeTo(value_new, 0);
}

void slight_Fade::setValueRange(float min_, float max_) {
    value_min = min_;
    value_max = max_;
    // TODO: define what to do if values change during standby..
    // will this generate a new current value? mapped from old valueRange to new?
}

float slight_Fade::getPosition() {
    return position;
}

// ********************************************
// THE END
// ********************************************
