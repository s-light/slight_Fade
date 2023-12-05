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

    TO DO:
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
// use "" for files in same directory as .ino

// ******************************************
// Constructor
// ******************************************

/** Constructor **/
// initialize chChannelCount
// http://forum.arduino.cc/index.php?topic=188261.msg1393390#msg1393390
slight_Fade::slight_Fade(
    uint8_t id_new,
    tCallbackFunctionValuesChanged callbackValuesChanged_new,
    tCallbackFunction callbackOnEvent_new)
    : id(id_new),
      callbackValuesChanged(callbackValuesChanged_new),
      callbackOnEvent(callbackOnEvent_new)
// NOLINTNEXTLINE(whitespace/braces)
{
    state = state_NotValid;

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

bool slight_Fade::isReady() { return ready; }

// ******************************************
// functions
// ******************************************

// / -----------------------------------------
//  Calc steps
// ------------------------------------------
bool slight_Fade::calculateValue() {
    bool flag_NewValue = 0;
    // check if there is a new value
    if (position != value_current) {
        value_current = position;
        flag_NewValue = 1;
    }
    return flag_NewValue;
}

// returns state of Fading system.
uint8_t slight_Fade::update() {
    /** Fading System State
        state_Standby                = 0;
        state_Finished                = 1;
        state_Fading                = 2;
        state_Fading_newValues        = 10;
    **/
    uint8_t stateTemp = state_NotValid;
    if (ready == true) {
        // if fading is active calc steps
        if (Active) {
            // calc duration since FadeStart
            fadeRuntime = (millis() - timestamp_FadeStart);
            position = normalize_to_01(millis(), timestamp_FadeStart,
                                       timestamp_FadeEnd);

            // if (position < 1.0) {
            if (millis() < timestamp_FadeEnd) {
                bool flag_NewValue = calculateValue();
                if (flag_NewValue) {
                    callbackValuesChanged(this, value_current);
                }
            } else {
                // set values to Target. Time is Over!!
                value_current = value_target;
                callbackValuesChanged(this, value_current);

                stateTemp = state_Standby;

                // set finished flag!
                Active = 0;
                flagFadingFinished = 1;
                generateEvent(event_fading_Finished);
            }
        } else {
            // nothing to do - so its Standby.
            stateTemp = state_Standby;
        }
        // check if State has Changed.
        if (state != stateTemp) {
            stateOld = state;
            state = stateTemp;
            // call cbfunc for StateChange.
            generateEvent(event_StateChanged);
        }
    } // end if ready
    return state;
}

// start new Fade
void slight_Fade::fadeStart() {
    if (ready == true) {
        fadePause();
        // start system
        uint8_t stateTemp = state_NotValid;
        if (value_target > value_current) {
            stateTemp = state_FadingUp;
        } else {
            stateTemp = state_FadingDown;
        }
        if (state != stateTemp) {
            stateOld = state;
            state = stateTemp;
            // call cbfunc for StateChange.
            generateEvent(event_StateChanged);
        }
        flagFadingFinished = 0;
        Active = 1;
        // just do the first calculation immediately
        update();
    }
}

void slight_Fade::fadeTo(uint32_t duration, float target) {
    if (ready == true) {
        fadeDuration = duration;
        timestamp_FadeStart = millis();
        timestamp_FadeEnd = timestamp_FadeStart + fadeDuration;
        value_target = target;
        fadeStart();
    }
}

void slight_Fade::fadeUp() {
    if (ready == true) {
        uint32_t duration = fadeDurationDefault * (1.0 - position);
        fadeTo(duration, 1.0);
    }
}

void slight_Fade::fadeDown() {
    if (ready == true) {
        uint32_t duration = fadeDurationDefault * position;
        fadeTo(duration, 0.0);
    }
}

void slight_Fade::fadePause() {
    if (ready == true) {
        // pause Fading
        Active = 0;
        update();
    }
}

void slight_Fade::fadeStop() {
    if (ready == true) {
        // if fading is active in some way or not finished yet
        if ((!flagFadingFinished) || (Active)) {
            // pause Fading
            Active = 0;
            // reset variables.
            flagFadingFinished = 1;
            update();
        }
    }
}

void slight_Fade::setDuration(uint32_t duration) {
    fadeDurationDefault = duration;
}

/*
uint16_t *slight_Fade::getCurrentValues() {
//uint16_t slight_Fade::getCurrentValues() {
    //return wValues_Current;
    return values_Current;
}*/

uint8_t slight_Fade::getID() { return id; }

uint8_t slight_Fade::getState() { return state; }

// uint8_t slight_Fade::setState(uint8_t state_new) { state_new;
// return state; }

uint8_t slight_Fade::printState(Print &out) {
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
    } // end switch
    return state;
}

uint8_t slight_Fade::getEventLast() { return eventLast; }

uint8_t slight_Fade::printEvent(Print &out, uint8_t eventTemp) {
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
    } // end switch
    return state;
}

uint8_t slight_Fade::printEventLast(Print &out) {
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

float slight_Fade::getValue() { return value_current; }

void slight_Fade::setValue(float value_new) {
    value_current = value_new;
    // flagFadingFinished = 1;
    update();
}

// ********************************************
// THE END
// ********************************************
