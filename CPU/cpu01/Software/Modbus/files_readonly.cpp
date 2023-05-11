/*
 * files_readonly.cpp
 *
 *  Created on: 4 cze 2022
 *      Author: Piotr
 */
#include <stdio.h>
#include "files_readonly.h"
#include "Modbus_ADU_slave.h"
#include "version.h"

#define FILES_READONLY_NB 3

#define PREFIX doc/modbus-maps/

#define FILE_PATH_INC(prefix, x, file) _FILE_INFO_INC(prefix, x, file)
#define _FILE_INFO_INC(prefix, x, file) STRING( prefix##x##/##file )
//#define __FILE_INFO_INC FILE_INFO_INC(PREFIX,MODBUS_ID,file_info.inc)


const Uint16 files_readonly_number = FILES_READONLY_NB;

// gdy przy odczycie gdy wychodzi sie poza tresc pliku zwraca pusta tresc w odpowiedzi

#pragma diag_push
#pragma diag_suppress 1934
/*
"../Software/Modbus/files_readonly.cpp", line 56: warning #1934-D: concatenation with "0x0108/modbus_map_small" in macro "_FILE_INFO_INC" does not create a valid token
"../Software/Modbus/files_readonly.cpp", line 56: warning #1934-D: concatenation with "/modbus_map_small" in macro "_FILE_INFO_INC" does not create a valid token
"../Software/Modbus/files_readonly.cpp", line 56: warning #1934-D: concatenation with "modbus_map_small" in macro "_FILE_INFO_INC" does not create a valid token
 * */
const struct file_readonly_header file_readonly_headers[FILES_READONLY_NB]={
#include FILE_PATH_INC( PREFIX, MODBUS_ID, file_info.inc)
                                                                            //"doc/modbus-maps/"MODBUS_ID"/file_info.inc"
     /*{ 19165, 0xba00, "modbus_map.txt"}, //TODO generowanie CRC i naglowkow automatycznie skryptem
     { 5662,  0x5254, "MODBUS_word.txt"},
     { 3943,  0xb072, "modbus_map.txt.zip"},*/

};
#pragma diag_pop

// aby odczytac powyzsze info dla pliku, nalezy zwiekszyc file_nb o 0x1000
// i odczytac jako zwykly plik.
// zwracana tresc tesktowa:
//  dlugosc[dec, bajty] crc[hex-4pocz. z md5sum] nazwa-pliku
// moze byc zakonczone dodatkowym zerem jesli teskt jest nieparzystej dlugosci

//procedura dodania pliku:
// 1. przygotowac plik do dodania (dbac o maly rozmiar)
// 2. wykonac z linii polecen w git-bashu w katalogu cpu01:
//    $ py scripts/conv_txt_to_C.py doc/modbus-maps/0x0106/modbus_word_small.txt > doc/modbus-maps/0x0106/modbus_map_small.inc
// 3. z pocztakowej tresci pliku odczytac dlugosc i crc
// 4. dopisac do file_readonly_headers
// 5. utworzyc jak ponizej przez include wlaczenie pliku
// 6. zwiekszyc rozmiar FILES_READONLY_NB

#pragma diag_push
#pragma diag_suppress 1934

#pragma DATA_SECTION(".files_ro")
static const Uint16 file1[]={
//0x1234, 0xabcd, 0xdead, 0xb006, 0xddbb
  //do pakowania dwoch znakow w Uint16 uzyc skryptu ./scripts/conv_txt_to_C.py
#include FILE_PATH_INC( PREFIX, MODBUS_ID, modbus_map_small.txt.inc)
};

#pragma DATA_SECTION(".files_ro")
static const Uint16 file2[]={
#include FILE_PATH_INC( PREFIX, MODBUS_ID, MODBUS_word_small.txt.inc)
};

#pragma DATA_SECTION(".files_ro")
static const Uint16 file3[]={
#include FILE_PATH_INC( PREFIX, MODBUS_ID, modbus_map_small.zip.inc)
};

#pragma diag_pop

c_char_ptr_t files_readonly_data[FILES_READONLY_NB]={
     file1,
     file2,
     file3,

};

status_code_t read_file_data( struct file_data_dsc * data_dsc, char * buffer_out, Uint16 *index )
{
    if( data_dsc->file_nb > files_readonly_number || data_dsc->file_nb == 0 )
        return err_invalid;

    Uint16 file_id = data_dsc->file_nb - 1;

    const Uint16 * pdata = files_readonly_data[ file_id ];

    //Uint16 copied_data = 0;
    //Uint16 length_words = (file_readonly_headers[ file_id ].len+1)/2;

    Uint16 file_length_bytes = file_readonly_headers[ file_id ].len;

    const Uint16 *precord = &pdata[ data_dsc->rec_no ];
    Uint16 data16;
    for( int i=0; i<data_dsc->rec_len*2; i++){
        if( 2*data_dsc->rec_no + i >= file_length_bytes )
            break;

        if( (i&1) == 0){
            data16 = precord[ i/2 ];
            buffer_out[ (*index)++ ] = data16>>8;
        }else{
            buffer_out[ (*index)++ ] = data16 & 0xFF;
        }
    }
    //Uwaga: gdy wielkosc bloku ma nieparzysta dlugosc, zwraca sa tylko wlasciwe dane
    //natomiast w polu rsp::rec_len[W] jest zaokraglona w gore liczba slow, chociaz ostatnie jest niepelne

    return status_ok; //Uwaga! moze zwrocic puste dane
}



/** odczyt danych o pliku, file_id+0x1000*/
status_code_t read_file_header( struct file_data_dsc * data_dsc, char * buffer_out, Uint16 *index )
{
    if( data_dsc->file_nb > (files_readonly_number+MODBUS_FILE_HEADER_OFFSET)
      || data_dsc->file_nb <= MODBUS_FILE_HEADER_OFFSET
      || data_dsc->rec_no != 0)
        return err_invalid;

    Uint16 file_id = data_dsc->file_nb - MODBUS_FILE_HEADER_OFFSET - 1;

    const struct file_readonly_header * f_hdr = &file_readonly_headers[ file_id ];

    Uint16 n = snprintf( &buffer_out[ (*index) ], 253-(*index), "%u %04x %s",
                f_hdr->len, f_hdr->crc, f_hdr->name);

    (*index) += n;

    return status_ok; //Uwaga! moze zwrocic puste dane
}

