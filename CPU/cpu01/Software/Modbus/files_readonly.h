/*
 * files_readonly.h
 *
 *  Created on: 4 cze 2022
 *      Author: Piotr
 */

#ifndef SOFTWARE_MODBUS_FILES_READONLY_H_
#define SOFTWARE_MODBUS_FILES_READONLY_H_

#include <string.h>
#include "stdafx.h"
#include "status_codes.h"

/**Naglowek pliku*/
struct file_readonly_header{
    Uint16 len;
    Uint16 crc;
    char name[32];
};

/**Tablica naglowkow*/
extern const struct file_readonly_header file_readonly_headers[];

/**wskaznik do tresci*/
typedef const Uint16 * const c_char_ptr_t;

/**tablica wskaznikow do tresci*/
extern c_char_ptr_t files_readonly_data[];

/**liczba plikow*/
extern const Uint16 files_readonly_number;

struct file_data_dsc{
    Uint16 file_nb;
    Uint16 rec_no;
    Uint16 rec_len;
};

/** kopiuje tresc pliku do bufora uint16
 * @param[in] data_dsc opis lokalizacji danych do odczytania
 * @param[out] buffer_out bufor do wpisania danych
 * @param[inout] index miejsce gdzie nalezy zaczac wpisywac, zwracane nast. do wpisywania
 *
 * @note do bufora wpisywnae dane niespakowane
 */
status_code_t read_file_data( struct file_data_dsc * data_dsc, char * buffer_out, Uint16 *index );

/** odczyt danych o pliku, file_id+0x1000
 * @param[in] data_dsc opis lokalizacji danych do odczytania (rec_no = 0, rec_len - ignorowany, file_nb = 0x1000 + file_nb )
 * @param[out] buffer_out bufor do wpisania danych
 * @param[inout] index miejsce gdzie nalezy zaczac wpisywac, zwracane nast. do wpisywania
 *
 * @note do bufora wpisywane znaki niespakowane
 */
status_code_t read_file_header( struct file_data_dsc * data_dsc, char * buffer_out, Uint16 *index );

#endif /* SOFTWARE_MODBUS_FILES_READONLY_H_ */
