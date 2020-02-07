/**************************************************************************************************

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
        15.05.2014 16:06  added option to specify current and target values outside of library (save ram)

    TO DO:
        ~ test new added pointers.
        ~ develop new fading system thats resource friendly (see infos.ods)

***************************************************************************************************/
/**************************************************************************************************
    license
    CC BY SA
        This work is licensed under the
        Creative Commons Attribution-ShareAlike 3.0 Unported License.
        To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/.

    Apache License Version 2.0
        Copyright 2020 Stefan Krueger

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

***************************************************************************************************/

/** Serial.print to Flash: Notepad++ Replace RegEx
    Find what:        Serial.print(.*)\("(.*)"\);
    Replace with:    Serial.print\1(F("\2"));
**/


// #define debug_slight_FaderLin

/** Includes Core Arduino functionality **/
#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif



/**************************************************************************************************/
/** Include yourselfs header file  (used to define structs and co)                               **/
/**************************************************************************************************/
#include "slight_FaderLin.h"
// use "" for files in same directory as .ino


/**************************************************************************************************/
/****                                       Constructor                                        ****/
/**************************************************************************************************/

/** Constructor **/
// initialize chChannelCount http://forum.arduino.cc/index.php?topic=188261.msg1393390#msg1393390
//  slight_FaderLin::slight_FaderLin(uint8_t cbChannelCount_Temp) : cbChannelCount ( cbChannelCount_Temp ) {
//  slight_FaderLin::slight_FaderLin(uint8_t cbChannelCount_Temp, tCbfuncValuesChanged cbfuncValuesChanged_Temp) : cbChannelCount ( cbChannelCount_Temp ), cbfuncValuesChanged (cbfuncValuesChanged_Temp){
//  slight_FaderLin::slight_FaderLin(uint8_t cbChannelCount_Temp, tCbfuncValuesChanged cbfuncValuesChanged_Temp, tCbfuncStateChanged cbfuncStateChanged_Temp)
slight_FaderLin::slight_FaderLin(
    uint8_t cbID_New,
    uint8_t cbChannelCount_New,
    tCbfuncValuesChanged cbfuncValuesChanged_New,
    tcbfOnEvent cbfCallbackOnEvent_New,
    uint16_t *pValues_Current_new,
    uint16_t *pValues_Target_new
) :
    cbID(cbID_New),
    cbChannelCount(cbChannelCount_New),
    cbfuncValuesChanged(cbfuncValuesChanged_New),
    cbfCallbackOnEvent(cbfCallbackOnEvent_New)
{
    //do some internal inits

    // get memory for the arrays
    /*    http://forum.arduino.cc/index.php?topic=57433.msg412702#msg412702

        After allocating the memory, you can access the memory
        as though it was an array (since it is), just like you are now doing.

        Would you be so kind as to explain what the line does?
        Sure. The malloc function allocates uint8_ts of memory, and returns a pointer to that memory.
        The type that it returns is void *, so it almost always needs to be cast to the correct type.
        In your case, the (uint16_t *) cast is doing just that.
        Inside the parentheses, you need to define how many uint8_ts to allocate.
        The number of uint8_ts is defined by the number of elements that the pointer
        points to (numRings) times the number of uint8_ts in each element (sizeof(uint16_t)).
    */

    if (pValues_Current_new) {
        waValues_Current = pValues_Current_new;
    } else {
        waValues_Current = (uint16_t *)malloc(sizeof(uint16_t) * cbChannelCount);
    }

    if (pValues_Target_new) {
        waValues_Target = pValues_Target_new;
    } else {
        waValues_Target = (uint16_t *)malloc(sizeof(uint16_t) * cbChannelCount);
    }


    waValues_Source = (uint16_t *)malloc(sizeof(uint16_t) * cbChannelCount);
    waValues_Current = (uint16_t *)malloc(sizeof(uint16_t) * cbChannelCount);
    waValues_Dif = (uint16_t *)malloc(sizeof(uint16_t) * cbChannelCount);
    baValues_DifIsNegativ = (boolean *)malloc(sizeof(boolean)  * cbChannelCount);

    /* //single channel version:
    wValues_Source = 0;
    wValues_Target = 0;
    wValues_Current = 0;
    wValues_Dif = 0;
    bValues_DifIsNegativ = 0;
    */

    bState = state_NotValid;

    bReady = false;
}

