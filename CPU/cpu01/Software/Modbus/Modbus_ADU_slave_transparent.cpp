/*
 * Modbus_glue.cpp
 *
 *  Created on: 12 paü 2020
 *      Author: MrTea
 */
#include <string.h>

#include <Modbus_ADU_slave_transparent.h>
#include "Modbus_Converter_memory.h"

#include "Modbus_RTU.h"
#include "SD_card.h"
#include "State.h"

#include "Modbus_ADU_api.h"



#define __byte_array(x, y) __byte((int *)x, y)



Uint16 *Modbus_ADU_slave_transparent::byte_expand2word(Uint16 *word_array, Uint16 *byte_array, Uint16 byte_offset, Uint16 size)
{
    register Uint16 *destination_pointer = word_array;
    Uint16 *destination_end_pointer = destination_pointer + size;

    register int16 *source_pointer = (int16 *)byte_array;

    while(destination_pointer < destination_end_pointer)
    {
        *destination_pointer++ =  __byte_array(source_pointer, byte_offset++);
    }

    return destination_pointer;
}

void Modbus_ADU_slave_transparent::word_truncate2byte(Uint16 *byte_array, Uint16 *word_array, Uint16 byte_offset, Uint16 size)
{
    register int16 *destination_pointer = (int16 *)byte_array;
    register Uint16 *source_pointer = word_array;
    Uint16 byte_offset_max = byte_offset + size;

    while(byte_offset < byte_offset_max)
    {
        __byte_array(destination_pointer, byte_offset++) = *source_pointer++;
    }
}

void Modbus_ADU_slave_transparent::print_filinfo(Uint16 fno_p)
{
    register Uint16 *fno_address = (Uint16 *)&fno.fsize;
    __byte_array(response, fno_p+0) = __byte_array(fno_address, 0);
    __byte_array(response, fno_p+1) = __byte_array(fno_address, 1);
    __byte_array(response, fno_p+2) = __byte_array(fno_address, 2);
    __byte_array(response, fno_p+3) = __byte_array(fno_address, 3);

    fno_address = (Uint16 *)&fno.fdate;
    __byte_array(response, fno_p+4) = __byte_array(fno_address, 0);
    __byte_array(response, fno_p+5) = __byte_array(fno_address, 1);

    fno_address = (Uint16 *)&fno.ftime;
    __byte_array(response, fno_p+6) = __byte_array(fno_address, 0);
    __byte_array(response, fno_p+7) = __byte_array(fno_address, 1);

    fno_address = (Uint16 *)&fno.fattrib;
    __byte_array(response, fno_p+8) = __byte_array(fno_address, 0);

    word_truncate2byte(response, (Uint16 *)&fno.fname, fno_p+9, 13);
}


