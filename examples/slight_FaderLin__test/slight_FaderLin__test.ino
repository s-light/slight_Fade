/**************************************************************************************************

    test_slight_FaderLinLib
        test the new approach for the slight_FaderLin lib.

    hardware:
        Arduino board of any typ.
        D9 --> LED connected to R to VCC

    libraries used:
        ~ slight_FaderLin
        ~

    written by stefan krueger (s-light), git@s-light.eu, http://s-light.eu


    changelog / history
        16.09.2013 19:05 created.
        16.09.2013 20:45 angefangen.
        16.09.2013 21:30 16Bit fading; DMX tested; it works!
        16.09.2013 23:15 multichannel support finished.
        16.09.2013 23:45 welcome fade
        18.09.2013 09:00 fade to zero is now also possible.
        18.09.2013 19:40 cleand up a little bit.
        12.11.2013 10:25 created out of test_faderDMX.
        12.11.2013 13:45 callback functions work.
        13.11.2013 18:00 add ID
        25.01.2014 15:46 some small changes (renamed lib), added printState()
        09.03.2014 20:26 updated to handle Instance callback

    TO DO:
        check why the switch statement does not work.

**************************************************************************************************/
/**************************************************************************************************
    license

    CC BY SA
        This work is licensed under the
        Creative Commons Attribution-ShareAlike 3.0 Unported License.
        To view a copy of this license, visit
http://creativecommons.org/licenses/by-sa/3.0/.

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
        Copyright (c) 2014 Stefan Krueger
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
**************************************************************************************************/
/**************************************************************************************************/
/** Includes:  (must be at the beginning of the file.) **/
/**************************************************************************************************/
// use "" for files in same directory as .ino
// #include "file.h"

#include <slight_FaderLin.h>

/**************************************************************************************************/
/** info **/
/**************************************************************************************************/
void print_info(Print &out) {
    out.println();
    //             "|~~~~~~~~~|~~~~~~~~~|~~~..~~~|~~~~~~~~~|~~~~~~~~~|"
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println(F("|                       ^ ^                      |"));
    out.println(F("|                      (0,0)                     |"));
    out.println(F("|                      ( _ )                     |"));
    out.println(F("|                       \" \"                      |"));
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println(F("| slight_FaderLin__test.ino"));
    out.println(F("|   Sketch to test, develop and showcase functions of "
                  "slight_FaderLin library"));
    out.println(F("|"));
    out.println(F("| This Sketch has a debug-menu:"));
    out.println(F("| send '?'+Return for help"));
    out.println(F("|"));
    out.println(F("| dream on & have fun :-)"));
    out.println(F("|"));
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println(F("|"));
    // out.println(F("| Version: Nov 11 2013  20:35:04"));
    out.print(F("| version: "));
    out.print(F(__DATE__));
    out.print(F("  "));
    out.print(F(__TIME__));
    out.println();
    out.println(F("|"));
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println();

    // out.println(__DATE__); Nov 11 2013
    // out.println(__TIME__); 20:35:04
}

/** Serial.print to Flash: Notepad++ Replace RegEx
    Find what:        Serial.print(.*)\("(.*)"\);
    Replace with:    Serial.print\1(F("\2"));
**/

/**************************************************************************************************/
/** libraries **/
/**************************************************************************************************/

// ------------------------------------------
/**  slight FaderLin                             **/
// ------------------------------------------
const uint8_t myFaderOne__channel_count = 2;
slight_FaderLin myFaderOne =
    slight_FaderLin(0,                         // byte kID
                    myFaderOne__channel_count, // byte channelCount
                    myCallback_ValuesChanged,  // tCallbackFunctionValuesChanged
                    myCallback_onEvent         // tCallbackFunction

    );

/**************************************************************************************************/
/** definitions (gloabl) **/
/**************************************************************************************************/

bool bLEDState = 0;
const byte kID_LED_Info = 9; // D9

unsigned long ulDebugOut_LiveSign_TimeStamp_LastAction = 0;
const uint16_t cwDebugOut_LiveSign_UpdateInterval = 1000; // ms

