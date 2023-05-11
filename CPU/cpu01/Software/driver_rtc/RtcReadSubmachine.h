/*
 * RtcReadSubmachine.h
 *
 *  Created on: 3 Nov 2020
 *      Author: Piotr
 */

#ifndef SOFTWARE_DRIVER_RTC_RTCREADSUBMACHINE_H_
#define SOFTWARE_DRIVER_RTC_RTCREADSUBMACHINE_H_

#include "Rtc.h"

/**@brief klasa wspomagajaca przeznaczona do odczytu rejestrow z RTC
 *
 * NIE UZYWAC SAMODZIELNIE.
 *
* @note wymaga wykonania dwoch faz:
* @note 1. zapisu na i2c - ustalenie rejestru poczatkowego
* @note 2. odczytu z i2c - odczyt danych
*
* klasa jest uzupelnieniem klasy Rtc,
* wykorzystuje podane przy activate() kolejne 4 stany glownej maszyny stanowej Rtc do wlasnej pracy (ta sama przestrzen stanow).
* Stan przechowuje w Rtc - modyfikuje go.
*
 submaszyna wykorzystuje przetrzen stanow maszyny glownej
     * base_state - stan bazowy, uzywane tez 3 kolejne
     *
     * stan bazowy - wyslanie adresu rejestru do odczytania
     * stan_bazowy+1 - oczekiwnaie na zakonczenie wyslania, rozpoczecie odczytu na i2c
     * stan_bazowy+2 - oczekiwanie na zakonczenie odczytu
     * stan_bazowy+3 - zakonczono odczyt, wynik w this->msg */

class RtcReadSubmachine{

    Rtc *rtc;   /**<@brief nadrzedna maszyna stanowa*/
public:
    bool active; /**<@brief wskazuje czy submaszyna jest aktywna czy nie*/
    Rtc::state_t base_state; /**<@brief stan bazowy w Rtc od ktorego zaczyna sie przestrzen stanow submaszyny*/
    uint16_t reg_nb; /**<@brief adres pierwszego rejestru w RTC do odczytania*/
    uint16_t count;  /**<@brief liczba kolejnych rejestrow do odczytania*/

    msg_buffer msg;  /**<@brief bufor na wymieniane wiadomosci, po odczytaniu tu sa dane*/

    RtcReadSubmachine(): rtc(NULL), active(false), base_state( Rtc::state_invalid),
            reg_nb(0), count(1){};

    /**@brief inicjalizacja submaszyny
     * @param[in] rtc wskaznik do klasy Rtc - maszyny nadrzednej*/
    void init( Rtc * rtc ){
        this->rtc = rtc;
    }

    /**@brief uruchomienie submaszyny
     * @param[in] base_state stan bazowy, aktywacja powoduje przejscie do tego stanu
     * @param[in] reg_nb adres  1. rejestru do odczytania
     * @param[in] count liczba rejestrow do odczytania */
    status_code_t activate( Rtc::state_t base_state, uint16_t reg_nb, uint16_t count );

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
    status_code_t process_event( Rtc::event_t event, void * xdata);
};




#endif /* SOFTWARE_DRIVER_RTC_RTCREADSUBMACHINE_H_ */