void Modbus_ADU_slave_transparent::process_FatFS_request()
{
    function = (enum FatFS_function_enum)__byte_array(request, 0);

    switch(function)
    {
        case FatFS_no_function:
        {
            break;
        }

        case FatFS_f_open:
        {
            Uint16 fresult_p = __byte_array(request, 1);
            //Uint16 fil_p = __byte_array(request, 2);
            Uint16 path_p = __byte_array(request, 3);
            Uint16 mode_p = __byte_array(request, 4);

            Uint16 mode = __byte_array(request, mode_p);

            Uint16 *end;
            end = byte_expand2word((Uint16 *)buffer, request, path_p, FF_SFN_BUF);
            *end = 0;

            if(fresult = f_open(&file, buffer, mode)) f_close(&file);

            __byte_array(response, fresult_p) = fresult;
            break;
        }

        case FatFS_f_close:
        {
            Uint16 fresult_p = __byte_array(request, 1);
            //Uint16 fil_p = __byte_array(request, 2);

            fresult = f_close(&file);

            __byte_array(response, fresult_p) = fresult;
            break;
        }

        case FatFS_f_read:
        {
            Uint16 fresult_p = __byte_array(request, 1);
            //Uint16 fil_p = __byte_array(request, 2);
            Uint16 buff_p = __byte_array(request, 3);
            Uint16 btr_p = __byte_array(request, 4);
            Uint16 br_p = __byte_array(request, 5);

            Uint16 btr = __byte_array(request, btr_p);
            Uint16 br;

            fresult = f_read(&file, buffer, btr, &br);

            word_truncate2byte(response, (Uint16 *)buffer, buff_p, br);

            __byte_array(response, fresult_p) = fresult;
            __byte_array(response, br_p) = br;
            break;
        }

        case FatFS_f_write:
        {
            Uint16 fresult_p = __byte_array(request, 1);
            //Uint16 fil_p = __byte_array(request, 2);
            Uint16 buff_p = __byte_array(request, 3);
            Uint16 btw_p = __byte_array(request, 4);
            Uint16 bw_p = __byte_array(request, 5);

            Uint16 btw = __byte_array(request, btw_p);

            byte_expand2word((Uint16 *)buffer, request, buff_p, btw);

            Uint16 bw;
            fresult = f_write(&file, buffer, btw, &bw);

            __byte_array(response, fresult_p) = fresult;
            __byte_array(response, bw_p) = bw;
            break;
        }

        case FatFS_f_lseek:
        {
            Uint16 fresult_p = __byte_array(request, 1);
            //Uint16 fil_p = __byte_array(request, 2);
            Uint16 ofs_p = __byte_array(request, 3);

            int32 ofs;
            register Uint16 *ofs_address = (Uint16 *)&ofs;
            __byte_array(ofs_address, 0) = __byte_array(request, ofs_p+0);
            __byte_array(ofs_address, 1) = __byte_array(request, ofs_p+1);
            __byte_array(ofs_address, 2) = __byte_array(request, ofs_p+2);
            __byte_array(ofs_address, 3) = __byte_array(request, ofs_p+3);

            fresult = f_lseek(&file, ofs);

            __byte_array(response, fresult_p) = fresult;
            break;
        }

        case FatFS_f_tell:
        {
            Uint16 fptr_p = __byte_array(request, 1);
            //Uint16 fil_p = __byte_array(request, 2);

            int32 fptr = f_tell(&file);

            register Uint16 *fptr_address = (Uint16 *)&fptr;
            __byte_array(response, fptr_p+0) = __byte_array(fptr_address, 0);
            __byte_array(response, fptr_p+1) = __byte_array(fptr_address, 1);
            __byte_array(response, fptr_p+2) = __byte_array(fptr_address, 2);
            __byte_array(response, fptr_p+3) = __byte_array(fptr_address, 3);
            break;
        }

        case FatFS_f_size:
        {
            Uint16 fsize_p = __byte_array(request, 1);
            //Uint16 fil_p = __byte_array(request, 2);

            int32 fsize = f_size(&file);

            register Uint16 *fsize_address = (Uint16 *)&fsize;
            __byte_array(response, fsize_p+0) = __byte_array(fsize_address, 0);
            __byte_array(response, fsize_p+1) = __byte_array(fsize_address, 1);
            __byte_array(response, fsize_p+2) = __byte_array(fsize_address, 2);
            __byte_array(response, fsize_p+3) = __byte_array(fsize_address, 3);
            break;
        }

        case FatFS_f_opendir:
        {
            Uint16 fresult_p = __byte_array(request, 1);
            //Uint16 dir_p = __byte_array(request, 2);
            Uint16 path_p = __byte_array(request, 3);

            Uint16 *end;
            end = byte_expand2word((Uint16 *)buffer, request, path_p, FF_SFN_BUF);
            *end = 0;

            fresult = f_opendir(&directory, buffer);

            __byte_array(response, fresult_p) = fresult;
            break;
        }

        case FatFS_f_closedir:
        {
            Uint16 fresult_p = __byte_array(request, 1);
            //Uint16 dir_p = __byte_array(request, 2);

            fresult = f_closedir(&directory);

            __byte_array(response, fresult_p) = fresult;
            break;
        }

        case FatFS_f_readdir:
        {
            Uint16 fresult_p = __byte_array(request, 1);
            //Uint16 dir_p = __byte_array(request, 2);
            Uint16 fno_p = __byte_array(request, 3);

            fresult = f_readdir(&directory, &fno);

            print_filinfo(fno_p);

            __byte_array(response, fresult_p) = fresult;
            break;
        }

        case FatFS_f_findfirst:
        {
            Uint16 fresult_p = __byte_array(request, 1);
            //Uint16 dir_p = __byte_array(request, 2);
            Uint16 fno_p = __byte_array(request, 3);
            Uint16 path_p = __byte_array(request, 4);
            Uint16 pattern_p = __byte_array(request, 5);

            Uint16 *end;
            end = byte_expand2word((Uint16 *)buffer, request, path_p, FF_SFN_BUF);
            *end = 0;
            end = byte_expand2word((Uint16 *)buffer + MODBUS_BUFFER_SIZE/2, request, pattern_p, FF_SFN_BUF);
            *end = 0;

            fresult = f_findfirst(&directory, &fno, buffer, buffer + MODBUS_BUFFER_SIZE/2);

            print_filinfo(fno_p);

            __byte_array(response, fresult_p) = fresult;
            break;
        }

        case FatFS_f_findnext:
        {
            Uint16 fresult_p = __byte_array(request, 1);
            //Uint16 dir_p = __byte_array(request, 2);
            Uint16 fno_p = __byte_array(request, 3);

            fresult = f_findnext(&directory, &fno);

            print_filinfo(fno_p);

            __byte_array(response, fresult_p) = fresult;
            break;
        }

        case FatFS_f_stat:
        {
            Uint16 fresult_p = __byte_array(request, 1);
            Uint16 path_p = __byte_array(request, 2);
            Uint16 fno_p = __byte_array(request, 3);

            Uint16 *end;
            end = byte_expand2word((Uint16 *)buffer, request, path_p, FF_SFN_BUF);
            *end = 0;

            fresult = f_stat(buffer, &fno);

            print_filinfo(fno_p);

            __byte_array(response, fresult_p) = fresult;
            break;
        }

        case FatFS_f_unlink:
        {
            Uint16 fresult_p = __byte_array(request, 1);
            Uint16 path_p = __byte_array(request, 2);

            Uint16 *end;
            end = byte_expand2word((Uint16 *)buffer, request, path_p, FF_SFN_BUF);
            *end = 0;

            fresult = f_unlink(buffer);

            __byte_array(response, fresult_p) = fresult;
            break;
        }

        case FatFS_f_rename:
        {
            Uint16 fresult_p = __byte_array(request, 1);
            Uint16 path_old_p = __byte_array(request, 2);
            Uint16 path_new_p = __byte_array(request, 3);

            Uint16 *end;
            end = byte_expand2word((Uint16 *)buffer, request, path_old_p, FF_SFN_BUF);
            *end = 0;
            end = byte_expand2word((Uint16 *)buffer + MODBUS_BUFFER_SIZE/2, request, path_new_p, FF_SFN_BUF);
            *end = 0;

            fresult = f_rename(buffer, buffer + MODBUS_BUFFER_SIZE/2);

            __byte_array(response, fresult_p) = fresult;
            break;
        }

        default:
            break;
    }

    __byte_array(request, 0) = FatFS_no_function;
}

