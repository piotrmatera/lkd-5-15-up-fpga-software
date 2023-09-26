/*
 * version.cpp
 *
 *  Created on: 20.04.2020
 *      Author: Piotr
 */

#include "version.h"

extern unsigned start_code;//ze skryptu linkera

/*__attribute__ ((section(".code_id"))) TODO dobrze byloby to umiescic pod stalym ofsetem od poczatku FW
 * na razie jest wzor ktory mozna rozpoznac*/
__attribute__ (( aligned(64) ))
const struct fw_id_descriptor_s fw_descriptor={
    { 0xcafe, 0xdead },
	(uint32_t)&start_code,
	BOARD_ID,
	MODBUS_ID,
	SW_ID,
	DEVICE_TYPE,
	FW_TYPE,
	FW_ID_STRING,
	{ FW_TYPE_HEX,
	  HEX_C( SHA_SHORT ),
	  0,
	  REPO_CLEAN},
	  __DATE__,
	  __TIME__
};


