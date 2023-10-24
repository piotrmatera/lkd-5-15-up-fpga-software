/*
 * eeprom_i2c.h
 *
 *  Created on: 8 lis 2022
 *      Author: Piotr
 *
 *  obsluguje eeprom 24lc01 128 bajtow
 */

#ifndef SOFTWARE_DRIVER_EEPROM_EEPROM_I2C_H_
#define SOFTWARE_DRIVER_EEPROM_EEPROM_I2C_H_

#include "i2c_transactions.h"

#define EEPROM_ADDRESS (0xa0 >> 1)

//poprzednia implementacja dla 24LC01
// 128 bajtow, 1kbit
// 5ms - zapis strony
// 8 bajtow - wielkosc strona

//obecna implemntacja dla eepromu 24LC512
// 64KB, 512kbit
// 5ms - zapis strony
// 128 bajtow - wielkosc strony

#define EEPROM_VIRTUAL_PAGES 0

#if EEPROM_VIRTUAL_PAGES
# define EEPROM_PAGE_GAP (EEPROM_REAL_PAGE-EEPROM_PAGE) //dodanie EEPROM_REAL_PAGE-EEPROM_PAGE jest bezpieczne, bo to jest tez wyrownane do 8
#else
# define EEPROM_PAGE_GAP 0
#endif

#if EEPROM_VIRTUAL_PAGES
# define EEPROM_SIZE 4096 //rozmiar w bajtach, ktory mozna zapisac
# define EEPROM_PAGE 8   //rozmiar strony w bajtach
# define EEPROM_REAL_PAGE 128 //rzeczywisty rozmiar strony eepromu
// Ominiecia problemu, ze wielkosc bufora sprzetowego modulu i2c wynosi tylko 14 bajtow
// co jest znacznie mniejsze niz 128 bajtow strony eepromu.
// TODO rozwiazanie powyzszego wymaga modyfikacji drivera_i2c aby w 'przerwaniu' oproznial/dopisywal do FIFO modulu i2c
//
// Na razie zapis tylko 8 bajtow na strone; reszta niewykorzystana - jako przerwa.
// adresowanie po stronie SW jakby nie bylo tych przerw.
// adresowanie po stronie HW (eepromu) dla zapisu od adresu 8 ciagu 11 bajtow:
// adrSW adrHW
// 0008  0008: zapis 8 pierwszych bajtow
// 0010  0088: zapis kolejnych 3 bajtow
//
// adresy po stronie SW: 8*512 stron -> 4KB
// adresy po stronie HW 128*512 stron -> 64KB

//#define EEPROM_SIZE 65536 //rozmiar w bajtach
//#define EEPROM_PAGE_MASK 0xFF80
//#define EEPROM_ADDRESS_MASK 0xFFFF
#else
# define EEPROM_SIZE 65536 //rozmiar w bajtach, ktory mozna zapisac
# define EEPROM_PAGE 8   //rozmiar strony w bajtach - do tego wyrownywane i w takich paczkach wysylane do eepromu

#endif
//adresy wewn. eeprom_i2c:
#define EEPROM_PAGE_MASK 0xFF80 //to jest sprawdzane w eeprom_i2c do sprawdzenia czy zapis w obrebie strony
#define EEPROM_ADDRESS_MASK 0xFFFF //to jest uzywane po tlumaczeniu adresu wirt. na rzeczywisty w eepromie

/**@brief czas zapisu do eepromu, nie mozna wykonac polingu na tym procesorze tms*/
#define EEPROM_WRITE_TIME_MS 6ULL  //w ms z dok. ukladu
#define EEPROM_WRITE_MARGIN_US   500ULL  //czas dodatkowy wynikajacy z dzialania programu
#define EEPROM_WRITE_TIME_US ((EEPROM_WRITE_TIME_MS)*1000ULL +(EEPROM_WRITE_MARGIN_US)) //w us
#define EEPROM_WRITE_TIME ((EEPROM_WRITE_TIME_US)*200ULL)  //w jednostkacj IpcTimer

/** operacje na eepromie*/
class eeprom_i2c{
public:

    struct i2c_transaction_buffer x_msg;

    /**@brief typ opisujacy rodzaje zdarzen*/
    typedef enum{

        event_periodic = 0x10, /**<@brief zdarzenie wysylane okresowo, nie wysylac samodzielnie, do uzytku wewnetrznego*/

        //zdarzenia odpowiadajace poleceniom:
        event_init = 0x20, /**<@brief wykonanie inicjalizacji interfejsow, wlaczenie RTC jesli wylaczone (+ustawienie domyslnego czasu)*/

        //region musi byc wyrownany do 8 bajtow
        event_read_region,
        event_write_region,
        event_abort  //przerwanie wykonywanej operacji
                     //polega na zapisaniu do region = NULL
                     //wywolywac jesli tylko gdy udalo sie rozpoczac operacje
    }event_t;

