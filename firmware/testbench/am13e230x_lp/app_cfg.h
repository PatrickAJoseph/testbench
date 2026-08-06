/*
 *  Copyright (C) 2025 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef APP_CFG_H
#define APP_CFG_H

/* SDK includes */
#include "device.h"

/* Syscfg includes */
#include "ti_sdk_dl_config.h"

/* ========================================================================== */
/*                 Common App Definitions                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                 Common App Structures                                     */
/* ========================================================================== */

/* ========================================================================== */
/*                 Common App Buffers                                        */
/* ========================================================================== */

#ifdef ENABLE_SIGNING
static const uint8_t App_SignatureHeader[128] __attribute__((section(".header"), used)) = {
    0x00U /* Dummy initialization — will be updated with actual header binary */
};

static const uint8_t App_SignatureTrailer[44] __attribute__((section(".trailer"), used)) = {
    0x00U /* Dummy initialization — will be updated with actual trailer binary */
};
#endif

/* ========================================================================== */
/*                 Common Function Prototypes                                 */
/* ========================================================================== */

static inline void App_Init()
{
    Device_Init();
    SYSCFG_DL_init();
}

#endif  // APP_CFG_H
