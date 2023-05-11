/*--------------------------------------------------------*/
/* F021\Registers.h                                       */
/*                                                        */
/* $Release Date: Sun Sep 29 07:38:45 CDT 2019 $                                       */
/* $Copyright:
// Copyright (C) 2014-2019 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $                                       */ 
/*                                                        */
/* F021 Flash API v1.00                                   */
/*--------------------------------------------------------*/

/*!
    \file F021\Registers.h
    \brief A complete mapping of the F021 Flash Registers

    Allows named access to the F021 Flash Registers.
*/
#ifndef F021_REGISTERS_H_
#define F021_REGISTERS_H_

#include "Types.h"

#if defined(_F2837xD) 
    #include "Registers_C28x.h"
#elif defined(_F2837xS)
    #include "Registers_C28x.h"
#elif defined(_F28004x)
    #include "Registers_C28x.h"	
#elif defined(_C28X)
    #include "Registers_Concerto_C28x.h"
#elif defined(_CONCERTO)
    #include  "Registers_Concerto_Cortex.h"  
#elif defined(_LITTLE_ENDIAN)
    #include "Registers_FMC_LE.h"
#else    
    #include "Registers_FMC_BE.h"
#endif
    
#endif /*F021_REGISTERS_H_*/