/** Destructor **/
slight_FaderLin::~slight_FaderLin() {
  //
}

/** Begin (activate object/instantce) **/
void slight_FaderLin::begin() {
    if (bReady == false) {
        #ifdef debug_slight_FaderLin
            Serial.println(F("************************************************************"));
            Serial.println(F("** Welcome to slight_FaderLin World - Library will initialise... **"));
            Serial.println(F("************************************************************"));
            Serial.println(F("slight_FaderLin::begin: "));
        #endif


        /**************************************************/
        /** ??                                           **/
        /**************************************************/

        // clean up memory.
        for ( uint8_t bIndex = 0; bIndex < cbChannelCount; bIndex++) {
            waValues_Source[bIndex] = 0;
            waValues_Target[bIndex] = 0;
            waValues_Current[bIndex] = 0;
            waValues_Dif[bIndex] = 0;
            baValues_DifIsNegativ[bIndex] = 0;
        }

        // single channel version:
        // wValues_Source = 0;
        // wValues_Target = 0;
        // wValues_Current = 0;
        // wValues_Dif = 0;


        bState = state_Standby;

        bReady = true;
    }
}

boolean slight_FaderLin::isReady() {
    return bReady;
}



/**************************************************************************************************/
/** functions                                                                                    **/
/**************************************************************************************************/

/************************************************/
/**  Debug things                              **/
/************************************************/

#ifdef debug_slight_FaderLin

// void slight_FaderLin::printArray(uint16_t *array) {
//     Serial.print(F(" "));
//     uint8_t bCount = cbChannelCount; // (sizeof(*&array) / sizeof(uint16_t))
//     //Serial.print(F(" Count: "));
//     //Serial.print(bCount);
//     for(uint8_t bIndex = 0; bIndex < bCount; bIndex++){
//         Serial.print(F(" ["));
//         Serial.print(bIndex);
//         Serial.print(F("]:"));
//         Serial.print(array[bIndex]);
//     }
// }


void slight_FaderLin::printArray(uint16_t *array) {
    Serial.print(F(" "));
    uint8_t bCount = cbChannelCount;
    uint8_t bIndex = 0;
    printuint8_tAlignRight(array[bIndex]);
    for(bIndex = 1; bIndex < bCount; bIndex++){
        Serial.print(F(", "));
        printuint8_tAlignRight(array[bIndex]);
    }
}

void slight_FaderLin::printuint8_tAlignRight(uint8_t bValue) {
    //uint8_t bOffset = 0;
    if (bValue < 100) {
        if (bValue < 10) {
            //bOffset = 2;
            Serial.print(F("  "));
        } else {
            //bOffset = 1;
            Serial.print(F(" "));
        }
    }
    Serial.print(bValue);
}

#endif

/************************************************/
/**  Calc steps                                **/
/************************************************/


