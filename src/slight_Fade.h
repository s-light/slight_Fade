/**************************************************************************************************
    slight_Fade Library
        for more information changelog / history see slight_Fade.cpp
    written by stefan krueger (s-light),
        git@s-light.eu, http://s-light.eu, https://github.com/s-light/
**************************************************************************************************/

/******************************************
license

Copyright 2023 Stefan Krueger

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

// Check if this class is already there or if it must be created...
#ifndef slight_Fade_h
    #define slight_Fade_h

    /** Includes Core Arduino functionality **/
    #include <Arduino.h>

    #include <slight_easing.h>
    #include <slight_mapping.h>

    #if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_ESP32)
        // fix  "error: macro "min" passed 3 arguments, but takes just 2"
        #undef min
        #undef max
        #include <functional>
    #endif

class slight_Fade {
public:
    // public definitions:

    // init
    static const uint8_t event_NoEvent = 0;

    // State
    static const uint8_t event_StateChanged = 10;

    static const uint8_t state_NotValid = 10;
    static const uint8_t state_Standby = 11;
    static const uint8_t state_FadingUp = 12;
    static const uint8_t state_FadingDown = 13;

    static const uint8_t event_fading_Paused = 30;
    static const uint8_t event_fading_Stop = 32;

    static const uint8_t event_fading_Finished = 42;

    // call back functions
    #if defined(ARDUINO_ARCH_AVR)
    using tCallbackFunctionValuesChanged =
        void (*)(slight_Fade* instance, float value, uint8_t count);
    using tCallbackFunction = void (*)(slight_Fade* instance);
    #elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_ESP32)
    using tCallbackFunctionValuesChanged =
        std::function<void(slight_Fade* instance, float value)>;
    using tCallbackFunction = std::function<void(slight_Fade* instance)>;
    #else
        #error Not implemented yet. please create a pull-request :-)
    #endif

    // Constructor
    slight_Fade(
        uint8_t id_,
        tCallbackFunctionValuesChanged callbackValuesChanged_,
        tCallbackFunction callbackOnEvent_,
        tEasingFunction easing_fn_ = easeLinear
    );

    // Destructor
    ~slight_Fade();

    // class basics
    void begin();
    uint8_t update();

    // check if class is ready to operate.
    bool isReady();

    // getID
    uint8_t getID();

    // getState
    uint8_t getState();
    uint8_t printState(Print& out);

    // event
    uint8_t getEventLast();
    uint8_t printEventLast(Print& out);
    uint8_t printEvent(Print& out, uint8_t stateTemp);

    // fader things
    void fadeTo(float target, uint32_t duration, float source);
    void fadeTo(float target, uint32_t duration);
    void fadeTo(float target);
    void fadeUp();
    void fadeDown();
    void fadeStart();
    void fadePause();
    void fadeStop();

    float getValue();
    void setValue(float value);

    uint8_t getChannelCount();

    void setDurationDefault(uint32_t duration);

private:
    const uint8_t id;

    // flag to check if the begin function is already called and the class is
    // ready to work.
    bool ready;

    uint8_t state;
    uint8_t stateOld;
    uint8_t setState(uint8_t state_);

    uint8_t event;
    uint8_t eventLast;

    // call back functions:
    const tCallbackFunction callbackOnEvent;
    const tCallbackFunctionValuesChanged callbackValuesChanged;

    bool flagFadingFinished;
    bool Active;

    uint32_t timestamp_FadeStart;
    uint32_t timestamp_FadeEnd;
    uint32_t fadeRuntime;
    uint32_t fadeDuration;
    uint32_t fadeDurationDefault;

    float position = 0.0;
    float position_w_easing = 0.0;
    float value_old = 0.0;
    float value_current = 0.0;
    float value_source = 0.0;
    float value_target = 0.0;
    float value_min = 0.0;
    float value_max = 0.0;

    tEasingFunction easing_fn;
    bool calculateValue();
    void fading_update();

    // private methods
    #ifdef debug_slight_Fade
    void printArray(uint16_t* array);
    #endif

    void generateEvent(uint8_t eventNew);
};

#endif  // ifndef slight_Fade_h

/** the end **/
