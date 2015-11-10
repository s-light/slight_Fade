/**************************************************************************************************
    slight_FaderLin Library
        for more information changelog / history see slight_FaderLin.cpp
    written by stefan krueger (s-light),
        github@s-light.eu, http://s-light.eu, https://github.com/s-light/
**************************************************************************************************/
/**************************************************************************************************
    license
    CC BY SA
        This work is licensed under the
        Creative Commons Attribution-ShareAlike 3.0 Unported License.
        To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/.

    Apache License Version 2.0
        Copyright 2014 Stefan Krueger

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
        Permission is hereby granted, free of charge, to any person obtaining a copy
        of this software and associated documentation files (the "Software"),
        to deal in the Software without restriction, including without limitation
        the rights to use, copy, modify, merge, publish, distribute, sublicense,
        and/or sell copies of the Software, and to permit persons to whom the Software
        is furnished to do so, subject to the following conditions:
        The above copyright notice and this permission notice shall be included in all
        copies or substantial portions of the Software.
        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
        INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
        PARTICULAR PURPOSE AND NONINFRINGEMENT.
        IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
        OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
        OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
        http://opensource.org/licenses/mit-license.php

**************************************************************************************************/

/**************************************************************************************************/
/** Check that for reincluding of class                                                          **/
/**************************************************************************************************/
//Check if this class is allready there or if it must be created...
#ifndef slight_FaderLin_h
#define slight_FaderLin_h


/** Includes Core Arduino functionality **/
#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif


/** Class Definition: **/

class slight_FaderLin {
    public:
        // public definitions:

            // init
            static const uint8_t event_NoEvent                =  0;

            // State
            static const uint8_t event_StateChanged        = 10;

            static const uint8_t state_NotValid            = 10;
            static const uint8_t state_Standby                = 11;
            static const uint8_t state_Fading                = 12;

            static const uint8_t event_fading_Stoped        = 30;

            static const uint8_t event_fading_Finished        = 42;


            //call back function definition
            typedef void (* tCbfuncStateChanged) (uint8_t bID, uint8_t bState);

            typedef void (* tcbfOnEvent) (slight_FaderLin *pInstance, uint8_t bEvent);
            typedef void (* tCbfuncValuesChanged) (uint8_t bID, word *wValues, uint8_t bCount);

        // public methods

            //Constructor
            //slight_FaderLin(uint8_t cbChannelCount_Temp, tCbfuncValuesChanged cbfuncValuesChanged_Temp, tCbfuncStateChanged cbfuncStateChanged_Temp);
            //slight_FaderLin(uint8_t cbID_New, uint8_t cbChannelCount_New, tCbfuncValuesChanged cbfuncValuesChanged_New, tCbfuncStateChanged cbfuncStateChanged_New);
            slight_FaderLin(
                uint8_t cbID_New,
                uint8_t cbChannelCount_New,
                tCbfuncValuesChanged cbfuncValuesChanged_New,
                tcbfOnEvent cbfCallbackOnEvent_New,
                uint16_t *pValues_Current_new = NULL,
                uint16_t *pValues_Target_new = NULL
            );


            //Destructor
            ~slight_FaderLin();


            // initialize class
            void begin();

            // check if class is ready to operate.
            boolean isReady();

            // getID
            uint8_t getID();

            // getState
            uint8_t getState();
            uint8_t printState(Print &pOut);

            // event
            uint8_t getLastEvent();
            uint8_t printEventLast(Print &pOut);
            uint8_t printEvent(Print &pOut, uint8_t bStateTemp);

            // update
            uint8_t update();

            // fader things
            void startFadeTo(unsigned long ulFadeDuration_New, uint16_t *waValues_NewTarget);
            //void startFadeTo(unsigned long ulFadeDuration_New, uint16_t wValues_NewTarget);
            void stopFade();

            //uint16_t *getCurrentValues();
            //uint16_t getCurrentValues();
            void getCurrentValues(uint16_t * pToArray);

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
        const uint8_t cbID;

        // flag to check if the begin function is already called and the class is ready to work.
        boolean bReady;

        uint8_t bState;


        uint8_t bEvent;
        uint8_t bEventLast;

        const tcbfOnEvent cbfCallbackOnEvent;


        //call back functions:
        const tCbfuncValuesChanged cbfuncValuesChanged;
        //const tCbfuncStateChanged cbfuncStateChanged;


        boolean flagFadingFinished;
        boolean Active;

        //unsigned long ulTimeStamp_LastStep;
        unsigned long ulTimeStamp_FadeStart;
        unsigned long ulFadeDuration;

        // channels:
        const uint8_t cbChannelCount;

        //http://forum.arduino.cc/index.php?topic=57433.msg412702#msg412702
        uint16_t *waValues_Source;
        uint16_t *waValues_Target;
        uint16_t *waValues_Current;
        uint16_t *waValues_Dif;
        boolean *baValues_DifIsNegativ;

        // for testing --> SingleChannel Version
        /*
        uint16_t wValues_Source;
        uint16_t wValues_Target;
        uint16_t wValues_Current;
        uint16_t wValues_Dif;
        boolean bValues_DifIsNegativ;
        */

        // private methods
        #ifdef debug_slight_FaderLin
            void printArray(uint16_t *array);
        #endif

        void generateEvent(uint8_t bEventNew);

};

#endif //ifndef slight_FaderLin_h

/** the end **/
