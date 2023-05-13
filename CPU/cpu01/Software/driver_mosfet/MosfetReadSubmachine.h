/*
 * MosfetReadSubmachine.h
 *
 *  Created on: 24 lut 2022
 *      Author: Piotr
 */

#ifndef SOFTWARE_DRIVER_MOSFET_MOSFETREADSUBMACHINE_H_
#define SOFTWARE_DRIVER_MOSFET_MOSFETREADSUBMACHINE_H_

#include "driver_i2c/driver_i2c.h"
#include "Dbg.h"

/**@brief klasa wspomagajaca przeznaczona do odczytu rejestrow ukladu po i2c
 *
 * NIE UZYWAC SAMODZIELNIE.
 *
* @note wymaga wykonania dwoch faz:
* @note 1. zapisu na i2c - ustalenie rejestru poczatkowego
* @note 2. odczytu z i2c - odczyt danych
*
* klasa jest uzupelnieniem klasy glownej MosfetDriver,
*/

class MosfetReadSubmachine{

    i2c_t * i2c;

public:
    //osobna przestrzen stanow submaszyny
     typedef enum sub_state_e{
         state_reset = 0, //niezainicjowana

         state_idle, //1 gotowa do dzialania

         state_start, //2 rozpoczeta sekwencja
         state_waiting4_tx_end, //3 oczekiwanie na zakonczenie zapisu po i2c (adres rejestru ukladu)
         state_waiting4_rx_end, //4 oczekiwanie na zakonczenie odczytu po i2c (tresc rejestrow)
         state_terminated,  //5 zakonczono sekwencje, aby przejsc do state_idle wymaga wywolania fn

         state_error //6 wystapil blad, wymaga zresetowania
     } state_t;

     state_t state;
     uint16_t state_error_line;

     typedef enum event_s{
         event_periodic = 0x10,
         event_restart
     }event_t;


       uint16_t reg_nb; /**<@brief adres pierwszego rejestru do odczytania*/
       uint16_t count;  /**<@brief liczba kolejnych rejestrow do odczytania*/

       msg_buffer msg;  /**<@brief bufor na wymieniane wiadomosci, po odczytaniu tu sa dane*/

       MosfetReadSubmachine(void): i2c(NULL), state(state_reset), state_error_line(0), reg_nb(0), count(1){};

       /**@brief inicjalizacja submaszyny
        * @param[in] supermachine wskaznik do klasy - maszyny nadrzednej*/
       void init( i2c_t * i2c );

       /**@brief uruchomienie submaszyny
        * @param[in] reg_nb adres  1. rejestru do odczytania
        * @param[in] count liczba rejestrow do odczytania */
       status_code_t activate( uint16_t reg_nb, uint16_t count );

       /**@brief funkcja do przetwarzania zdarzen
        * @param[in] event zglaszane zdarzenie
        * @param[in] xdata dane dodatkowe zdarzenia
        * @return zwraca:
        *   status_inactive - maszyna nieaktywna, wtedy nic nie robi
        *   status_terminated - wlasnie zakonczyla dzialanie (mozna to uzyc do rozpoznawania zakonczenia;
        *                       do tego celu nadaje sie rowniez wykrycie osiagniecia stanu base_state+3
        *   status_ok - prawidlowe dzialanie
        *   err_invalid - gdy jest aktyna ale stan jest poza zakresem jej dziedziny
        *   inne - gdy blad zwracany przez i2c.read/write      */
       status_code_t process_event( event_t event, void * xdata);

       /**@brief sygnalizuje aby przelaczyc (zresetowac) maszyne - do stanu idle */
       void switch_to_idle(void);

private:
       void change_state_to( state_t new_state, uint16_t line );

   };






#endif /* SOFTWARE_DRIVER_MOSFET_MOSFETREADSUBMACHINE_H_ */