bool bDebugOut_LiveSign_Serial_Enabled = 0;
bool bDebugOut_LiveSign_LED_Enabled = 0;

// ------------------------------------------
/**  led and serial debug                        **/
// ------------------------------------------
// myFaderOne output on:
byte bPin_LED = 9;

byte bDebugOut_myFaderOne_Output_Enable = 0;

// unsigned long ulTimeStamp_LastAction    = 0;
// const uint16_t cwUpdateInterval            = 0; //ms

// ------------------------------------------
/** Menu Input                                   **/
// ------------------------------------------

// a string to hold new data
char sMenu_Input_New[] = "[ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, ]";
// flag if string is complete
bool bMenu_Input_New_FlagComplete = false;

// string for Currently to process Command
char sMenu_Command_Current[] = "[ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, ]";

/**************************************************************************************************/
/** functions **/
/**************************************************************************************************/

// ------------------------------------------
/**  Debug things                              **/
// ------------------------------------------

// http://forum.arduino.cc/index.php?topic=183790.msg1362282#msg1362282
int freeRam() {
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void printBinary8(Print &out, uint8_t iIn) {

    for (unsigned int mask = 0b10000000; mask; mask >>= 1) {
        if (mask & iIn) {
            out.print('1');
        } else {
            out.print('0');
        }
    }
}

void printBinary12(Print &out, uint16_t iIn) {
    //                       B12345678   B12345678
    // for (unsigned int mask = 0x8000; mask; mask >>= 1) {
    for (unsigned int mask = 0b100000000000; mask; mask >>= 1) {
        if (mask & iIn) {
            out.print('1');
        } else {
            out.print('0');
        }
    }
}

void printBinary16(Print &out, uint16_t iIn) {
    //                       0B12345678   B12345678
    for (unsigned int mask = 0b1000000000000000; mask; mask >>= 1) {
        if (mask & iIn) {
            out.print('1');
        } else {
            out.print('0');
        }
    }
}

void printArray_uint8(Print &out, byte *array, byte bCount) {
    out.print(F(" "));
    byte bIndex = 0;
    printAlignRight_uint8(out, array[bIndex]);
    for (bIndex = 1; bIndex < bCount; bIndex++) {
        out.print(F(", "));
        printAlignRight_uint8(out, array[bIndex]);
    }
}

void printAlignRight_uint8(Print &out, byte bValue) {
    // byte bOffset = 0;
    if (bValue < 100) {
        if (bValue < 10) {
            // bOffset = 2;
            out.print(F("  "));
        } else {
            // bOffset = 1;
            out.print(F(" "));
        }
    }
    out.print(bValue);
}

void printArray_uint16(Print &out, uint16_t *array, byte bCount) {
    out.print(F(" [ "));
    byte bIndex = 0;
    printAlignRight_uint16(out, array[bIndex]);
    for (bIndex = 1; bIndex < bCount; bIndex++) {
        out.print(F(", "));
        printAlignRight_uint16(out, array[bIndex]);
    }
    out.print(F("]"));
}

byte printAlignRight_uint16(Print &out, uint16_t wValue) {
    byte bLeadingZeros = 0;
    if (wValue < 10000) {
        bLeadingZeros = bLeadingZeros + 1;
        out.print(F(" "));
        if (wValue < 1000) {
            bLeadingZeros = bLeadingZeros + 1;
            out.print(F(" "));
            if (wValue < 100) {
                bLeadingZeros = bLeadingZeros + 1;
                out.print(F(" "));
                if (wValue < 10) {
                    bLeadingZeros = bLeadingZeros + 1;
                    out.print(F(" "));
                }
            }
        }
    }
    out.print(wValue);
    return bLeadingZeros;
}

// ------------------------------------------
/**  Menu System                               **/
// ------------------------------------------

// Modes for Menu Switcher
const uint8_t cbMenuMode_MainMenu = 1;
const uint8_t cbMenuMode_SubMenu1 = 2;
uint8_t bMenuMode = cbMenuMode_MainMenu;

// SubMenu SetValues
void handle_SubMenu1(Print &out, char *caCommand) {
    out.println(F("SubMenu1:"));
    out.print(F("\t caCommand: "));
    out.println(caCommand);
    out.println(F("\t nothing here."));
    out.println(F("\t finished."));
    // exit submenu
    // reset state manschine of submenu
    // jump to main
    bMenuMode = cbMenuMode_MainMenu;
    bMenu_Input_New_FlagComplete = true;
}

// Main Menu
void handle_MainMenu(Print &out, char *caCommand) {
    /* out.print("sCommand: '");
    out.print(sCommand);
    out.println("'"); */
    switch (caCommand[0]) {
    case 'h':
    case 'H':
    case '?': {
        // help
        out.println(
            F("____________________________________________________________"));
        out.println();
        out.println(F("Help for Commands:"));
        out.println();
        out.println(F("\t '?': this help"));
        out.println(F("\t 'i': sketch info"));
        out.println(F("\t 'y': toggle DebugOut livesign print"));
        out.println(F("\t 'Y': toggle DebugOut livesign LED"));
        out.println(F("\t 'x': run performance test"));
        out.println(F("\t 'X': tests"));
        out.println();
        out.println(F("\t 'f': startFadeTo(ID, value) 'f65535'"));
        out.println(F("\t 'a': myFaderOne.startFadeTo 0"));
        out.println(F("\t 'A': myFaderOne.startFadeTo 255"));
        out.println();
        out.println(F("\t 'set:' enter SubMenu1"));
        out.println();
        out.println(
            F("____________________________________________________________"));
    } break;
    case 'i': {
        print_info(out);
    } break;
    case 'y': {
        out.println(F("\t toggle DebugOut livesign Serial:"));
        bDebugOut_LiveSign_Serial_Enabled = !bDebugOut_LiveSign_Serial_Enabled;
        out.print(F("\t bDebugOut_LiveSign_Serial_Enabled:"));
        out.println(bDebugOut_LiveSign_Serial_Enabled);
    } break;
    case 'Y': {
        out.println(F("\t toggle DebugOut livesign LED:"));
        bDebugOut_LiveSign_LED_Enabled = !bDebugOut_LiveSign_LED_Enabled;
        out.print(F("\t bDebugOut_LiveSign_LED_Enabled:"));
        out.println(bDebugOut_LiveSign_LED_Enabled);
    } break;
    case 'X': {
        // get state
        out.println(F("__________"));
        out.println(F("Tests:"));

        // out.println(F("nothing to do."));

        // uint16_t wTest = 65535;
        // uint16_t wTest = atoi(&caCommand[1]);
        // out.print(F("wTest: "));
        // out.print(wTest);
        // out.println();
        //
        // out.print(F("1: "));
        // out.print((byte)wTest);
        // out.println();
        //
        // out.print(F("2: "));
        // out.print((byte)(wTest>>8));
        // out.println();

        out.println(F("* get a value from input:"));
        uint16_t uiValue = atoi(&caCommand[1]);

        out.println(F("* create temp array"));
        uint16_t uiaTemp[myFaderOne.getChannelCount()];

        out.println(F("* set values"));
        // memset is only working on 8bit value..
        // memset(uiaTemp, uiValue, size)
        for (size_t i = 0; i < myFaderOne.getChannelCount(); i++) {
            uiaTemp[i] = uiValue;
        }

        out.println(F("* print result"));
        printArray_uint16(out, uiaTemp, myFaderOne.getChannelCount());

        out.println();

        out.println(F("__________"));
    } break;
    case 'x': {
        out.println(F("__________"));
        out.println(F("Run Performance Test:"));
        // timeMeasurement_readable();
        timeMeasurement_colums();
        out.println(F("__________"));
    } break;
    //--------------------------------------------------------------------------------
    case 'f': {
        out.print(F("\t startFadeTo: "));
        uint16_t value = atoi(&caCommand[1]);
        out.print(value);
        myFaderOne.startFadeTo(5000, value);
    } break;
    case 'A': {
        out.println(F("\t startFadeTo 0"));
        uint16_t value = 0;
        myFaderOne.startFadeTo(5000, value);
    } break;
    case 'a': {
        out.println(F("\t startFadeTo 255"));
        uint16_t value = 255;
        myFaderOne.startFadeTo(5000, value);
    } break;
    //--------------------------------------------------------------------------------
    case 's': {
        // SubMenu1
        if ((caCommand[1] == 'e') && (caCommand[2] == 't') &&
            (caCommand[3] == ':')) {
            // if full command is 'set:' enter submenu
            bMenuMode = cbMenuMode_SubMenu1;
            if (1) { // if ( caCommand[4] != '\0' ) {
                // full length command
                // handle_SetValues(out, &caCommand[4]);
            } else {
                bMenu_Input_New_FlagComplete = true;
            }
        }
    } break;
    //--------------------------------------------------------------------------------
    default: {
        out.print(F("command '"));
        out.print(caCommand);
        out.println(F("' not recognized. try again."));
        sMenu_Input_New[0] = '?';
        bMenu_Input_New_FlagComplete = true;
    }
    } // end switch

    // end Command Parser
}

// Menu Switcher
void menuSwitcher(Print &out, char *caCommand) {
    switch (bMenuMode) {
    case cbMenuMode_MainMenu: {
        handle_MainMenu(out, caCommand);
    } break;
    case cbMenuMode_SubMenu1: {
        handle_SubMenu1(out, caCommand);
    } break;
    default: {
        // something went wronge - so reset and show MainMenu
        bMenuMode = cbMenuMode_MainMenu;
    }
    } // end switch bMenuMode
}

// Check for NewLineComplete and enter menuSwitcher
// sets Menu Output channel (out)
void check_NewLineComplete() {
    // if SMenuCurrent is a full Line (terminated with \n) than parse things
    if (bMenu_Input_New_FlagComplete) {
        // Serial.print(F("bMenu_Input_New_FlagComplete: sMenu_Input_New: '"));
        // Serial.print(sMenu_Input_New);
        // Serial.println(F("'"));
        // Serial.print(F("   state_UI: '"));
        // Serial.print(state_UI);
        // Serial.println(F("'"));

        // coppy to current buffer
        strcpy(sMenu_Command_Current, sMenu_Input_New);

        // reset input
        memset(sMenu_Input_New, '\0', sizeof(sMenu_Input_New) - 1);
        bMenu_Input_New_FlagComplete = false;

        // run command
        menuSwitcher(Serial, sMenu_Command_Current);

    } // if bMenu_Input_New_FlagComplete
}

// ------------------------------------------
/**  Serial Receive Handling                   **/
// ------------------------------------------

void handle_SerialReceive() {
    // collect next input text
    while (Serial.available()) {
        // get the new byte:
        char charNew = (char)Serial.read();
        // Serial.print(F("charNew '"));
        // Serial.print(charNew);
        // Serial.print(F("' : "));
        // Serial.println(charNew, DEC);
        // Serial.print(F("'\\n' : "));
        // Serial.println('\n', DEC);
        // Serial.print(F("'\\r' : '"));
        // Serial.println('\r', DEC);

        /* http://forums.codeguru.com/showthread.php?253826-C-String-What-is-the-difference-between-n-and-r-n
            '\n' == 10 == LineFeed == LF
            '\r' == 13 == Carriag Return == CR
            Windows: '\r\n'
            Linux: '\n'
            Apple: '\r'
        */
        // if the incoming character is a linefeed '\r' or newline '\n'
        // if ((charNew == '\r') || (charNew == '\n')) {
        if (charNew == '\n') {
            // clean up last received char if it is a '\r'
            if (sMenu_Input_New[strlen(sMenu_Input_New) - 1] == '\r') {
                sMenu_Input_New[strlen(sMenu_Input_New) - 1] = '\0';
            }
            // set complete flag
            bMenu_Input_New_FlagComplete = true;
        } else {
            // add it to the sMenu_Input_New:
            // sMenu_Input_New = sMenu_Input_New + charNew;
            // char array version
            // check for length..
            if (strlen(sMenu_Input_New) < sizeof(sMenu_Input_New)) {
                sMenu_Input_New[strlen(sMenu_Input_New)] = charNew;
            } else {
                Serial.println(F("inputstring to long!"));
                // reset
                memset(sMenu_Input_New, '\0', sizeof(sMenu_Input_New) - 1);
                bMenu_Input_New_FlagComplete = false;
            }
        }
    }
}

// ------------------------------------------
/**  my Fader things                           **/
// ------------------------------------------

void myCallback_ValuesChanged(slight_FaderLin *pInstance, uint16_t *values,
                              byte count) {
    uint8_t pin = pInstance->getID();
    // uint8_t pin = (*instance).getID();

    if (bDebugOut_myFaderOne_Output_Enable) {
        Serial.print(F("OutputValue: "));
        printArray_uint16(Serial, values, count);
        Serial.println();
    }
    // invertiere ausgang..
    analogWrite(bPin_LED, 255 - values[0]);
    /*
    for (byte bIndex = 0; bIndex < bCount; bIndex++) {
        DMXSerialLeo.write(bIndex + 1, bPointerValues[bIndex]);
    }*/
}

void myCallback_onEvent(slight_FaderLin *instance) {
    // react on events:

case slight_FaderLin::event_fading_Finished: {
    Serial.print(F("led on pin "));
    Serial.print((*instance).getID());
    Serial.print(F(": fading Finished."));
    Serial.println();
} break;
} // end switch
}