// returns state of Fading system.
uint8_t slight_FaderLin::update() {
    /** Fading System State
        state_Standby                = 0;
        state_Finished                = 1;
        state_Fading                = 2;
        state_Fading_newValues        = 10;
    **/
    uint8_t bStateTemp = state_NotValid;
    if (bReady == true) {
        // if fading is active calc steps
        if (Active) {

            #ifdef debug_slight_FaderLin
                Serial.print(F("u 1.0:target:"));
                printArray(waValues_Target);
                Serial.println();
            #endif

            //calc duration since FadeStart
            unsigned long ulDurationTemp = ( millis() - ulTimeStamp_FadeStart );
            #ifdef debug_slight_FaderLin
                /**/
                Serial.print(F("ulDurationTemp: "));
                Serial.print(ulDurationTemp);
                Serial.println();
                /**/
            #endif
            //if ulDuration < ulFadeDuration
            if (ulDurationTemp < ulFadeDuration) {
                // set state to 'Fading';
                bStateTemp = state_Fading;

                /* ulDurations steht im Verhältniss zum aktuellen wert!
                    gesammt dauer zu gesamt werte differenz    =    vergangene dauer zu aktueller wert
                    aktueller wert = x
                    x = (Werte Dif * vergangene dauer) / gesamt dauer;

                    ulRatio = vergangene dauer / gesamt dauer;
                    X = WertDif * ulRatio;
                        // calc Ratio
                        //unsigned long ulRatio = (ulDurationTemp / ulFadeDuration);
                    --> Geht nicht! da Ratio < 1 ist....
                    use: ((wValues_Dif * ulDurationTemp) / ulFadeDuration)
                */


                #ifdef debug_slight_FaderLin
                    Serial.print(F("u 2.0:target:"));
                    printArray(waValues_Target);
                    Serial.println();
                #endif

                bool bFlag_NewValues = 0;

                // for every channel calc value
                for ( uint8_t bIndex = 0; bIndex < cbChannelCount; bIndex++) {

                    #ifdef debug_slight_FaderLin
                        Serial.print(F("u"));
                        Serial.print(bIndex+10);
                        Serial.print(F(".1:target:"));
                        printArray(waValues_Target);
                        Serial.println();
                    #endif

                    word wValueTemp = ((waValues_Dif[bIndex] * ulDurationTemp) / ulFadeDuration);
                    #ifdef debug_slight_FaderLin
                        Serial.print(F("wValueTemp: "));
                        Serial.print(wValueTemp);
                        Serial.println();
                    #endif

                    // calc result value
                    word wValue_NewCurrent = 0;
                    if (baValues_DifIsNegativ[bIndex]) {
                        wValue_NewCurrent = waValues_Source[bIndex] - wValueTemp;
                    } else {
                        wValue_NewCurrent = waValues_Source[bIndex] + wValueTemp;
                    }

                    #ifdef debug_slight_FaderLin
                        Serial.print(F("u"));
                        Serial.print(bIndex+10);
                        Serial.print(F(".2:target:"));
                        printArray(waValues_Target);
                        Serial.println();
                    #endif

                    // check if there is a new value
                    if ( wValue_NewCurrent != waValues_Current[bIndex] ) {
                        waValues_Current[bIndex] = wValue_NewCurrent;
                        // set flag that there are new values..
                        bFlag_NewValues = 1;

                    }

                    #ifdef debug_slight_FaderLin
                        Serial.print(F("u"));
                        Serial.print(bIndex+10);
                        Serial.print(F(".3:target:"));
                        printArray(waValues_Target);
                        Serial.println();
                    #endif
                } // for loop

                if (bFlag_NewValues) {
                    //call cbfunc for updating output values.
                    cbfuncValuesChanged(cbID, waValues_Current, cbChannelCount);
                }

                #ifdef debug_slight_FaderLin
                    Serial.print(F("u22.0:target:"));
                    printArray(waValues_Target);
                    Serial.println();
                #endif

                #ifdef debug_slight_FaderLin
                    Serial.print(F("waValues_Current: "));
                    printArray(waValues_Current);
                    Serial.println();
                #endif

                /* old
                // single channel version:
                    word wValueTemp = ((wValues_Dif * ulDurationTemp) / ulFadeDuration);
                    #ifdef debug_slight_FaderLin
                        Serial.print(F("wValueTemp: "));
                        Serial.print(wValueTemp);
                        Serial.println();
                    #endif
                    word wValue_NewCurrent = 0;
                    if (bValues_DifIsNegativ) {
                        //wValue_NewCurrent = ((wValues_Dif * ulDurationTemp) / ulFadeDuration) - wValues_Source;
                        wValue_NewCurrent = wValues_Source - wValueTemp;
                    } else {
                        wValue_NewCurrent = wValues_Source + wValueTemp;
                    }
                    // check if there is a new value
                    if ( wValue_NewCurrent != wValues_Current ) {
                        wValues_Current = wValue_NewCurrent;
                        // set changes to 1
                        bState = state_Fading_newValues;
                    } else {
                        // no new values..
                        bState = state_Fading;
                    }
                    #ifdef debug_slight_FaderLin
                        Serial.print(F("wValues_Current: "));
                        Serial.print(wValues_Current);
                        Serial.println();
                    #endif
                */

            } else {
                // set values to Target. Time is Over!!
                #ifdef debug_slight_FaderLin
                    Serial.println(F("Target Time is Over... Fade is Finished!"));
                #endif

                #ifdef debug_slight_FaderLin
                    Serial.print(F("ff:target:"));
                    printArray(waValues_Target);
                    Serial.println();
                    Serial.println(F("ff:setcurrent2target"));
                #endif


                //this do not work. it just sets the pointer to the same memeory location...
                //    waValues_Current = waValues_Target;
                //use this:
                for ( uint8_t bIndex = 0; bIndex < cbChannelCount; bIndex++) {
                    // set end Values
                    waValues_Current[bIndex] = waValues_Target[bIndex];
                }

                //call cbfunc for updating output values.
                cbfuncValuesChanged(cbID, waValues_Current, cbChannelCount);

                #ifdef debug_slight_FaderLin
                    Serial.print(F("ff:target:"));
                    printArray(waValues_Target);
                    Serial.println();
                    Serial.print(F("ff:current:"));
                    printArray(waValues_Current);
                    Serial.println();
                #endif

                /*
                for ( uint8_t bIndex = 0; bIndex < cbChannelCount; bIndex++) {
                    waValues_Current[bIndex] = waValues_Target[bIndex];
                }*/
                // single channel version:
                    //wValues_Current = wValues_Target;

                bStateTemp = state_Standby;

                // set fininshed flag!
                Active = 0;
                flagFadingFinished = 1;
                generateEvent(event_fading_Finished);
            }
        } else {
            //nothing to do - so its Standby.
            bStateTemp = state_Standby;
        }
        // check if State has Changed.
        if (bState != bStateTemp) {
            bState = bStateTemp;
            //call cbfunc for StateChange.
            generateEvent(event_StateChanged);
        }

    } // end if bReady
    return bState;
}