Modbus_error_enum_t Modbus_ADU_slave_transparent::Fcn_before_processed()
{
    if(Mdb_slave_ADU.function == Read_Discrete_Inputs)
    {

    }

    if(Mdb_slave_ADU.function == Read_Coils || Mdb_slave_ADU.function == Write_Multiple_Coils || Mdb_slave_ADU.function == Write_Single_Coil)
    {

    }

    if(Mdb_slave_ADU.function == Read_Input_Registers)
    {
        if(Mdb_slave_ADU.end_address >= sizeof(Modbus_Converter.input_registers.FatFS_response))
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
            Modbus_Converter.input_registers.status_master = status_master;
            Modbus_Converter.input_registers.alarm_master = alarm_master;
            Modbus_Converter.input_registers.alarm_master_snapshot = alarm_master_snapshot;
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
            Modbus_Converter.input_registers.rtu_port_id = this->RTU->get_sci_id();
            Modbus_Converter.input_registers.alarm_slave = alarm_slave[0];
            Modbus_Converter.input_registers.alarm_slave_snapshot = alarm_slave_snapshot[0];
            Modbus_Converter.input_registers.status_slave = status_slave[0];
            Modbus_Converter.input_registers.Machine_state = Machine.state;
            Modbus_Converter.input_registers.Temp1 = __fmax(log_slave[0].Temperature.a, log_slave[0].Temperature.b);
            Modbus_Converter.input_registers.Temp2 = __fmax(log_slave[0].Temperature.c, log_slave[0].Temperature.n);
        }
        if(Mdb_slave_ADU.start_address < sizeof(Modbus_Converter.input_registers.FatFS_response))
        {
            memcpy(Modbus_Converter.input_registers.FatFS_response, response, sizeof(Modbus_Converter.input_registers.FatFS_response));
        }
    }

    if(Mdb_slave_ADU.function == Read_Holding_Registers || Mdb_slave_ADU.function == Write_Multiple_Registers || Mdb_slave_ADU.function == Write_Single_Register)
    {
        Modbus_Converter.holding_registers.control_master = control_master;
        Modbus_Converter.holding_registers.control_ext_modbus = control_ext_modbus;
        memcpy(Modbus_Converter.holding_registers.FatFS_request, request, sizeof(Modbus_Converter.holding_registers.FatFS_request));
    }

    return No_Error;
}