    /** dane zwiazane z zapisem/odczytem regionu*/
    struct event_region_xdata{
        uint16_t start;    /**<@brief adres w eepromie*/
        uint16_t total_len;/**<@brief calkowita liczba bajtow do odczytania
                               To jest zmieniane w trakcie odczytow/zapisow*/
        uint16_t len;      /**<@brief ilosc bajtow z/do bufora data w jednej transakcji i2c
                               To jest zmieniane w trakcie odczytow/zapisow*/
        uint16_t *data;    /**<@brief bufor zewnetrzny z danymi. Do pamieci eeprom sa
               zapisywane kolejne bajty ze slow w data. Najpierw mlodszy bajt slowa. */

        /** stan realizacji transferu
         * do sprawdzania stanu - kiedy sie zakonczy
         * @note zainicjowac przed wywolaniem na idle  */
        enum op_status{
            idle,
            started,
            done_ok,
            done_error
        } status;

    };

    /**opis regionu aktualnie przetwarzanego, poprzez ustaienie tego na NULL bedzie sygn. abort*/
    struct event_region_xdata *region;

    eeprom_i2c();

    /**@brief Inicjalizacja interfejsu i2c, ustawienie adresu i2c dla RTC*/
    status_code_t init( i2c_transactions_t * i2c_bus );

    /**@brief ogolna funkcja do wyslania polecen w postaci zdarzenia
     * @param[in] event wysylane zdarzenie - odpowiada roznym poleceniom
     * @param[in] xdata argument zdarzenia - opis danych struktura event_region_data
     * @return status_ok gdy wlasciwie przekazano, uwaga! to nie znaczy jeszcze prawidlowego wykonania - bo wykonanie jest pozniej*/
    status_code_t process_event( event_t event, void * xdata = NULL);


    /**@brief funkcja zastepujaca przerwania, nalezy ja wywolywac okresowo
     * wtedy wykonywane sa reakcje na zmiany stanu i2c (np. wyslanie danych)*/
    void process( void );

//private:

    /**@brief interfejs I2C do komunikacji ukladem RTC po magistrali i2c */
       i2c_transactions_t * i2c_bus;

    /**@brief stany glownej maszyny stanowej klasy Rtc*/
      typedef enum{
          state_reset = 0,    //0 stan - zanim zostanie wywolane init
          state_idle,         //1 przyjmowanie polecen

          //w trakcie dalszych stanow nie przyjmuje polecen, sa ciagiem sekwencji realizujacych polecenia

          state_read_region,   //2 oczekwianie na odczyt czasu
          state_write_region,  //3

          state_wait_for_write,  //4
          state_wait_for_write_delay,
          //state_wait_for_write1, //5
          //state_wait_for_write2, //6

          state_read_start,
          state_write_start,

          state_error,
          state_invalid = 0x20 //P32
      }state_t;

      state_t state;

      /**@brief ustawia adres dla nastepnych wywolan*/
      status_code_t set_i2c_address( Uint16 i2c_addr );
private:
      Uint16 i2c_addr;

      /**wywolywane wewn. przy zmianie stanu tego obiektu*/
      void change_state_to( state_t new_state );

      /** typy wiadomosci*/
      typedef enum{
                  read_part,
                  write_part,
                  polling, //obecnie nieobslugiwane

      } msg_type_t;


      /**@brief przygotowanie wiadomosci do przeslania po i2c
       * param[in] msg_type typ wiadomosci
       * param[in] buffer wiadomosc (do wykonania jako transakcja)
       * param[in] xdata dane zwiazane z wiadomoscia (nie sa kopiowane, musza byc trwale przez caly czas realizacji transakcji)       */
      status_code_t msg_factory( msg_type_t msg_type, i2c_transaction_buffer * buffer, void * xdata );

      Uint64 write_ready_time; //czas po ktorym na pewno sie zapisalo (zamiast pollingu)

private:
      /**@brief wyznacza rozmiar paczki do odczytu/zapisu
       * @param[in] start_address adres gdzie zapisac w epromie
       * @param[inout] len_left ilosc bajtow do przetworzenia
       * @return wielkosc nastepnej czesci
       * @note len_left jest zmniejszany o rozmiar nast. czesci */
      uint16_t aligned_part( uint16_t start_address, uint16_t * len_left );
};


#endif /* SOFTWARE_DRIVER_EEPROM_EEPROM_I2C_H_ */