// start new Fade
void slight_FaderLin::startFadeTo(
    uint32_t ulFadeDuration_New,
    uint16_t *waValues_NewTarget
) {
    if (bReady == true) {
        #ifdef debug_slight_FaderLin
            Serial.print(F("startFadeTo:  ulFadeDuration_New: "));
            Serial.print(ulFadeDuration_New);
            printArray(waValues_NewTarget);
            Serial.println();
        #endif

        // Stop Fading
            Active = 0;
        // reset variables.
            flagFadingFinished = 1;

        if ( (flagFadingFinished) ) {
            // set Source and Target values
            for (uint8_t bIndex = 0; bIndex < cbChannelCount; bIndex++) {
                // set Source values
                waValues_Source[bIndex] = waValues_Current[bIndex];
                // set Target Values
                waValues_Target[bIndex] = waValues_NewTarget[bIndex];
                // build Dif
                if (waValues_Target[bIndex] > waValues_Source[bIndex]) {
                    waValues_Dif[bIndex] =
                        waValues_Target[bIndex] - waValues_Source[bIndex];
                    baValues_DifIsNegativ[bIndex] = false;
                } else {
                    waValues_Dif[bIndex] =
                        waValues_Source[bIndex] - waValues_Target[bIndex];
                    baValues_DifIsNegativ[bIndex] = true;
                }
            }


            #ifdef debug_slight_FaderLin
                Serial.print(F("fstart:target  :"));
                printArray(waValues_Target);
                Serial.println();
            #endif
            #ifdef debug_slight_FaderLin
                Serial.print(F("waValues_Source: "));
                printArray(waValues_Source);
                Serial.println();

                Serial.print(F("waValues_Target: "));
                printArray(waValues_Target);
                Serial.println();

                Serial.print(F("waValues_Dif   : "));
                printArray(waValues_Dif);
                Serial.println();
                /* boolean is not supported jet..
                Serial.print(F("baValues_DifIsNegativ: "));
                printArray(baValues_DifIsNegativ);
                Serial.println();*/
            #endif



            /*
            // single channel version
                // set Source values
                wValues_Source = wValues_Current;
                // set Target Values
                wValues_Target = waValues_NewTarget[0];
                if ( wValues_Target >= wValues_Source) {
                    wValues_Dif = wValues_Target - wValues_Source;
                    bValues_DifIsNegativ = false;
                } else {
                    wValues_Dif = wValues_Source - wValues_Target;
                    bValues_DifIsNegativ = true;
                }

            #ifdef debug_slight_FaderLin
                Serial.print(F("wValues_Source: "));
                Serial.print(wValues_Source);
                Serial.print(F("; wValues_Target:"));
                Serial.print(wValues_Target);
                Serial.print(F("; wValues_Dif:"));
                Serial.print(wValues_Dif);
                Serial.print(F("; bValues_DifIsNegativ:"));
                Serial.print(bValues_DifIsNegativ);
                Serial.println();
            #endif
            */
            // set Start Time
            ulTimeStamp_FadeStart = millis();
            // set Duration
            ulFadeDuration = ulFadeDuration_New;
            #ifdef debug_slight_FaderLin
                Serial.print(F("ulFadeDuration: "));
                Serial.print(ulFadeDuration);
                Serial.println();
            #endif
            // start system
            flagFadingFinished = 0;
            Active = 1;
            #ifdef debug_slight_FaderLin
                Serial.print(F("for   update:target:"));
                printArray(waValues_Target);
                Serial.println();
            #endif
            // just do the first calculation immediately
            update();
            #ifdef debug_slight_FaderLin
                Serial.print(F("after update:target:"));
                printArray(waValues_Target);
                Serial.println();
            #endif
        }
    }
}

