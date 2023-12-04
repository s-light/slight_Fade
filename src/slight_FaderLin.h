/**************************************************************************************************
    slight_FaderLin Library
        for more information changelog / history see slight_FaderLin.cpp
    written by stefan krueger (s-light),
        git@s-light.eu, http://s-light.eu, https://github.com/s-light/
**************************************************************************************************/

/******************************************
license

Copyright 2019 Stefan Krueger

------------------------------------------
Apache License Version 2.0

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

------------------------------------------
The MIT License (MIT)

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

// Check if this class is allready there or if it must be created...
#ifndef slight_FaderLin_h
#define slight_FaderLin_h

/** Includes Core Arduino functionality **/
#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

// experiments with std:function
// https://stackoverflow.com/questions/14189440/c-callback-using-class-member#14189561
// more on this topic at
// https://github.com/arduino/ArduinoCore-avr/pull/58
#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_ESP32)
// fix  "error: macro "min" passed 3 arguments, but takes just 2"
#undef min
#undef max
// fix
// undefined reference to `std::__throw_bad_function_call()'
// found at
// https://forum.arduino.cc/index.php?topic=382211.msg2790687#msg2790687
// namespace std {
//     void __throw_bad_function_call() {
//         Serial.println(F("STL ERROR - __throw_bad_function_call"));
//     }
// }
// but results in
// warning: 'noreturn' function does return [enabled by default
// and
// multiple definition of `std::__throw_bad_function_call()'
// if we move this to the main .ino file it works...
// → please include slight_ButtonInput_CallbackHelper.h from the main sketch.
#include <functional>
#endif

/** Class Definition: **/

class slight_FaderLin {
public:
    // public definitions:

    // init
    static const uint8_t event_NoEvent = 0;

    // State
    static const uint8_t event_StateChanged = 10;

    static const uint8_t state_NotValid = 10;
    static const uint8_t state_Standby = 11;
    static const uint8_t state_Fading = 12;

    static const uint8_t event_fading_Stopped = 30;

    static const uint8_t event_fading_Finished = 42;

// call back functions
#if defined(ARDUINO_ARCH_AVR)
    using tCallbackFunctionValuesChanged = void (*)(
        slight_FaderLin *instance, uint16_t *values, uint8_t count);
    using tCallbackFunction = void (*)(slight_FaderLin *instance);
#elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_ESP32)
    using tCallbackFunctionValuesChanged = std::function<void(
        slight_FaderLin *instance, uint16_t *values, uint8_t count)>;
    using tCallbackFunction = std::function<void(slight_FaderLin *instance)>;
#else
#error Not implemented yet. please create a pull-request :-)
#endif

    // public methods

    // Constructor
    // slight_FaderLin(uint8_t channelCount_Temp, tCallbackFunctionValuesChanged
    // cbfuncValuesChanged_Temp, tCbfuncStateChanged cbfuncStateChanged_Temp);
    // slight_FaderLin(uint8_t kID_New, uint8_t channelCount_New,
    // tCallbackFunctionValuesChanged cbfuncValuesChanged_New, tCbfuncStateChanged
    // cbfuncStateChanged_New);
    slight_FaderLin(uint8_t kID_New, uint8_t channelCount_New,
                    tCallbackFunctionValuesChanged callbackValuesChanged_New,
                    tCallbackFunction callbackOnEvent_new,
                    uint16_t *values_Current_new = NULL,
                    uint16_t *values_Target_new = NULL);

    // Destructor
    ~slight_FaderLin();

    // initialize class
    void begin();

    // check if class is ready to operate.
    bool isReady();

    // getID
    uint8_t getID();

    // getState
    uint8_t getState();
    uint8_t printState(Print &out);

    // event
    uint8_t getEventLast();
    uint8_t printEventLast(Print &out);
    uint8_t printEvent(Print &out, uint8_t stateTemp);

    // update
    uint8_t update();

    // fader things
    void startFadeTo(uint32_t fadeDuration_New, uint16_t value_NewTarget);
    void startFadeTo(uint32_t fadeDuration_New, uint16_t value_NewTarget, uint16_t value_uCurrent);
    // void startFadeTo(uint32_t fadeDuration_New, uint16_t *values_NewTarget);
    void startFadeTo(uint32_t fadeDuration_New, uint16_t *values_NewTarget, uint16_t *values_uCurrent = NULL);
    void stopFade();

    // uint16_t *getCurrentValues();
    // uint16_t getCurrentValues();
    void getCurrentValues(uint16_t *pToArray);

    // const uint8_t getChannelCount();
    uint8_t getChannelCount();

// debug helpers
#ifdef debug_slight_FaderLin
    void printBinary8(uint8_t bIn);
    void printuint8_tAlignRight(uint8_t bValue);
#endif

    uint8_t getBitPos(uint8_t bValue);

private:
    // per object data

    // ID
    const uint8_t kID;

    // flag to check if the begin function is already called and the class is
    // ready to work.
    bool ready;

    uint8_t state;

    uint8_t event;
    uint8_t eventLast;

    // channels:
    const uint8_t channelCount;

    // call back functions:
    const tCallbackFunction callbackOnEvent;
    const tCallbackFunctionValuesChanged callbackValuesChanged;
    // const tCallbackFunction callbackValuesChanged;

    bool flagFadingFinished;
    bool Active;

    // uint32_t ulTimeStamp_LastStep;
    uint32_t timestamp_FadeStart;
    uint32_t fadeDuration;

    // http://forum.arduino.cc/index.php?topic=57433.msg412702#msg412702
    uint16_t *values_Source;
    uint16_t *values_Target;
    uint16_t *values_Current;
    uint16_t *values_Dif;
    bool *values_DifIsNegativ;

    bool calculate_value(uint32_t ulDurationTemp,
                                          uint8_t ch_index);
    bool calculate_values(uint32_t ulDurationTemp);

// for testing --> SingleChannel Version
/*
uint16_t values_Source;
uint16_t values_Target;
uint16_t values_Current;
uint16_t values_Dif;
bool bValues_DifIsNegativ;
*/

// private methods
#ifdef debug_slight_FaderLin
    void printArray(uint16_t *array);
#endif

    void generateEvent(uint8_t eventNew);
    };

#endif // ifndef slight_FaderLin_h

/** the end **/
