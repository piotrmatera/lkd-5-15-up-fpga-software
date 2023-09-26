/*
 * MosfetCtrlApp.cpp
 *
 *  Created on: 25 lut 2022
 *      Author: Piotr Romaniuk
 */

#include <string.h>
#include "MosfetCtrlApp.h"
#include "IO.h"

MosfetDriver mosfet_driver;

MosfetCtrlApp::MosfetCtrlApp(): state(state_reset){
    for(int h=0; h<2;h++){
        error_state[h] = 0;
        values[h] = 0;
        this->sm_status_info[h].error_line = 0;
        this->sm_status_info[h].i2c_state = 0;
        this->sm_status_info[h].subm_state = 0;
    }
    for(int h=0;h<4;h++)
        this->error_state[h] = 0xDE;
}

status_code_t MosfetCtrlApp::init(){
    status_code_t retc = mosfet_driver.init();
    change_state_to( state_idle );
    return retc;
}


void MosfetCtrlApp::process( void ){
    mosfet_driver.process();
    this->process_event(event_periodic, NULL);
}

const uint16_t driver_mosfet_cfg[CHIP1ED389_CONFIG_REGS_COUNT] = {
//TODO Tu wpisac wartosci rejestow, obecnie wartosci takie jak po reset
    CHIP1ED389_ADDRESS_CHIP, //00 I2CADD: I2C address of gate driver (0x0d)
    CHIP1ED389_ADDRESS_GROUP,//01 I2CGADD: I2c group address of gate driver (0x0d)
    1,      //02 I2CCFOK: I2c adres configruation access

    0,      //03: PSUPR: Input pin filter times for IN, RDYC, FLT_N and I2c (0)
    0,      //04: FCLR:  FLT_N clear behavior by RDYC or timer (0)
    CHIP1ED389_RECOVER_RESTORE|CHIP1ED389_RECOVER_RECOVER,      //05: RECOVER: Input and output configuration recovery modes (0)

    0,      //06: UVLO: UVLO threshold level for VCC2 and VEE2 (0)
    0,      //07: UVSVCC2C: VCC2 soft UVLO enable and threshold level (0)
    0,      //08: UVSVEE2C: VEE2 soft UVLO enable and threshold level (0)
    7,      //09: ADCCFG: ADC enable and compare polarity (3)
        //zmiana w sosunku do default: wlaczenie pomiarow napiec przez ADC (Vcc2, Vee2, GND2)
    0,      //0a: VEXTCFG: CLAMP pin voltage compare limit (0)
    0,      //0b: OTWCFG: Over-temperature warning level and action (0)
    22,   //0c: D1LVL: DESAT disable and DESAT1 voltage theshold level (0x1f)//8
    8,      //0d: D1FILT: DESAT1 filter time and type (8)
    22,   //0e: D2LVL: DESAT2 enable during TLTOff, influence on fault off, and volt. threshold level (0x10)//8
    8,   //0f: D2FILT: DESAT2 filter time and type  (0x3F)
    1,      //10: CNTLIM: DESAT2 event counter limit (1)

    0,      //11: D2CNTDEC: DESAT2 event count down (0)
    5,      //12: DLEBT: DESAT leading edge blanking time (5)
    0,      //13: F2ODLY: Delay from fault event to gate driver off (0)
    0,      //14: DTECOR: DESAT temperature compensation (0)
    0,      //15: DRVFOFF: type of fault switch-off (0)
    0,      //16: DRVCFG: type of normal switch-off and TLTOff gate charge range (0)
    0x4E,   //17: TLTOC1: TLTOff level and ramp A (0x4e)
    0x48,   //18: TLTOC2: TLTOff duration and ramp B (0x48)
    9,      //19: CSSOFCFG: Soft turn-off current (0x09)
    0x22,   //1a: CLCFG: CLAMP and pin monitoring filter time and type, CLAMP output types and disable (0x20)
    0x0C,   //1b: SOTOUT: Switch-off timeout time and fault signaling
 /*0x1C CHIP1ED389_CFGOK*/    1 //1c: CFGOK: Register configruation lock (0) - zapisanie tu 1 powoduje ze ustawienia zostana przeslane na strone izolowana

};

