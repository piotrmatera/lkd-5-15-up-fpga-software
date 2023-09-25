/*
 * dbg_calibration.h
 *
 *  Created on: 25 wrz 2023
 *      Author: Piotr
 */

#ifndef SOFTWARE_DEBUG_DBG_CALIBRATION_H_
#define SOFTWARE_DEBUG_DBG_CALIBRATION_H_

#include <stdio.h>

#if FW_FOR_CALIBRATION
//tylko w wersji kodu do kalibracji - konfiguracja budowania projektu Calibration-for-encrypted-fw

//wypisywanie komunikatow na Modbus_ext, jak printf
# define dbg_printf( frm, ...) \
        do{\
            snprintf( _dbg_buffer, 256, frm ,## __VA_ARGS__ );\
            dbg_log( _dbg_buffer );\
        }while(0)

//bufor do tworzenia komunikatu
extern char _dbg_buffer[256];

//funkcja wypisujaca na Modbus_ext
void dbg_log( char * txt);

//zmiana przypisania wyjsc do przekaznikow, na CPU01
void Cpu01_relays_control(void);

//powrot do wlasciwej konfigruacji (steruje CLA)
void Setup_again_normal_relays_control(void);

//pomocnicze makro do zmiany konfiguracji koncowki
#define __GPIO_Setup(i, _cfg)                           \
    do{  \
        GPIO_WritePin(i, _cfg.defval);                  \
        GPIO_SetupPinMux(i, _cfg.cpucla, _cfg.mux);     \
        GPIO_SetupPinOptions(i, _cfg.dir, _cfg.options);\
    }while(0)



#else
//dla normalnej kompilacji ponizsze funkcje sa puste
# define __NOTHING do{}while(0)
# define dbg_printf( frm, ...) __NOTHING
# define  dbg_log( _txt )      __NOTHING
# define __GPIO_Setup(i, _cfg) __NOTHING
# define Cpu01_relays_control() __NOTHING
# define Setup_again_normal_relays_control() __NOTHING

#endif

// numery koncowek przekaznkow
extern const Uint16 relays_pin[];

// liczba koncowek w relays_pin[]
extern const Uint16 relays_nb;

#endif /* SOFTWARE_DEBUG_DBG_CALIBRATION_H_ */
