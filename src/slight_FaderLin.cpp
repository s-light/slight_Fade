// NOLINT(legal/copyright)

/*****************************************

    slight_FaderLin
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
        24.01.2013 11:03  changed library name to slight_FaderLin
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

// #define debug_slight_FaderLin

// include own headerfile
// NOLINTNEXTLINE(build/include)
#include "./slight_FaderLin.h"
// use "" for files in same directory as .ino

// ******************************************
// Constructor
// ******************************************

/** Constructor **/
// initialize chChannelCount
// http://forum.arduino.cc/index.php?topic=188261.msg1393390#msg1393390
slight_FaderLin::slight_FaderLin(
    uint8_t kID_New, uint8_t channelCount_New,
    tCallbackFunctionValuesChanged callbackValuesChanged_new,
    tCallbackFunction callbackOnEvent_new, uint16_t *values_Current_new,
    uint16_t *values_Target_new)
    : kID(kID_New), channelCount(channelCount_New),
      callbackValuesChanged(callbackValuesChanged_new),
      callbackOnEvent(callbackOnEvent_new)
// NOLINTNEXTLINE(whitespace/braces)
{
    // do some internal inits

    // get memory for the arrays
    /*    http://forum.arduino.cc/index.php?topic=57433.msg412702#msg412702

        After allocating the memory, you can access the memory
        as though it was an array (since it is), just like you are now doing.

        Would you be so kind as to explain what the line does?
        Sure. The malloc function allocates uint8_ts of memory, and returns a
       pointer to that memory. The type that it returns is void *, so it almost
       always needs to be cast to the correct type. In your case, the
       reinterpret_cast<uint16_t *>(xxx) cast is doing just that. Inside the
       parentheses, you need to define how many uint8_ts to allocate. The number
       of uint8_ts is defined by the number of elements that the pointer points
       to (numRings) times the number of uint8_ts in each element
       (sizeof(uint16_t)).
    */

    if (values_Current_new) {
        values_Current = values_Current_new;
    } else {
        values_Current = reinterpret_cast<uint16_t *>(
            malloc(sizeof(uint16_t) * channelCount));
    }

    if (values_Target_new) {
        values_Target = values_Target_new;
    } else {
        values_Target = reinterpret_cast<uint16_t *>(
            malloc(sizeof(uint16_t) * channelCount));
    }

    values_Source =
        reinterpret_cast<uint16_t *>(malloc(sizeof(uint16_t) * channelCount));
    values_Current =
        reinterpret_cast<uint16_t *>(malloc(sizeof(uint16_t) * channelCount));
    values_Dif =
        reinterpret_cast<uint16_t *>(malloc(sizeof(uint16_t) * channelCount));
    values_DifIsNegativ =
        reinterpret_cast<bool *>(malloc(sizeof(bool) * channelCount));

    /* //single channel version:
    wValues_Source = 0;
    wValues_Target = 0;
    wValues_Current = 0;
    wValues_Dif = 0;
    bValues_DifIsNegativ = 0;
    */

    state = state_NotValid;

    ready = false;
}

/** Destructor **/
slight_FaderLin::~slight_FaderLin() {
    //
}

/** Begin (activate object/instantce) **/
void slight_FaderLin::begin() {
    if (ready == false) {
#ifdef debug_slight_FaderLin
        // NOLINTNEXTLINE(whitespace/line_length)
        Serial.println(F("*****************************************************"
                         "**************"));
        // NOLINTNEXTLINE(whitespace/line_length)
        Serial.println(F("** Welcome to slight_FaderLin World - Library will "
                         "initialise... **"));
        // NOLINTNEXTLINE(whitespace/line_length)
        Serial.println(F("*****************************************************"
                         "**************"));
        Serial.println(F("slight_FaderLin::begin: "));
#endif

        // / -----------------------------------------
        // ??
        // ------------------------------------------

        // clean up memory.
        for (uint8_t ch_index = 0; ch_index < channelCount; ch_index++) {
            values_Source[ch_index] = 0;
            values_Target[ch_index] = 0;
            values_Current[ch_index] = 0;
            values_Dif[ch_index] = 0;
            values_DifIsNegativ[ch_index] = 0;
        }

        // single channel version:
        // wValues_Source = 0;
        // wValues_Target = 0;
        // wValues_Current = 0;
        // wValues_Dif = 0;

        state = state_Standby;

        ready = true;
    }
}

bool slight_FaderLin::isReady() { return ready; }

// ******************************************
// functions
// ******************************************

// ------------------------------------------
/**  Debug things                              **/
// ------------------------------------------

#ifdef debug_slight_FaderLin

