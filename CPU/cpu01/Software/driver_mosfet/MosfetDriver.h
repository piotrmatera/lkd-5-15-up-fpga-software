/*
 * MosfetDriver.h
 *
 *  Created on: 24 lut 2022
 *      Author: Piotr Romaniuk
 */

#ifndef SOFTWARE_DRIVER_MOSFET_MOSFETDRIVER_H_
#define SOFTWARE_DRIVER_MOSFET_MOSFETDRIVER_H_


#include "driver_i2c/driver_i2c.h"
#include "chip1ed389mu12m.h"
#include "MosfetReadSubmachine.h"


/** @brief klasa do obslugi drivera mosfetow
 *
 * TO JEST WLASCIWE API
 *
 * @note nie uzywa przerwan tylko metode przegladania
 * */
class MosfetDriver{

    // wiadomosc uzywana do realizacji polecenia zapisu do rejestrow
    msg_buffer msg_write_reg = {1,0,{0}};

    /**@brief submaszyna stanowa uzywana do odczytu z rejestrow
     * jest niezbedna bo odczyt rejestru wymaga zapisu (w sensie operacji i2c) adresu,
     * a potem bez wysylania i2c.stop wykonania transakcji odczytu (w sensie i2c) danych*/
    MosfetReadSubmachine submachine_read_reg;

public:

    /**@brief typ opisujacy rodzaje zdarzen*/
    typedef enum{
        event_periodic = 0x10, /**<@brief zdarzenie wysylane okresowo, nie wysylac samodzielnie, do uzytku wewnetrznego*/

     //zdarzenia odpowiadajace poleceniom:
        /**@brief zadanie odczytania rejestrow, w xdata podac &xdata_event_regs
         * .count nie moze przekraczac MAX_BUFFER_SIZE
         * bufor data musi istniec przez cala operacje odczytu */
        event_read_regs,

        /**@brief zadanie odczytania rejestrow, w xdata podac &xdata_event_regs
         * .count nie moze przekraczac MAX_BUFFER_SIZE
         * bufor data musi istniec przez cala operacje odczytu */
        event_write_regs,

        /**@brief resetowanie po wystapieniu bledu (wejsciu w stan state_error)*/
        event_restart,

    }event_t;

    struct xdata_event_regs{
        uint16_t count;
        uint16_t address;
        uint16_t *data; //musi byc podczepione na zewnatrz gdy zapis (UWAGA! ten bufor musi istniec az do zakonczenia transakcji na i2c)
    };


public:

    /**@brief konstruktor - inicjalizacja pol klas, rowniez klasy submaszyny*/
    MosfetDriver();

    /**@brief Inicjalizacja */
    status_code_t init();

    /**@brief ogolna funkcja do wyslania polecen w postaci zdarzenia
     * @param[in] event wysylane zdarzenie - odpowiada roznym poleceniom
     * @param[in] xdata argument zdarzenia - obecnie tylko dla event_setup_time
     * @return status_ok gdy wlasciwie przekazano, uwaga! to nie znaczy jeszcze prawidlowego wykonania - bo wykonanie jest pozniej*/
    status_code_t process_event( event_t event, void * xdata = NULL);


    /**@brief funkcja zastepujaca przerwania, nalezy ja wywolywac okresowo
     * wtedy wykonywane sa reakcje na zmiany stanu i2c (np. wyslanie danych)*/
    void process( void );

    /**@brief ustawia adres i2c do nastepnej transmisji*/
    status_code_t set_i2c_device_address( uint16_t addr );

    typedef struct sm_status_s{
            uint16_t error_line;
            uint16_t subm_state;
            uint16_t i2c_state;
        } sm_status_t;

    /**@brief pobiera informacje o bledzie*/
    void get_submachine_error_line( sm_status_t * state_info ){
        state_info->error_line = submachine_read_reg.state_error_line;
        state_info->subm_state = submachine_read_reg.state;
        state_info->i2c_state = i2c.state;
    }

private:

    struct xdata_event_regs read_buf;

    /**@brief ostatnio odczytany status
     * @note aby odczytac nalezy uzyc: get_last_status() */
   // chip1ed389_status_regs_t chip_status;

    /**@brief interfejs I2C do komunikacji z ukladem
     * @note ukrywa specyfike dla tego procesora; dostarcza transakcje read oraz write na i2c  */
    i2c_t i2c;

    /**@brief stany glownej maszyny stanowej klasy Rtc*/
    typedef enum{
        state_reset = 0,    //0 stan - zanim zostanie wywolane init

        state_idle,         //1 przyjmowanie polecen

        //w trakcie dalszych stanow nie przyjmuje polecen, sa ciagiem sekwencji realizujacych polecenia

        state_read_regs,    //2
        state_write_regs,   //3

        state_error//4
    }state_t;

    /**@brief stan */
    state_t state;

    /**@brief funkcja zmiany stanu maszyny stanowej, wspiera logowanie zachowania
     * @param[in] new_state nowy stan na ktory nalezy sie przelaczyc */
    void change_state_to( state_t new_state );

    /**@brief funkcja pomocnicza wywolywana przez maszyne stanowa gdy odczytano status,
    * @note kopiuje go do ...*/
    void notify_read_reg( msg_buffer * msg );

public:
    bool is_state_idle(){
            return (state == state_idle);
    }

    bool is_state_error(){
                return (state == state_error);
    }


};


extern MosfetDriver mosfet_driver;



#endif /* SOFTWARE_DRIVER_MOSFET_MOSFETDRIVER_H_ */
