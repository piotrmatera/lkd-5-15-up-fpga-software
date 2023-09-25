/*
 * dbg_calibration.cpp
 *
 *  Created on: 25 wrz 2023
 *      Author: Piotr
 */
#include <string.h>
#include "Modbus_devices.h"
#include "dbg_calibration.h"

#if FW_FOR_CALIBRATION

void dbg_log( char * txt){
  while( Modbus_slave_EXT.RTU->state != Modbus_RTU_class::Modbus_RTU_idle ){
      Modbus_slave_EXT.RTU->interrupt_task();
    }
  //TODO ddoac zabezpieczenie przed przepelnieniem bufora
    strcpy(Modbus_slave_EXT.RTU->data_out, txt);//"UUUABCDEFGHIJKLMN");
    Modbus_slave_EXT.RTU->data_out_length = strlen(txt);
    Modbus_slave_EXT.RTU->send_data();
}

char _dbg_buffer[256];


void Cpu01_relays_control(void){

            const struct GPIO_struct cfg ={ LOW, MUX0, CPU1_IO, OUTPUT, PUSHPULL };

            for(Uint16 i=0; i<relays_nb; i++){
                __GPIO_Setup( relays_pin[i], cfg );
            }

//            const struct GPIO_struct cfg2 ={ HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL };
//            __GPIO_Setup( RELAY_EN, cfg2 );

}
#undef __GPIO_Setup

void Setup_again_normal_relays_control(void){
    for(Uint16 i=0;i<relays_nb;i++){

            Uint16 p = relays_pin[i];
            GPIO_CLEAR( p );
            GPIO_Setup( p );
        }
//        GPIO_Setup( RELAY_EN );

}

const Uint16 relays_pin[]={
                                   C_SS_RLY_L1_CM,
                                   GR_RLY_L1_CM,
                                   C_SS_RLY_L2_CM,
                                   GR_RLY_L2_CM,
                                   C_SS_RLY_L3_CM,
                                   GR_RLY_L3_CM,
                                   C_SS_RLY_N_CM,
                                   GR_RLY_N_CM
};

const Uint16 relays_nb = sizeof(relays_pin)/sizeof(relays_pin[0]);

#endif

