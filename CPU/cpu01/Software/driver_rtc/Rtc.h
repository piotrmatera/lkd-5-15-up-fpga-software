/*
 * rtc.h
 *
 *  Created on: 2 Nov 2020
 *      Author: Piotr
 */

#ifndef SOFTWARE_DRIVER_RTC_RTC_H_
#define SOFTWARE_DRIVER_RTC_RTC_H_

#include "driver_i2c/driver_i2c.h"
#include "mcp7940n.h"

class RtcReadSubmachine; //forward declaration

/** @brief klasa do obslugi RTC, wspolpracuje z MCP7940N
 *
 * TO JEST WLASCIWE API
 *
 * @note nie uzywa przerwan tylko metode przegladania
 * */
class Rtc{
friend class RtcReadSubmachine; //klasa SubmachineRead musi miec dostep do skladowych prywatnych, manipuluje stanem Rtc

// wiadomosci uzywane do realizacji polecen
    /**@brief wiadomosc do uruchomienia zegara w RTC, wlaczenia baterii i ustawienia czasu domyslnego 0:00:00 pn {20}00-01-01
     * @note uzywane przy pierwszym uruchomieniu, gdy RTC nie pracowalo, albo po wymianie baterii
     */
    msg_buffer msg_start_rtc0 = { 8, 0, {MCP7940N_SEC_REG, MCP7940N_ST_MASK|0x00 /*sec*/, 0x00/*min*/, 0x00/*hr*/,
                                         MCP7940N_WEEKDAY_VBATEN_MASK|0x01, 0x01/*day*/, 0x01/*month*/, 0x00 /*year*/}};

    /**@brief wiadomosc do zatrzymania oscylatora w RTC */
    msg_buffer msg_stop_rtc  = { 2, 0, {MCP7940N_SEC_REG, 0 }};

    /**@brief wiadomosc do ustawiania nowego czasu*/
    msg_buffer msg_new_time = { 8, 0, {MCP7940N_SEC_REG, 0x00, 0x00, 0x00,
                                            MCP7940N_WEEKDAY_VBATEN_MASK|0x00, 0x00, 0x00, 0x00 }};

public:

    /**@brief typ opisujacy rodzaje zdarzen*/
    typedef enum{

        event_periodic = 0x10, /**<@brief zdarzenie wysylane okresowo, nie wysylac samodzielnie, do uzytku wewnetrznego*/

        //zdarzenia odpowiadajace poleceniom:
        event_init = 0x20, /**<@brief wykonanie inicjalizacji interfejsow, wlaczenie RTC jesli wylaczone (+ustawienie domyslnego czasu)*/

        event_read_time, /**<@brief zlecenie odczytania czasu, mozna go potem odczytac przez: get_last_time()*/

        /**@brief zlecenie ustawienia nowego czasu, jako argument nalezy podac wskaznik do struktury Rtc::datetime_bcd_s
        * @note wykonuje sekwencje zapobiegajaca przewinieciu:
        * @note    1) zatrzymanie RTC (ST=0)
        * @note    2) odczekanie na wylaczenie oscylatora, az OSCRUN=0
        * @note    3) ustawienie czasu z wlaczeniem RTC (ST=1)             *
        */
        event_setup_time_bcd,

        /**@brief  zlecenie ustawienia nowego czasu, jako argument nalezy podac wskaznik do struktury Rtc::datetime_s*/
        event_setup_time
    }event_t;

public:

    /**@brief struktura do wymiany danych - opisu daty i czasu (wersja w kodowaniu BCD*/
    struct datetime_bcd_s{
        uint16_t sec;
        uint16_t min;
        uint16_t hour;
        uint16_t dayofweek;
        uint16_t day;
        uint16_t month;
        uint16_t year; //rok = dwie ost. cyfry z roku 2021 -> 21
    };

    /**@brief struktura do wymiany danych - opisu daty i czasu */
    struct datetime_s{
        uint16_t sec;
        uint16_t min;
        uint16_t hour;
        uint16_t dayofweek;
        uint16_t day;
        uint16_t month;
        uint16_t year; //rok = dwie ost. cyfry z roku 2021 -> 21
    };

    /**2brief konstruktor - inicjalizacja pol klas, rowniez klasy submaszyny*/
    Rtc();

    /**@brief Inicjalizacja interfejsu i2c, ustawienie adresu i2c dla RTC*/
    status_code_t init();

    /**@brief ogolna funkcja do wyslania polecen w postaci zdarzenia
     * @param[in] event wysylane zdarzenie - odpowiada roznym poleceniom
     * @param[in] xdata argument zdarzenia - obecnie tylko dla event_setup_time
     * @return status_ok gdy wlasciwie przekazano, uwaga! to nie znaczy jeszcze prawidlowego wykonania - bo wykonanie jest pozniej*/
    status_code_t process_event( event_t event, void * xdata = NULL);

    /**@brief pobranie czasu w kodzie BCD
     * @return ostatnio odczytany czas; nalezy sprawdzic czy nie zwroci niedozwolonego stanu (np. miesiac=0) - oznacza to ze odczyt jeszcze nie nastapil*/
    datetime_bcd_s get_last_time_bcd( void );

