/*
 * Modbus_glue_slave.h
 *
 *  Created on: 12 paü 2020
 *      Author: MrTea
 */

#ifndef SOFTWARE_MODBUS_MODBUS_ADU_SLAVE_TRANSPARENT_H_
#define SOFTWARE_MODBUS_MODBUS_ADU_SLAVE_TRANSPARENT_H_

#include "stdafx.h"
#include "Modbus_RTU.h"
#include "ff.h"
#include "Modbus_ADU_slave_general.h"

#define MODBUS_BUFFER_SIZE 256

/** Urzadzenie modbus z mozliwoscia obslugi FatFS*/
class Modbus_ADU_slave_transparent: public Modbus_ADU_slave_general
{
public:
    enum FatFS_function_enum
    {
        FatFS_no_function = 0,
        FatFS_f_open = 1,
        FatFS_f_close = 2,
        FatFS_f_read = 3,
        FatFS_f_write = 4,
        FatFS_f_lseek = 5,
        FatFS_f_truncate = 6,     //not available
        FatFS_f_sync = 7,         //not available
        FatFS_f_forward = 8,      //not available
        FatFS_f_expand = 9,       //not available
        FatFS_f_gets = 10,        //not available
        FatFS_f_putc = 11,        //not available
        FatFS_f_puts = 12,        //not available
        FatFS_f_printf = 13,      //not available
        FatFS_f_tell = 14,
        FatFS_f_eof = 15,         //not available
        FatFS_f_size = 16,
        FatFS_f_error = 17,       //not available
    //Directory Access
        FatFS_f_opendir = 18,
        FatFS_f_closedir = 19,
        FatFS_f_readdir = 20,
        FatFS_f_findfirst = 21,
        FatFS_f_findnext = 22,
    //File and Directory Management
        FatFS_f_stat = 23,
        FatFS_f_unlink = 24,
        FatFS_f_rename = 25,
        FatFS_f_chmod = 26,       //not available
        FatFS_f_utime = 27,       //not available
        FatFS_f_mkdir = 28,       //not available
        FatFS_f_chdir = 29,       //not available
        FatFS_f_chdrive = 30,     //not available
        FatFS_f_getcwd = 31,      //not available
    //Volume Management and System Configuration
        FatFS_f_mount = 32,       //not available
        FatFS_f_mkfs = 33,        //not available
        FatFS_f_fdisk = 34,       //not available
        FatFS_f_getfree = 35,     //not available
        FatFS_f_getlabel = 36,    //not available
        FatFS_f_setlabel = 37,    //not available
        FatFS_f_setcp = 38        //not available
    };



    Modbus_ADU_slave_transparent( Uint8 slave_address, Modbus_RTU_class *RTU );

    void clear();
private:
    Modbus_error_enum_t Fcn_before_processed();
    void Fcn_after_processed();

    void print_filinfo(Uint16 fno_p);
    Uint16 *byte_expand2word(Uint16 *destination_addres, Uint16 *source_address, Uint16 byte_offset, Uint16 size);
    void word_truncate2byte(Uint16 *destination_addres, Uint16 *source_address, Uint16 byte_offset, Uint16 size);
    void process_FatFS_request();

    enum FatFS_function_enum function;
    FRESULT fresult;
    Uint16 request[128];
    Uint16 response[128];

    char buffer[MODBUS_BUFFER_SIZE];
    FIL file;
    FILINFO fno;
    DIR directory;

};





#endif /* SOFTWARE_MODBUS_MODBUS_ADU_SLAVE_TRANSPARENT_H_ */