// void slight_FaderLin::printArray(uint16_t *array) {
//     Serial.print(F(" "));
//     uint8_t bCount = channelCount; // (sizeof(*&array) / sizeof(uint16_t))
//     //Serial.print(F(" Count: "));
//     //Serial.print(bCount);
//     for(uint8_t ch_index = 0; ch_index < bCount; ch_index++){
//         Serial.print(F(" ["));
//         Serial.print(ch_index);
//         Serial.print(F("]:"));
//         Serial.print(array[ch_index]);
//     }
// }

void slight_FaderLin::printArray(uint16_t *array) {
    Serial.print(F(" "));
    uint8_t bCount = channelCount;
    uint8_t ch_index = 0;
    printuint8_tAlignRight(array[ch_index]);
    for (ch_index = 1; ch_index < bCount; ch_index++) {
        Serial.print(F(", "));
        printuint8_tAlignRight(array[ch_index]);
    }
}

void slight_FaderLin::printuint8_tAlignRight(uint8_t bValue) {
    // uint8_t bOffset = 0;
    if (bValue < 100) {
        if (bValue < 10) {
            // bOffset = 2;
            Serial.print(F("  "));
        } else {
            // bOffset = 1;
            Serial.print(F(" "));
        }
    }
    Serial.print(bValue);
}

#endif

// / -----------------------------------------
//  Calc steps
// ------------------------------------------
bool slight_FaderLin::calculate_value(uint32_t durationTemp, uint8_t ch_index) {
    bool flag_NewValues = 0;
    uint16_t valueTemp = ((values_Dif[ch_index] * durationTemp) / fadeDuration);
    // calc result value
    uint16_t value_NewCurrent = 0;
    if (values_DifIsNegativ[ch_index]) {
        value_NewCurrent = values_Source[ch_index] - valueTemp;
    } else {
        value_NewCurrent = values_Source[ch_index] + valueTemp;
    }
    // check if there is a new value
    if (value_NewCurrent != values_Current[ch_index]) {
        values_Current[ch_index] = value_NewCurrent;
        // set flag that there are new values..
        flag_NewValues = 1;
    }
    return flag_NewValues;
}

bool slight_FaderLin::calculate_values(uint32_t durationTemp) {

    /* ulDurations steht im Verhältniss zum aktuellen wert!
        gesammt dauer zu gesamt werte differenz    =    vergangene dauer zu
       aktueller wert aktueller wert = x x = (Werte Dif * vergangene dauer)
       / gesamt dauer;

        ulRatio = vergangene dauer / gesamt dauer;
        X = WertDif * ulRatio;
            // calc Ratio
            //unsigned long ulRatio = (durationTemp / fadeDuration);
        --> Geht nicht! da Ratio < 1 ist....
        use: ((wValues_Dif * durationTemp) / fadeDuration)
    */
    bool flag_NewValues = 0;

    // for every channel calc value
    for (uint8_t ch_index = 0; ch_index < channelCount; ch_index++) {
        // or assignment - this way we change only if new values..
        flag_NewValues |= calculate_value(durationTemp, ch_index);
    }

    if (flag_NewValues) {
        callbackValuesChanged(this, values_Current, channelCount);
    }
    return flag_NewValues;
}

// returns state of Fading system.
uint8_t slight_FaderLin::update() {
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
            uint32_t durationPosition = (millis() - timestamp_FadeStart);
            // if ulDuration < fadeDuration
            if (durationPosition < fadeDuration) {
                // set state to 'Fading';
                stateTemp = state_Fading;
                calculate_values(durationPosition);
            } else {
                // set values to Target. Time is Over!!
                for (uint8_t ch_index = 0; ch_index < channelCount;
                     ch_index++) {
                    // set end Values
                    values_Current[ch_index] = values_Target[ch_index];
                }

                callbackValuesChanged(this, values_Current, channelCount);

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
            state = stateTemp;
            // call cbfunc for StateChange.
            generateEvent(event_StateChanged);
        }
    } // end if ready
    return state;
}

void slight_FaderLin::startFade_setupValues(uint16_t *values_NewTarget,
                                            uint16_t *values_uCurrent) {
    for (uint8_t ch_index = 0; ch_index < channelCount; ch_index++) {
        // set Source values
        if (values_uCurrent) {
            // use user given start values
            values_Source[ch_index] = values_uCurrent[ch_index];
        } else {
            values_Source[ch_index] = values_Current[ch_index];
        }
        // set Target Values
        values_Target[ch_index] = values_NewTarget[ch_index];
        // build Dif
        if (values_Target[ch_index] > values_Source[ch_index]) {
            values_Dif[ch_index] =
                values_Target[ch_index] - values_Source[ch_index];
            values_DifIsNegativ[ch_index] = false;
        } else {
            values_Dif[ch_index] =
                values_Source[ch_index] - values_Target[ch_index];
            values_DifIsNegativ[ch_index] = true;
        }
    }
}