void myCallback_onEvent(slight_FaderLin *instance) {
    // Serial.print(F("Instance ID:"));
    // Serial.println((*pInstance).getID());
    //

    uint8_t event = instance->getEventLast();
    // Serial.print(F("Event: "));
    // (*pInstance).printEvent(Serial, event);
    // Serial.println();

    // react on events:
    switch (event) {
    case slight_FaderLin::event_StateChanged: {
        // Serial.print(F("slight_FaderLin "));
        // Serial.print((*pInstance).getID());
        // Serial.println(F(" : "));
        // Serial.print(F("\t state: "));
        // (*pInstance).printState(Serial);
        // Serial.println();

        // switch (state) {
        //     case slight_FaderLin::state_Standby : {
        //             //
        //         } break;
        //     case slight_FaderLin::state_Fading : {
        //             //
        //         } break;
        //     case slight_FaderLin::state_Finished : {
        //             //
        //         } break;
        // } //end switch
    } break;

    case slight_FaderLin::event_fading_Finished: {
        // Serial.print(F("\t fading Finished."));
    } break;
    } // end switch
}

// ------------------------------------------
/** Time Measurement                             **/
// ------------------------------------------
void timeMeasurement_readable() {
    Serial.println(F("time measurement (can take some seconds): "));

    Serial.print(F("\tchannelCount: "));
    Serial.println(myFaderOne.getChannelCount());
    Serial.print(F("\t\tfree RAM = "));
    Serial.println(freeRam());

    unsigned long ulTimeStamp = 0;
    unsigned long ulDuration = 0;
    /** **/
    uint16_t wLoopCount = 10000;
    Serial.println(F("\t\tupdate() in standby: "));
    ulTimeStamp = millis();
    for (uint16_t wLoop = 0; wLoop < wLoopCount; wLoop++) {
        myFaderOne.update();
    }
    ulDuration = millis() - ulTimeStamp;
    Serial.print(F("\t\t\t--> "));
    Serial.print(ulDuration);
    Serial.print(F("ms for "));
    Serial.print(wLoopCount);
    Serial.print(F(" calls"));
    Serial.println();

    Serial.print(F("\t\t\t--> "));
    Serial.print((ulDuration / (float)wLoopCount) * 1000);
    Serial.print(F("us for "));
    Serial.print(1);
    Serial.print(F(" call"));
    Serial.println();

    /** **/
    Serial.println(F("\t\tupdate() while fading: "));
    uint16_t waTemp[] = {0, 0};
    myFaderOne.startFadeTo(0, waTemp);
    waTemp[0] = 255;
    waTemp[1] = 255;
    myFaderOne.startFadeTo(10000, waTemp);
    ulTimeStamp = millis();
    for (uint16_t wLoop = 0; wLoop < wLoopCount; wLoop++) {
        myFaderOne.update();
    }
    ulDuration = millis() - ulTimeStamp;
    Serial.print(F("\t\t\t--> "));
    Serial.print(ulDuration);
    Serial.print(F("ms for "));
    Serial.print(wLoopCount);
    Serial.print(F(" calls"));
    Serial.println();

    Serial.print(F("\t\t\t--> "));
    Serial.print((ulDuration / (float)wLoopCount) * 1000);
    Serial.print(F("us for "));
    Serial.print(1);
    Serial.print(F(" call"));
    Serial.println();
    /** **/
}

