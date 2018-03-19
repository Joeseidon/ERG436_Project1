/* --COPYRIGHT--,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//**************************************************************
// structure.c
//
//! \brief Contians definitions for element and sensor structures
//!        used in associated application.
//!
//!        Author: a0224638, 9/10/2015
//!
//**************************************************************

#include "structure.h"

//! Structure definition for my_buttons
const struct Element up_element = {
    //Use P4.1
    .inputBits = CAPTIOPOSEL_2 + CAPTIOPISEL_7,
    //set maxResponse and threshold
    .maxResponse = 7000+1500,
    .threshold = 3200
};
const struct Element down_element = {
    //Use P4.1
    .inputBits = CAPTIOPOSEL_2 + CAPTIOPISEL_4,
    //set maxResponse and threshold
    .maxResponse = 7000+3000,
    .threshold = 3000
};
const struct Element select_element = {
    //Use P4.1
    .inputBits = CAPTIOPOSEL_2 + CAPTIOPISEL_6,
    //set maxResponse and threshold
    .maxResponse = 7000+3000,
    .threshold = 2500
};

//! Define the elements used for button use detection
const struct Sensor my_keys ={
    .halDefinition = RO_CTIO_TA2_WDTA,
    //set CAPTIO0CTL register for input CAPTIO config
    .inputCaptioctlRegister = (uint16_t *)&CAPTIO0CTL,
    .numElements = 3,
    .baseOffset = 0,
    //pointer to element
    .arrayPtr[0] = &down_element,
    .arrayPtr[1] = &up_element,
    .arrayPtr[2] = &select_element,
    //Timer information
    .measGateSource = GATE_WDTA_VLO,
    .accumulationCycles = WDTA_GATE_64

};
