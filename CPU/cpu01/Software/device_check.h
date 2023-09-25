/*
 * device_check.h
 *
 *  Created on: 28 paŸ 2022
 *      Author: Piotr
 */

#ifndef SOFTWARE_DEVICE_CHECK_H_
#define SOFTWARE_DEVICE_CHECK_H_

#define HASHED_DEVICE_ID_LOCATION  0x085FC0 //TODO: sprawdzic lokalizacje
//oraz czy brak EXEONLY nie psuje bezpieczenswa
//ew. dodac funkcje PAI bootloadera, ktora skopiuje wartosci z bootloadera (device-id) do podanej pamieci
//sprawdzic gdzie jest podana pamiec (aby nie uzyto takiego kopiowania z bezp strefy (bootloader) do zlamania bezp.
//
//mozna narzucic gdzie bedzie miejsce do skopiowania danych (zakres adresow) i wymusic taka zmienna w FW w tym miejscu (globalna/statyczna)


static inline void hash_device_id( Uint32 hash_device_id[2] ){
    Uint32 device_id = *(Uint32 *)(0x000703C0 + 0xC);
    hash_device_id[0] = (device_id >> 3)^(device_id<<5)^device_id<<1;
    hash_device_id[1] = (device_id >> 5)^(device_id<<3)^device_id>>1;
}

static inline int is_correct( void ){
    Uint32 hashed_device_id[2];
    hash_device_id( hashed_device_id );
    if( hashed_device_id[0] != ((Uint32*)HASHED_DEVICE_ID_LOCATION)[0] )
        return 0;
    if( hashed_device_id[1] != ((Uint32*)HASHED_DEVICE_ID_LOCATION)[1] )
        return 0;
    return 1;
}

static inline int is_unprogrammed_device_id(void){
    if( ((Uint32*)HASHED_DEVICE_ID_LOCATION)[0] != 0xFFFFFFFFUL )
        return 0;
    if( ((Uint32*)HASHED_DEVICE_ID_LOCATION)[1] != 0xFFFFFFFFUL )
        return 0;
    return 1;
}

void programm_device_id(void);

#endif /* SOFTWARE_DEVICE_CHECK_H_ */