    /**@brief pobranie czasu
     * @return ostatnio odczytany czas; nalezy sprawdzic czy nie zwroci niedozwolonego stanu (np. miesiac=0) - oznacza to ze odczyt jeszcze nie nastapil*/
    datetime_s get_last_time( void );

    /**@brief funkcja zastepujaca przerwania, nalezy ja wywolywac okresowo
     * wtedy wykonywane sa reakcje na zmiany stanu i2c (np. wyslanie danych)*/
    void process( void );

    /**@brief konwersja czasu z postaci dziesietnej na BCD
     * @param[in] datetime struktura opisujaca czas dziesietnie
     * @return zwracana struktura opisujaca czas w BCD */
    datetime_bcd_s time_convert_to_bcd( datetime_s * datetime );

    /**@brief konwersja czasu z postaci BCD na dziesietna
     * @param[in] datetime struktura opisujaca czas w BCD
     * @return zwracana struktura opisujaca czas dziesietnie */
    datetime_s time_convert_from_bcd( datetime_bcd_s * datetime );

    /**@brief konwersja liczby w zakresie 0-99 na BCD
     * @param[in] dec wartosc dziesietna
     * @return zwraca wartosc w kodzie BCD  */
    uint16_t dec_2_bcd( uint16_t dec );

    /**@brief konwersja liczby z wartosci BCD (0 - 0x99) na liczbe dziesietna
     * @param[in] bcd liczba w BCD
     * @return zwraca liczbe dziesietna     */
    uint16_t bcd_2_dec( uint16_t bcd );

private:

    /**@brief ostatnio odczytany czas w kodowaniu BCD
     * @note aby odczytac nalezy uzyc: get_last_time_bcd() */
    struct datetime_bcd_s last_time_bcd;

    /**@brief ostatnio odczytany czas
     * @note aby odczytac nalezy uzyc: get_last_time() */
    struct datetime_s last_time;

    /**@brief interfejs I2C do komunikacji z ukladem RTC
     * @note ukrywa specyfike dla tego procesora; dostarcza transakcje read oraz write na i2c  */
    i2c_t i2c;

    /**@brief stany glownej maszyny stanowej klasy Rtc*/
    typedef enum{
        state_reset = 0,    //0 stan - zanim zostanie wywolane init

        state_idle,         //1 przyjmowanie polecen

        //w trakcie dalszych stanow nie przyjmuje polecen, sa ciagiem sekwencji realizujacych polecenia


//stany submaszyny (odczyt czasu)
        state_read_time1,   //base=2 wyslanie adresu rejestru
        state_read_time2,   //3 oczekiwnie na zakonczenie wyslania, rozpoczecie odczytu
        state_read_time3,   //4 oczekiwnaie na zakonczenie odczytu
        state_read_time4,   //5 zakonczono
//stany submaszyny (odczyt rejestru0 na potrzeby sprawdzenia czy RTC wlaczony)
        state_read_reg0_1,  //6 wyslanie adresu
        state_read_reg0_2,  //7 oczek. na zak. wysl., rozpoczecie odbioru
        state_read_reg0_3,  //8 oczek. na zak. odczytu
        state_read_reg0_4,  //9 zakonczono

        state_setup_default, //:10  wlaczenie zegara w RTC i ustawienie domyslnego czasu

//sekwencja ustawienia nowego czasu
        state_new_time,  //;11 wylaczenie zegara w RTC
 //submaszyna odczytu reg3
        state_new_time1, //<12 wyslanie adresu reg3 (tam jest OSCRUN)
        state_new_time2, //=13 oczekiwanie na zak. wysylania adresu do odczytu reg3
        state_new_time3, //>14 oczekiwanie na zakonczenie odczytu reg3
        state_new_time4, //?15 zakonczono

        state_new_time5, //@16 wyslanie nowego czasu
        state_new_time6, //A17 oczekiwanie na zakonczenie, wyslanie wiadomosci wlaczajcej zegar w RTC
        state_new_time7, //B18 oczekiwnaie na zakonczenie

        state_invalid = 0x20 //P32
    }state_t;

    /**@brief stan Rtc*/
    state_t state;

    /**@brief funkcja zmiany stanu maszyny stanowej, wspiera logowanie zachowania
     * @param[in] new_state nowy stan na ktory nalezy sie przelaczyc */
    void change_state_to( state_t new_state );

    /**@brief funkcja pomocnicza wywolywana przez maszyne stanowa gdy odczytano nowy czas,
    * @note kopiuje go do last_time oraz last_time_bcd*/
    void time_is_read( msg_buffer * msg );

    /**@brief przygotowuje nowa wiadomosc do ustalenia czasu
     * @param[in] new_time nowy czas w kodowaniu BCD */
    status_code_t prepare_setup_msg( datetime_bcd_s * new_time );

    /**@brief przygotowuje wiadomosci do uruchomienia zegara RTC po zapisaniu nowego czasu
     * @param[in] new_time nowy czas w kodowaniu BCD */
    status_code_t prepare_start_msg( void );
};



#endif /* SOFTWARE_DRIVER_RTC_RTC_H_ */
