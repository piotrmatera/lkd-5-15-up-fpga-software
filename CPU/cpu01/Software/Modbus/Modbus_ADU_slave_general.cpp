/*
 * Modbus_ADU_slave_general.cpp
 *
 *  Created on: 8 wrz 2021
 *      Author: Piotr
 */

#include "Modbus_ADU_slave_general.h"
#include "Modbus_ADU_api.h"
#include "Modbus_Converter_memory.h"


Modbus_ADU_slave_general::Modbus_ADU_slave_general( Uint8 slave_address, Modbus_RTU_class *RTU, translator_map * translation_mapping, Modbus_translator::translator_mode_t translator_mode ){
    init( slave_address, RTU, translation_mapping, translator_mode );
}

Modbus_ADU_slave_general::~Modbus_ADU_slave_general(){

}

void Modbus_ADU_slave_general::init( Uint8 slave_address, Modbus_RTU_class *RTU, translator_map * translation_mapping, Modbus_translator::translator_mode_t translator_mode){

    this->translator_mode = translator_mode;
    this->translation_mapping = translation_mapping;
    this->RTU = RTU;
    this->slave_address = slave_address;
}

// + sygnalizacja wykonana do RTU o przetworzeniu
//
//
// mdb_request_valid - + przetworzono (czeka na wyslanei odpowiedz)
// mdb_no_request    -   brak danych
// mdb_request_error = + { mdb_request_too_short - niewlasciwe polecenie,
//                     + { mdb_request_crc_error -
//                     + { - blad testu wstepnego, np. niewlasciwy zakres adresow dla tego urzadzenia
//
// mdb_request_wrong_address - niewlasciwy adres *** - tylko tu nastepne urzadzenie, ktore jest na tym samym RTU
//

mdb_prepare_status_t Modbus_ADU_slave_general::task(){

   if(!RTU->is_data_ready_signalled())
       return mdb_no_request;
   __HOOK_ADU_slave_general_Task_w_data;

   //dalej gdy cos jest do przetworzenia
   Mdb_slave_ADU.data_in = RTU->data_in;
   Mdb_slave_ADU.data_in_length = RTU->data_in_length;

   Modbus_Converter.info.slave_address = this->slave_address;//TODO sprawdzic czy tutaj ta zmiana w Modbus_Converter~slave_address nic nie szkodzi
   mdb_prepare_status_t retc = MdbSlavePrepareRequest(&Modbus_Converter.info, this->translation_mapping, this->translator_mode );

   switch( retc ){
   case mdb_request_valid:
       {
              if( Fcn_before_processed() != No_Error ){
                  RTU->signal_data_processed();
                  return mdb_request_error; //np. niewlasciwy zakres adresow dla tego urzadzenia
              }

              Mdb_slave_ADU.data_out = RTU->data_out;
              MdbSlaveProcessRequest(); //tu sa przetworzone dane z bufora RTU
              RTU->signal_data_processed();

              RTU->data_out_length = Mdb_slave_ADU.data_out_length;
              RTU->send_data();

              Fcn_after_processed();

              //RTU->signal_data_processed(); - to bylo za pozno, Fcn_after_processed() wykonuje operacje na karcie SD
              //                                co prowadzi do przyblokowania tego tasku na okolo 11ms
              //                                w tym czasie przychodzila wiad. z pytaniem o fresult po fopen
              //                                i byla gubiona.
              return mdb_request_valid;
       }


   case mdb_request_wrong_address:
       return mdb_request_wrong_address;

   case mdb_request_too_short:
   case mdb_request_crc_error:
   default:
       RTU->signal_data_processed();
       return mdb_request_error;
   }
//
//
//       return 1; //niewlasciwe polecenie, niewlasciwy adres albo obsluzono polecenie
//   }
//   return 0; //brak danych do obsluzenia
}