void timeMeasurement_colums() {
    Serial.println(F("time measurement (can take some seconds): "));

    uint16_t wLoopCount = 10000;
    unsigned long ulTimeStamp = 0;
    unsigned long ulDuration = 0;

    Serial.print(F("loopCount"));
    Serial.print(F("\t"));
    Serial.print(F("channelCount"));
    Serial.print(F("\t"));
    Serial.print(F("free RAM"));
    Serial.print(F("\t"));
    Serial.print(F("standby (ms/sum)"));
    Serial.print(F("\t"));
    Serial.print(F("standby (us/call)"));
    Serial.print(F("\t"));
    Serial.print(F("fading (ms/sum)"));
    Serial.print(F("\t"));
    Serial.print(F("fading (us/call)"));
    Serial.print(F("\t"));

    Serial.println();

    Serial.print(wLoopCount);
    Serial.print(F("\t"));
    Serial.print(myFaderOne.getChannelCount());
    Serial.print(F("\t"));
    Serial.print(freeRam());
    Serial.print(F("\t"));

    // standby
    ulTimeStamp = millis();
    for (uint16_t wLoop = 0; wLoop < wLoopCount; wLoop++) {
        myFaderOne.update();
    }
    ulDuration = millis() - ulTimeStamp;

    Serial.print(ulDuration);
    Serial.print(F("\t"));
    Serial.print((ulDuration / (float)wLoopCount) * 1000);
    Serial.print(F("\t"));

    // fading
    uint16_t waTemp[] = {0, 0};
    myFaderOne.startFadeTo(0, waTemp);
    waTemp[0] = 255;
    waTemp[1] = 255;
    myFaderOne.startFadeTo(60000, waTemp);

    ulTimeStamp = millis();
    for (uint16_t wLoop = 0; wLoop < wLoopCount; wLoop++) {
        myFaderOne.update();
    }
    ulDuration = millis() - ulTimeStamp;

    Serial.print(ulDuration);
    Serial.print(F("\t"));
    Serial.print((ulDuration / (float)wLoopCount) * 1000);
    Serial.print(F("\t"));

    Serial.println();
}