status_code_t MosfetCtrlApp::process_event( event_t event, void * xdata ){
    status_code_t retc = status_ok;

    if( state == state_error && event == event_restart ){
        mosfet_driver.process_event(MosfetDriver::event_restart, NULL);
        change_state_to( state_idle );
        return retc;
    }

    if( state != state_idle && event != event_periodic )
          return err_invalid;

    switch( state ){
    case state_idle:
        switch(event){
        case event_configure:
             change_state_to( state_configuring );
             break;
        case event_get_status:
            change_state_to( state_getting_status );
            break;
        }
        return status_ok;
//---------------------------------------------------------------------
    case state_configuring:
//        error_state[ logpoint_precfg ] = 0;
//        for( loop_cnt = 0; loop_cnt< MAX_MOS_DRIVERS; loop_cnt++ )
//        {
//            mosfet_driver.set_i2c_device_address( CHIP1ED389_ADDRESS_CHIP + loop_cnt);
//            mosfet_driver.process_event(MosfetDriver::event_restart, NULL );
//            this->buffer[0] = 0;
//            MosfetDriver_WriteRegs( CHIP1ED389_CFGOK, 1, this->buffer );
//            MosfetDriver_WriteRegs( CHIP1ED389_I2CCFGOK, 1, this->buffer );
//            this->buffer[0] = CHIP1ED389_ADDRESS_INIT;
//            this->buffer[1] = CHIP1ED389_ADDRESS_INIT;
//            MosfetDriver_WriteRegs( CHIP1ED389_I2CADD, 2, this->buffer );
//            while( MosfetDriver_Status(loop_cnt, &error_state[ logpoint_precfg ]) == status_continue )
//            {
//               SM_YIELD();
//            }
//        }

        memcpy( this->buffer, driver_mosfet_cfg, CHIP1ED389_CONFIG_REGS_COUNT );
        error_state[ logpoint_precfg ] = 0; //tu zbierany stan wykonania dla poszczegolnych ukladow (bity = uklady)


        mosfet_driver.set_i2c_device_address( CHIP1ED389_ADDRESS_INIT );
        //petla po wszystkich ukladach - ustawienie adresow i2c
        // (obecnie takie same ale manipulujac sygnalem na wejsciach driver.IN (=PWMup)
        // mozna zrobic aby kazdy uklad uzyskal inny adres i2c )
        for( loop_cnt = 0; loop_cnt< MAX_MOS_DRIVERS; loop_cnt++ ){
            EMIF_mem.write.PWM_control = 0xFF00 | (1UL << loop_cnt);
            this->buffer[0] = driver_mosfet_cfg[0] + loop_cnt;

            mosfet_driver.process_event(MosfetDriver::event_restart, NULL );

            //I. zapis pierwszej czesci rejestrow - ustawienie adresow
            MosfetDriver_WriteRegs( CHIP1ED389_I2CADD, 3, this->buffer );

            //oczekiwanie na zakonczenie zapisu
            while( MosfetDriver_Status(loop_cnt, &error_state[ logpoint_precfg ]) == status_continue ){
               SM_YIELD(); //uwaga! tu wychodzi i potem wraca w to miejsce
            }

        }
        EMIF_mem.write.PWM_control = 0xFF00;

        //powyzsze moze sie nie udac gdy adres juz ustawiony
        // rozstrzygnac czy zapisuje sie trwale w ukladzie:
        // nietrwale ale gdy jest reset procesora to adres zostaje
        // soft-reset nie kasuje adresow i2c

        error_state[ logpoint_softreset ] = 0;

        //Odblokowanie dostepu do rejestrow
        //musi byc przed soft-reset i nie moga byc razem w jednej transakcji i2c
        for( loop_cnt = 0; loop_cnt< MAX_MOS_DRIVERS; loop_cnt++ ){
            mosfet_driver.set_i2c_device_address( CHIP1ED389_ADDRESS_CHIP + loop_cnt);
            mosfet_driver.process_event(MosfetDriver::event_restart, NULL );
            this->values[0] = 0; //CFGOK.USER_OK=0
            MosfetDriver_WriteRegs( CHIP1ED389_CFGOK, 1, this->values );

            //oczekiwanie na zakonczenie zapisu
            while( MosfetDriver_Status(loop_cnt, &error_state[ logpoint_softreset ]) == status_continue ){
               SM_YIELD(); //uwaga! tu wychodzi i potem wraca w to miejsce
            }
        }


        //UWAGA: gdy recover=restore&recover (albo tylko samo restore) to driver nie odpowiadal na probe odblokwoania
        // adres zmieniony i2c, write  ACK
        // adres CFGOK (=1C)           ACK
        // nowa wartosc =0             NACK
        //Konieczne bylo wylaczenie zasilania Vcc2 (przeniesione wlaczanie zasilania za konfigruacje)

        //petla po wszystkich ukladach - wykonanie soft_reset (moga byc w stanie FAULT)
        for( loop_cnt = 0; loop_cnt< MAX_MOS_DRIVERS; loop_cnt++ ){
            mosfet_driver.set_i2c_device_address( CHIP1ED389_ADDRESS_CHIP + loop_cnt);
            mosfet_driver.process_event(MosfetDriver::event_restart, NULL );
            this->values[0] = CHIP1ED389_CLEARREG__SOFT_RST;//CLEARREG.SOFT_RESET = 1
            MosfetDriver_WriteRegs( CHIP1ED389_CLEARREG, 1, this->values );

            //oczekiwanie na zakonczenie zapisu
            while( MosfetDriver_Status(loop_cnt, &error_state[ logpoint_softreset ]) == status_continue ){
               SM_YIELD(); //uwaga! tu wychodzi i potem wraca w to miejsce
            }
        }

//       //test czy sie skasuje bez unlock
//       //I. odczyt
//       MosfetDriver_ReadRegs( CHIP1ED389_RECOVER, 1, this->values);
//
//       //oczekiwanie na zakonczenie zapisu
//       while( MosfetDriver_Status(loop_cnt, &error_state[1]) == status_continue ){
//           SM_YIELD(); //uwaga! tu wychodzi i potem wraca w to miejsce
//       }



        error_state[0] = 0;

        // TODO byloby dobrze miec jedna magistrale i2c bez multiplekserow
        //wtedy latwiej grupowo ustawiac parametry i sterowac lacznie wszystkimi ukladami

        //petla po wszystkich ukladach
        for( loop_cnt = 0; loop_cnt< MAX_MOS_DRIVERS; loop_cnt++ ){
            //odblokwoanie dostepu do rejestrow (gdy uklad jest juz zaprorgamwoany i byl reset cpu
            // jest juz wczesniej przed SOFT-RESET


            //skasowanie licznikow
            //po resecie cpu i konfigruacji wystepowaly zdarzenia bledow kom. i UVcc2
            //czy to moglo byc spowodowane przez RDYC=0?
            mosfet_driver.set_i2c_device_address( CHIP1ED389_ADDRESS_CHIP + loop_cnt);
            this->values[0] = CHIP1ED389_CLEARREG__D2E_CL
                        | CHIP1ED389_CLEARREG__UV2F_CL
                        | CHIP1ED389_CLEARREG__UV1F_CL
                        | CHIP1ED389_CLEARREG__EVTSI_CL;

            MosfetDriver_WriteRegs( CHIP1ED389_CLEARREG, 1, this->values);
            while( MosfetDriver_Status(loop_cnt, &error_state[0]) == status_continue ){
               SM_YIELD(); //uwaga! tu wychodzi i potem wraca w to miejsce
            }

            if( error_state[0] & (1<<loop_cnt) ){
                mosfet_driver.get_submachine_error_line(&sm_status_info[0]);
                continue; //kolejny uklad
            }


            //II. zapis drugiej czesci rejestrow
            MosfetDriver_WriteRegs( 3, MAX_BUFFER_SIZE, &this->buffer[3]);

            while( MosfetDriver_Status(loop_cnt, &error_state[0]) == status_continue ){
               SM_YIELD(); //uwaga! tu wychodzi i potem wraca w to miejsce
            }

            if( error_state[0] & (1<<loop_cnt) ){
                 mosfet_driver.get_submachine_error_line(&sm_status_info[0]);
                 continue; //kolejny uklad, jesli jakis problem
            }

            //III. zapis 3. czesci rejestrow
            MosfetDriver_WriteRegs( MAX_BUFFER_SIZE+3, CHIP1ED389_CONFIG_REGS_COUNT-MAX_BUFFER_SIZE-3,
                                   &this->buffer[MAX_BUFFER_SIZE+3]);

            //oczekiwanie na zakonczenie zapisu
            while( MosfetDriver_Status(loop_cnt, &error_state[0]) == status_continue ){
               SM_YIELD(); //uwaga! tu wychodzi i potem wraca w to miejsce
            }

            if( error_state[0] & (1<<loop_cnt) ){
                 mosfet_driver.get_submachine_error_line(&sm_status_info[0]);
            }

        }//koniec petli po wszystkich ukladach

        change_state_to( error_state[0] == 0 ? state_idle : state_error);
        mosfet_driver.get_submachine_error_line( &this->sm_status_info[0]);
        return status_ok;
//---------------------------------------------------------------------
    case state_getting_status:
        //warto byloby wszystko zebrac a potem wywolac sd_card.save

        error_state[1] = 0; //tu zbierany stan wykonania dla poszczegolnych ukladow (bity = uklady)
        memset( buffer_in, 0, sizeof(buffer_in)); //[MAX_MOS_DRIVERS][ CHIP1ED389_STATUS_REGS_COUNT ];

       //petla po wszystkich ukladach
       for( loop_cnt = 0; loop_cnt< MAX_MOS_DRIVERS; loop_cnt++ ){
           mosfet_driver.set_i2c_device_address( CHIP1ED389_ADDRESS_CHIP + loop_cnt);
           mosfet_driver.process_event(MosfetDriver::event_restart, NULL );

           //I. odczyt 1. czesci rejestrow
           MosfetDriver_ReadRegs( CHIP1ED389_RDYSTAT, MAX_BUFFER_SIZE, &buffer_in[loop_cnt][0]);

           //oczekiwanie na zakonczenie zapisu
           while( MosfetDriver_Status(loop_cnt, &error_state[1]) == status_continue ){
               SM_YIELD(); //uwaga! tu wychodzi i potem wraca w to miejsce
           }

           if( error_state[1] & (1<<loop_cnt) ){
               mosfet_driver.get_submachine_error_line(&sm_status_info[1]);
               continue; //kolejny uklad, jesli jakis problem
           }

           //II. odczyt 2. czeci rejestrow
           MosfetDriver_ReadRegs( MAX_BUFFER_SIZE+CHIP1ED389_RDYSTAT, CHIP1ED389_STATUS_REGS_COUNT-MAX_BUFFER_SIZE,
                                     &buffer_in[loop_cnt][MAX_BUFFER_SIZE]);

           while( MosfetDriver_Status(loop_cnt, &error_state[1]) == status_continue ){
              SM_YIELD(); //uwaga! tu wychodzi i potem wraca w to miejsce
           }

           if( error_state[1] & (1<<loop_cnt) ){
              mosfet_driver.get_submachine_error_line(&sm_status_info[1]);
           }


       }//koniec petli po wszystkich ukladach


       // DBEUG +++++++++++++++++++++++++++++++++++++++
       //odczyt rejestrow konfigruacyjnych - obserwacja na LA

//       I2cMuxAddress( 0 ); //wybranie ukladu
//       mosfet_driver.process_event(MosfetDriver::event_restart, NULL );
//
//       MosfetDriver_ReadRegs( CHIP1ED389_PSUPR, MAX_BUFFER_SIZE, this->buffer );
//       while( MosfetDriver_Status(0) == status_continue ){
//         SM_YIELD();
//       }
//       MosfetDriver_ReadRegs( CHIP1ED389_PSUPR+MAX_BUFFER_SIZE, 12, &this->buffer[MAX_BUFFER_SIZE] );
//       while( MosfetDriver_Status(0) == status_continue ){
//         SM_YIELD();
//       }

       //----------------------------------------------

       change_state_to( error_state[1] == 0 ? state_idle : state_error);
       mosfet_driver.get_submachine_error_line( &this->sm_status_info[1]);
       return status_ok;

    case state_error:
//TODO obsluga wyjscia ze stanu bledu - na zewnatrz w State.cpp
        break;
    }


    return retc;

}

