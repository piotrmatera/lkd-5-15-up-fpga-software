/*
 * Modbus_ADU_slave_translated.cpp
 *
 *  Created on: 8 wrz 2021
 *      Author: Piotr
 */


#include "State_master.h"
#include "State_slave.h"
#include "Modbus_ADU_slave_translated.h"
#include "Modbus_Converter_memory.h"

#include "Modbus_RTU.h"
#include "SD_card.h"
#include "Modbus_ADU_api.h"

Modbus_ADU_slave_translated::Modbus_ADU_slave_translated( Uint8 slave_address,
                                                          Modbus_RTU_class *RTU,
                                                          translator_map * translation_mapping,
                                                          Modbus_translator::translator_mode_t translator_mode )
                            : Modbus_ADU_slave_general( slave_address, RTU, translation_mapping, translator_mode ){

}



void Modbus_ADU_slave_translated::clear(){

}

Modbus_error_enum_t Modbus_ADU_slave_translated::Fcn_before_processed(){
    if(  (Mdb_slave_ADU.function != Read_Input_Registers)
      && (Mdb_slave_ADU.function != Report_Server_ID)
      && (Mdb_slave_ADU.function != Diagnostics )){
            return Illegal_Function;
    }
//    if(Mdb_slave_ADU.function == Read_Discrete_Inputs)
//        {
//
//        }
//
//        if(Mdb_slave_ADU.function == Read_Coils || Mdb_slave_ADU.function == Write_Multiple_Coils || Mdb_slave_ADU.function == Write_Single_Coil)
//        {
//
//        }

        if(Mdb_slave_ADU.function == Read_Input_Registers)
        {
                Fast_copy_modbus_CPUasm();
                DINT_copy_CPUasm((Uint16 *)&Modbus_Converter.input_registers.Energy_meter.P_p,  (Uint16 *)&Energy_meter.upper.P_p,  sizeof(Modbus_Converter.input_registers.Energy_meter.P_p));
                DINT_copy_CPUasm((Uint16 *)&Modbus_Converter.input_registers.Energy_meter.P_n,  (Uint16 *)&Energy_meter.upper.P_n,  sizeof(Modbus_Converter.input_registers.Energy_meter.P_n));
                DINT_copy_CPUasm((Uint16 *)&Modbus_Converter.input_registers.Energy_meter.QI,   (Uint16 *)&Energy_meter.upper.QI,   sizeof(Modbus_Converter.input_registers.Energy_meter.QI));
                DINT_copy_CPUasm((Uint16 *)&Modbus_Converter.input_registers.Energy_meter.QII,  (Uint16 *)&Energy_meter.upper.QII,  sizeof(Modbus_Converter.input_registers.Energy_meter.QII));
                DINT_copy_CPUasm((Uint16 *)&Modbus_Converter.input_registers.Energy_meter.QIII, (Uint16 *)&Energy_meter.upper.QIII, sizeof(Modbus_Converter.input_registers.Energy_meter.QIII));
                DINT_copy_CPUasm((Uint16 *)&Modbus_Converter.input_registers.Energy_meter.QIV,  (Uint16 *)&Energy_meter.upper.QIV,  sizeof(Modbus_Converter.input_registers.Energy_meter.QIV));
                DINT_copy_CPUasm((Uint16 *)&Modbus_Converter.input_registers.Energy_meter.sum,  (Uint16 *)&Energy_meter.upper.sum,  sizeof(Modbus_Converter.input_registers.Energy_meter.sum));
                Modbus_Converter.input_registers.Energy_meter_algebraic_sum.P_p = Modbus_Converter.input_registers.Energy_meter.P_p[0]
                                                                                + Modbus_Converter.input_registers.Energy_meter.P_p[1]
                                                                                + Modbus_Converter.input_registers.Energy_meter.P_p[2];
                Modbus_Converter.input_registers.Energy_meter_algebraic_sum.P_n = Modbus_Converter.input_registers.Energy_meter.P_n[0]
                                                                                + Modbus_Converter.input_registers.Energy_meter.P_n[1]
                                                                                + Modbus_Converter.input_registers.Energy_meter.P_n[2];
                Modbus_Converter.input_registers.Energy_meter_algebraic_sum.QI = Modbus_Converter.input_registers.Energy_meter.QI[0]
                                                                               + Modbus_Converter.input_registers.Energy_meter.QI[1]
                                                                               + Modbus_Converter.input_registers.Energy_meter.QI[2];
                Modbus_Converter.input_registers.Energy_meter_algebraic_sum.QII = Modbus_Converter.input_registers.Energy_meter.QII[0]
                                                                                + Modbus_Converter.input_registers.Energy_meter.QII[1]
                                                                                + Modbus_Converter.input_registers.Energy_meter.QII[2];
                Modbus_Converter.input_registers.Energy_meter_algebraic_sum.QIII = Modbus_Converter.input_registers.Energy_meter.QIII[0]
                                                                                 + Modbus_Converter.input_registers.Energy_meter.QIII[1]
                                                                                 + Modbus_Converter.input_registers.Energy_meter.QIII[2];
                Modbus_Converter.input_registers.Energy_meter_algebraic_sum.QIV = Modbus_Converter.input_registers.Energy_meter.QIV[0]
                                                                                + Modbus_Converter.input_registers.Energy_meter.QIV[1]
                                                                                + Modbus_Converter.input_registers.Energy_meter.QIV[2];

                Modbus_Converter.input_registers.file_number_logs = SD_card.file_number_logs;
                Modbus_Converter.input_registers.file_number_errors = SD_card.file_number_errors;
                Modbus_Converter.input_registers.status_ACDC = status_ACDC;
//                Modbus_Converter.input_registers.alarm_master = alarm_master;
//                Modbus_Converter.input_registers.alarm_master_snapshot = alarm_master_snapshot;
                Modbus_Converter.input_registers.L_grid_previous[0] = L_grid_meas.L_grid_previous[0];
                Modbus_Converter.input_registers.L_grid_previous[1] = L_grid_meas.L_grid_previous[1];
                Modbus_Converter.input_registers.L_grid_previous[2] = L_grid_meas.L_grid_previous[2];
                Modbus_Converter.input_registers.L_grid_previous[3] = L_grid_meas.L_grid_previous[3];
                Modbus_Converter.input_registers.L_grid_previous[4] = L_grid_meas.L_grid_previous[4];
                Modbus_Converter.input_registers.L_grid_previous[5] = L_grid_meas.L_grid_previous[5];
                Modbus_Converter.input_registers.L_grid_previous[6] = L_grid_meas.L_grid_previous[6];
                Modbus_Converter.input_registers.L_grid_previous[7] = L_grid_meas.L_grid_previous[7];
                Modbus_Converter.input_registers.L_grid_previous[8] = L_grid_meas.L_grid_previous[8];
                Modbus_Converter.input_registers.L_grid_previous[9] = L_grid_meas.L_grid_previous[9];
                Modbus_Converter.input_registers.RTC_current_time = RTC_current_time;
        }

//        if(Mdb_slave_ADU.function == Read_Holding_Registers || Mdb_slave_ADU.function == Write_Multiple_Registers || Mdb_slave_ADU.function == Write_Single_Register)
//        {
//            Modbus_Converter.holding_registers.control = control;
//            memcpy(Modbus_Converter.holding_registers.FatFS_request, request, sizeof(Modbus_Converter.holding_registers.FatFS_request));
//        }
        return No_Error;
}

void Modbus_ADU_slave_translated::Fcn_after_processed(){

}