// ------------------------------------------
/**                                            **/
// ------------------------------------------

/****************************************************************************************************/
/** Setup **/
/****************************************************************************************************/
void setup() {

    // ------------------------------------------
    /** Initialise PINs                            **/
    // ------------------------------------------

    // LiveSign
    pinMode(kID_LED_Info, OUTPUT);
    digitalWrite(kID_LED_Info, HIGH);

    pinMode(bPin_LED, OUTPUT);
    digitalWrite(bPin_LED, HIGH);

    // ------------------------------------------
    /** init serial                                **/
    // ------------------------------------------

// for ATmega32U4 devices:
#if defined(__AVR_ATmega32U4__)
    // wait for arduino IDE to release all serial ports after upload.
    delay(2000);
#endif

    Serial.begin(115200);

// for ATmega32U4 devices:
#if defined(__AVR_ATmega32U4__)
    // Wait for Serial Connection to be Opend from Host or 6second timeout
    unsigned long ulTimeStamp_Start = millis();
    while ((!Serial) && ((millis() - ulTimeStamp_Start) < 6000)) {
        1;
    }
#endif

    Serial.println();

    Serial.print(F("# Free RAM = "));
    Serial.println(freeRam());

    // ------------------------------------------
    /** Welcom                                     **/
    // ------------------------------------------

    print_info(Serial);

    // ------------------------------------------
    /** start my Fader                             **/
    // ------------------------------------------

    Serial.println(F("slight_FaderLin:"));
    {

        Serial.println(F("\t myFaderOne.begin();"));
        myFaderOne.begin();

        /*
        Serial.println(F("\t DebugOut OFF"));
        byte bTempDebugState = bDebugOut_myFaderOne_Output_Enable;
        bDebugOut_myFaderOne_Output_Enable = 0;


        Serial.println(F("\t myFaderOne welcome fade"));
        myFaderOne.startFadeTo( 1000, memList_A[memList_A__Full]);
        while ( myFaderOne.update() != myFaderOne.state_Finished) {
            //update output with new values
            myFaderOne_Output();
        }
        myFaderOne.update();
        myFaderOne_Output();
        myFaderOne.startFadeTo( 1000, memList_A[memList_A__Off]);
        while ( myFaderOne.update() != myFaderOne.state_Finished) {
            //update output with new values
            myFaderOne_Output();
        }
        myFaderOne.update();
        myFaderOne_Output();

        Serial.println(F("\t DebugOut restore"));
        bDebugOut_myFaderOne_Output_Enable = bTempDebugState;
        */
    }
    Serial.println(F("\t finished."));

    // ------------------------------------------
    /** show Serial Commands                       **/
    // ------------------------------------------

    // reset Serial Debug Input
    memset(sMenu_Input_New, '\0', sizeof(sMenu_Input_New) - 1);
    // print Serial Options
    sMenu_Input_New[0] = '?';
    bMenu_Input_New_FlagComplete = true;

    // ------------------------------------------
    /** GO                                         **/
    // ------------------------------------------

    Serial.println(F("Loop:"));

} /** setup **/