uint16_t MosfetCtrlApp::buffer_item( uint16_t device, uint16_t reg ){
    if( device < MAX_MOS_DRIVERS  && reg < CHIP1ED389_STATUS_REGS_COUNT )
        return this->buffer_in[device][reg];
    return 255;
}

bool MosfetCtrlApp::finished( void ){
    MosfetCtrlApp::state_t mosfet_state = mosfet_ctrl_app.getState();
    if( mosfet_state == MosfetCtrlApp::state_idle ) return true;
    if( mosfet_state == MosfetCtrlApp::state_error ) return true;
    return false;
}

status_code_t MosfetCtrlApp::MosfetDriver_Status(uint16_t loop_cnt, uint16_t * p_error_state){
    if( mosfet_driver.is_state_idle() ) return status_ok;
    if( mosfet_driver.is_state_error() ){
        (*p_error_state) |= (1<<loop_cnt);
        return err_generic;
    }
    return status_continue;

}

status_code_t MosfetCtrlApp::MosfetDriver_ReadRegs( uint16_t address, uint16_t count, uint16_t * data ){
    this->data_dsc.address = address;
    this->data_dsc.count = count;
    this->data_dsc.data = data;
    return mosfet_driver.process_event(MosfetDriver::event_read_regs, &this->data_dsc );
}

status_code_t MosfetCtrlApp::MosfetDriver_WriteRegs( uint16_t address, uint16_t count, uint16_t * data ){
    this->data_dsc.address = address;
    this->data_dsc.count = count;
    this->data_dsc.data = data;
    return mosfet_driver.process_event(MosfetDriver::event_write_regs, &this->data_dsc );
}


uint16_t MosfetCtrlApp::getErrorsConfig(){
    return this->error_state[logpoint_config];
}

uint16_t MosfetCtrlApp::getErrorsGetStatus(){
    return this->error_state[logpoint_getstatus];
}

uint16_t MosfetCtrlApp::getErrorsConfigPre(){
    return this->error_state[logpoint_precfg];
}

uint16_t MosfetCtrlApp::getErrorsAtPoint(logpoint_t point){
    if( point < (sizeof(error_state)/sizeof(error_state[0])))
        return error_state[point];
    return 0xDEAD;
}

void MosfetCtrlApp::change_state_to( state_t new_state ){
    state = new_state;
    dbg_marker('+');
    dbg_marker(state);
}

MosfetCtrlApp::state_t MosfetCtrlApp::getState(){
       return this->state;
}
