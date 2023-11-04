/*
 * version.h
 *
 *  Created on: 20.04.2020
 *      Author: Piotr
 */

#ifndef VERSION_H_
#define VERSION_H_

#include <stdint.h>
#include "common_utils.h"
#include "version-id.h"

#define MODBUS_ID 0x0310 //wersja mapy modbus

#define BOARD_ID 0x0303 //UWAGA!!! to oznaczenie jest uzywane jako numer wersji plytki CPU i musi byc zapisane w EEPROMie
                        //         bootloader kontroluje czy to jest zgone inaczaj nie zaladuje FW
                        // wersja z testem obecnie w galezi: repo bootloder: dev-pr-bootloader-fpga-skjee-encrypted-w-eeprom-hw-check
#define SW_ID 0x0301

//tutaj ustawiac moc
#define DEVICE_TYPE "SKJEE-5-25"

//L=Loaded_FW, B=Basic_FW(no BLD)

#define FW_TYPE 'L' //gdy wersja obslugiwana przez bootloader
//#define FW_TYPE 'B' //gdy wersja samodzielna TODO powiazac z definicja ze skryptu linkera
#if FW_TYPE == 'L'
# define FW_TYPE_HEX 0xD
#elif FW_TYPE == 'B'
# define FW_TYPE_HEX 0xB
#else
# error Zdefiniuj typ FW
#endif

#if (REPO_CLEAN == 0 )
# define REPO_CLEAN_SYMBOL "!"
#else
# define REPO_CLEAN_SYMBOL "."
#endif

#define FW_ID_STRING REPO_CLEAN_SYMBOL STRING(SHA_SHORT)

struct fw_id_descriptor_s {
    uint32_t marker[2];
	uint32_t start_address;
	uint16_t board_id;
	uint16_t modbus_id;
	uint16_t software_id;
	const char  dev_type[16];
	const char  fw_type;
	const char  sha_id[15];
	struct dsc_s{
	    uint32_t    type:4;
	    uint32_t    sha_hex:24;
	    uint32_t    rsvd:3;
	    uint32_t    clean:1;
	} dsc;
	const char * build_date;
	const char * build_time;

};

extern const struct fw_id_descriptor_s fw_descriptor;

#endif /* VERSION_H_ */