void Modbus_ADU_slave_transparent::Fcn_after_processed()
{
    if(Mdb_slave_ADU.function == Write_Multiple_Registers || Mdb_slave_ADU.function == Write_Single_Register)
    {
        memcpy(request, Modbus_Converter.holding_registers.FatFS_request, sizeof(request));
        process_FatFS_request();
        control_master = Modbus_Converter.holding_registers.control_master;
        control_ext_modbus = Modbus_Converter.holding_registers.control_ext_modbus;

        extern Modbus_ADU_slave_general * pModbus_slave_EXT_translated;
        pModbus_slave_EXT_translated->slave_address = control_ext_modbus.fields.ext_server_id;


        RTC_new_time = Modbus_Converter.holding_registers.RTC_new_time;
        Uint16 time_offset = Uint16_offset(Modbus_Converter.holding_registers, RTC_new_time);
        Uint16 time_size = sizeof(Modbus_Converter.holding_registers.RTC_new_time);
        if(Mdb_slave_ADU.start_address <= time_offset && Mdb_slave_ADU.end_address >= time_offset + time_size - 1)
        {
            Machine.save_to_RTC = 1;
        }

        DINT;
        Conv.Id.a = Modbus_Converter.holding_registers.Id.a;
        Conv.Id.b = Modbus_Converter.holding_registers.Id.b;
        Conv.Id.c = Modbus_Converter.holding_registers.Id.c;
        Conv.Iq.a = Modbus_Converter.holding_registers.Iq.a;
        Conv.Iq.b = Modbus_Converter.holding_registers.Iq.b;
        Conv.Iq.c = Modbus_Converter.holding_registers.Iq.c;
        EINT;
        Conv.control_type = Modbus_Converter.holding_registers.control_type;
        Conv.C_conv = Modbus_Converter.holding_registers.C_conv;
    }

    if(Mdb_slave_ADU.function == Read_Input_Registers)
    {
        if(Mdb_slave_ADU.start_address <= sizeof(Modbus_Converter.input_registers.FatFS_response))
        {
            if(control_master.flags.bit.Modbus_FatFS_repeat && function == FatFS_f_read)
            {
                __byte_array(request, 0) = FatFS_f_read;
                process_FatFS_request();
            }
        }
    }

    if(Mdb_slave_ADU.function == Write_Multiple_Coils || Mdb_slave_ADU.function == Write_Single_Coil)
    {

    }
}

Modbus_ADU_slave_transparent::Modbus_ADU_slave_transparent( Uint8 slave_address, Modbus_RTU_class *RTU ): Modbus_ADU_slave_general( slave_address, RTU, NULL, Modbus_translator::translator_disabled ){

}

void Modbus_ADU_slave_transparent::clear(){
    this->function = FatFS_no_function;
    this->fresult = FR_OK;
    memset( this->request, 0, sizeof(this->request));
    memset( this->response, 0, sizeof(this->response));
    memset( this->buffer, 0, sizeof(this->buffer));
    memset( &this->file, 0, sizeof(this->file));
    memset( &this->fno, 0, sizeof(this->fno ));
    memset( &this->directory,0, sizeof(this->directory));
    if( this->RTU )
        memset( this->RTU, 0, sizeof(*this->RTU));

}