/****************************************************************************************************/
/** Main Loop **/
/****************************************************************************************************/
void loop() {

    // ------------------------------------------
    /** Menu Input                                   **/
    // ------------------------------------------
    // Serial
    handle_SerialReceive();
    check_NewLineComplete();

    // ------------------------------------------
    /** my fader                                     **/
    // ------------------------------------------
    myFaderOne.update();

    // ------------------------------------------
    /** Timed things                                 **/
    // ------------------------------------------

    /*
    // every Nms
    if ( ( millis() - ulTimeStamp_LastAction ) > cwUpdateInterval) {
        ulTimeStamp_LastAction =  millis();

    }
    */

    // ------------------------------------------
    /** Debug Out                                    **/
    // ------------------------------------------

    if ((millis() - ulDebugOut_LiveSign_TimeStamp_LastAction) >
        cwDebugOut_LiveSign_UpdateInterval) {
        ulDebugOut_LiveSign_TimeStamp_LastAction = millis();

        if (bDebugOut_LiveSign_Serial_Enabled) {
            Serial.print(millis());
            Serial.print(F("ms;"));
            Serial.print(F("  free RAM = "));
            Serial.println(freeRam());
        }

        if (bDebugOut_LiveSign_LED_Enabled) {
            bLEDState = !bLEDState;
            if (bLEDState) {
                // set LED to HIGH
                digitalWrite(kID_LED_Info, HIGH);
            } else {
                // set LED to LOW
                digitalWrite(kID_LED_Info, LOW);
            }
        }
    }

    // ------------------------------------------
    /**                                              **/
    // ------------------------------------------

} /** loop **/

/****************************************************************************************************/
/** THE END **/
/****************************************************************************************************/