void slight_FaderLin::startFadeTo(
    uint32_t ulFadeDuration_New,
    uint16_t wValue_NewTarget
) {
    if (bReady == true) {
        uint16_t temp_array[cbChannelCount];
        for (size_t i = 0; i < cbChannelCount; i++) {
            temp_array[i] = wValue_NewTarget;
        }
        startFadeTo(ulFadeDuration_New, temp_array);
    }
}


void slight_FaderLin::stopFade() {
    if (bReady == true)
    {
        // if fading is active in some way or not finished yet
        if ( (!flagFadingFinished) || (Active) ) {
            // Stop Fading
            Active = 0;
            // reset variables.
            flagFadingFinished = 1;
        }
    }
}

/*
uint16_t *slight_FaderLin::getCurrentValues() {
//uint16_t slight_FaderLin::getCurrentValues() {
    //return wValues_Current;
    return waValues_Current;
}*/



uint8_t slight_FaderLin::getID() {
    return cbID;
}

uint8_t slight_FaderLin::getState() {
    return bState;
};

uint8_t slight_FaderLin::printState(Print &pOut) {
    switch (bState) {
        case slight_FaderLin::state_Standby : {
                pOut.print(F("standby"));
            } break;
        case slight_FaderLin::state_NotValid : {
                pOut.print(F("NotValid"));
            } break;
        case slight_FaderLin::state_Fading : {
                pOut.print(F("fading"));
            } break;
        default: {
                pOut.print(F("error: '"));
                pOut.print(bState);
                pOut.print(F(" ' is not a know state."));
            }
    } //end switch
    return bState;
};


uint8_t slight_FaderLin::getLastEvent() {
    return bEventLast;
};

uint8_t slight_FaderLin::printEvent(Print &pOut, uint8_t bEventTemp) {
    switch (bEventTemp) {
        case slight_FaderLin::event_NoEvent : {
            pOut.print(F("no event"));
        } break;

        case slight_FaderLin::event_StateChanged : {
            pOut.print(F("state changed"));
        } break;

        // fading
        case slight_FaderLin::event_fading_Finished: {
            pOut.print(F("fading finished"));
        } break;
        case slight_FaderLin::event_fading_Stoped: {
            pOut.print(F("fading stoped"));
        } break;

        default: {
            pOut.print(F("error: '"));
            pOut.print(bState);
            pOut.print(F(" ' is not a know event."));
        }
    } //end switch
    return bState;
};

uint8_t slight_FaderLin::printEventLast(Print &pOut) {
    printEvent(pOut, bEventLast);
    return bEventLast;
};


void slight_FaderLin::generateEvent(uint8_t bEventNew) {
    bEvent = bEventNew;
    // call event
    if (bEvent != event_NoEvent) {
        cbfCallbackOnEvent(this, bEvent);
    }
    bEventLast = bEvent;
    bEvent = event_NoEvent;
}



uint8_t slight_FaderLin::getChannelCount() {
    return cbChannelCount;
}


void slight_FaderLin::getCurrentValues(uint16_t * pToArray) {
    /*Serial.print(F("waValues_Current[0]: "));
    Serial.println(waValues_Current[0]);
    for (uint8_t bIndex = 0; bIndex < cbChannelCount; bIndex++) {
        pToArray[bIndex] = waValues_Current[bIndex];
    }*/
    memcpy(pToArray, waValues_Current, cbChannelCount*sizeof(word));
    /*Serial.print(F("pToArray[0]: "));
    Serial.println(pToArray[0]);*/
}

/****************************************************************************************************/
/** THE END                                                                                        **/
/****************************************************************************************************/
