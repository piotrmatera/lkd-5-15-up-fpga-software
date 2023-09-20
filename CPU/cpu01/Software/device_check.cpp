/*
 * device_check.cpp
 *
 *  Created on: 28 paŸ 2022
 *      Author: Piotr
 */
#include "F28x_Project.h"
#include "device_check.h"
#include "FLASH_API/F021_F2837xD_C28x.h"

//cala sekwencja musi byc w RAM, po wyjsciu z AsyncCommand jeszcze nie skonczylo uzywac flash
#pragma CODE_SECTION(".TI.ramfunc")
Fapi_StatusType ProgramPart( Uint32 begin_address, Uint16 data_buffer_start[4] )
{
    EALLOW;
    Fapi_StatusType retv = Fapi_issueProgrammingCommand((Uint32 *)begin_address,data_buffer_start,4,0,0,Fapi_AutoEccGeneration);
    EDIS;
    while(Fapi_checkFsmForReady() == Fapi_Status_FsmBusy);
    return retv;
}

uint32_t crc32b(unsigned char *message, uint32_t crc) {
   int i, j;
   //uint32_t byte, crc, mask;
   uint32_t byte, mask;

   i = 0;
   //crc = 0xFFFFFFFF;
   while (message[i] != 0) {
      byte = message[i];            // Get next byte.
      crc = crc ^ byte;
      for (j = 7; j >= 0; j--) {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      i = i + 1;
   }
   return ~crc;
}

void programm_device_id(void)
{

        EALLOW;
        //DCSM_COMMON_REGS.FLSEM (offset=0)
        // operacje kasowania i programowania dozwolone ze strefy 1 (Security Zone 1)
        *(uint32_t*)0x0005F070 = 0xa501;
        EDIS;

        Uint32 hdevice_id[2];
        Uint16 * data_buffer = (Uint16 *)hdevice_id;

        hash_device_id( hdevice_id );


        SeizeFlashPump();
        EALLOW;
        Fapi_initializeAPI(F021_CPU0_W0_BASE_ADDRESS, 200);
        Fapi_setActiveFlashBank(Fapi_FlashBank0);
        EDIS;

        Uint32 random[2];
        Uint32 crc = 0xFFFF1234;
        unsigned char calib_text[]="CALIBRATION DATA";//napis ma 16 znakow
        unsigned int i = 0;
        for( Uint32 address = HASHED_DEVICE_ID_LOCATION-32; address < HASHED_DEVICE_ID_LOCATION+32; address+=4){
            crc = crc32b( (unsigned char*)hdevice_id, crc );
            random[0] = crc;
            crc = crc32b( (unsigned char*)hdevice_id, crc );
            random[1] = crc;

            if( address < (HASHED_DEVICE_ID_LOCATION-32+4*4) ){
                data_buffer = (Uint16 *)random;
                for(int h=0; h<4; h++){
                    data_buffer[h] = calib_text[i++];
                }
            }

            data_buffer = ( address != HASHED_DEVICE_ID_LOCATION )?
                (Uint16 *)random :
                (Uint16 *)hdevice_id;

            ProgramPart( address, data_buffer );
        }

        ReleaseFlashPump();

        EALLOW;
        //DCSM_COMMON_REGS.FLSEM (offset=0)
        // operacje kasowania i programowania dozwolone z dowolnego miejsca
        // ale w przypadku gdy dotyczy strefy bezp. to musi bys z tej strefy
        *(uint32_t*)0x0005F070 = 0xa500;
        EDIS;

}