// start new Fade
void slight_FaderLin::startFade() {
    if (ready == true) {
        stopFade();

        timestamp_FadeStart = millis();
        // start system
        flagFadingFinished = 0;
        Active = 1;
        // just do the first calculation immediately
        update();
    }
}

void slight_FaderLin::startFadeTo(uint32_t fadeDuration_New,
                                  uint16_t *values_NewTarget,
                                  uint16_t *values_uCurrent) {
    if (ready == true) {
        fadeDuration = fadeDuration_New;
        startFade_setupValues(values_NewTarget, values_uCurrent);
        startFade();
    }
}

void slight_FaderLin::startFadeTo(uint32_t fadeDuration_New,
                                  uint16_t value_NewTarget) {
    if (ready == true) {
        uint16_t temp_array[channelCount];
        for (size_t i = 0; i < channelCount; i++) {
            temp_array[i] = value_NewTarget;
        }
        startFadeTo(fadeDuration_New, temp_array);
    }
}

void slight_FaderLin::startFadeTo(uint32_t fadeDuration_New,
                                  uint16_t value_NewTarget,
                                  uint16_t value_uCurrent) {
    if (ready == true) {
        uint16_t temp_array[channelCount];
        for (size_t i = 0; i < channelCount; i++) {
            temp_array[i] = value_NewTarget;
        }
        uint16_t temp_array2[channelCount];
        for (size_t i = 0; i < channelCount; i++) {
            temp_array2[i] = value_uCurrent;
        }
        startFadeTo(fadeDuration_New, temp_array, temp_array2);
    }
}

void slight_FaderLin::stopFade() {
    if (ready == true) {
        // if fading is active in some way or not finished yet
        if ((!flagFadingFinished) || (Active)) {
            // Stop Fading
            Active = 0;
            // reset variables.
            flagFadingFinished = 1;
            update();
        }
    }
}

void slight_FaderLin::fadeUp() {
    if (ready == true) {
    }
}

void slight_FaderLin::fadeDown() {
    if (ready == true) {
    }
}

void slight_FaderLin::setDuration(uint32_t duration) {
    fadeDurationDefault = duration;
}

/*
uint16_t *slight_FaderLin::getCurrentValues() {
//uint16_t slight_FaderLin::getCurrentValues() {
    //return wValues_Current;
    return values_Current;
}*/

uint8_t slight_FaderLin::getID() { return kID; }

uint8_t slight_FaderLin::getState() { return state; }

uint8_t slight_FaderLin::printState(Print &out) {
    switch (state) {
    case slight_FaderLin::state_Standby: {
        out.print(F("standby"));
    } break;
    case slight_FaderLin::state_NotValid: {
        out.print(F("NotValid"));
    } break;
    case slight_FaderLin::state_Fading: {
        out.print(F("fading"));
    } break;
    default: {
        out.print(F("error: '"));
        out.print(state);
        out.print(F(" ' is not a know state."));
    }
    } // end switch
    return state;
}

uint8_t slight_FaderLin::getEventLast() { return eventLast; }

uint8_t slight_FaderLin::printEvent(Print &out, uint8_t eventTemp) {
    switch (eventTemp) {
    case slight_FaderLin::event_NoEvent: {
        out.print(F("no event"));
    } break;

    case slight_FaderLin::event_StateChanged: {
        out.print(F("state changed"));
    } break;

    // fading
    case slight_FaderLin::event_fading_Finished: {
        out.print(F("fading finished"));
    } break;
    case slight_FaderLin::event_fading_Stopped: {
        out.print(F("fading stoped"));
    } break;

    default: {
        out.print(F("error: '"));
        out.print(state);
        out.print(F(" ' is not a know event."));
    }
    } // end switch
    return state;
}

uint8_t slight_FaderLin::printEventLast(Print &out) {
    printEvent(out, eventLast);
    return eventLast;
}

void slight_FaderLin::generateEvent(uint8_t eventNew) {
    event = eventNew;
    eventLast = event;
    // call event
    if (event != event_NoEvent) {
        callbackOnEvent(this);
    }
    event = event_NoEvent;
}

uint8_t slight_FaderLin::getChannelCount() { return channelCount; }

void slight_FaderLin::getCurrentValues(uint16_t *pToArray) {
    /*Serial.print(F("values_Current[0]: "));
    Serial.println(values_Current[0]);
    for (uint8_t ch_index = 0; ch_index < channelCount; ch_index++) {
        pToArray[ch_index] = values_Current[ch_index];
    }*/
    memcpy(pToArray, values_Current, channelCount * sizeof(word));
    /*Serial.print(F("pToArray[0]: "));
    Serial.println(pToArray[0]);*/
}

// ********************************************
// THE END
// ********************************************
