/*
 * nonvolatile.h
 *
 *  Created on: 13 kwi 2023
 *      Author: Piotr
 */

#ifndef SOFTWARE_NONVOLATILE_H_
#define SOFTWARE_NONVOLATILE_H_

#include "F28x_Project.h"
#include "Software/driver_eeprom/eeprom_i2c.h"

struct region_t{
        union{
            /** lokalizacja w pamieci */
            Uint16 *ptr_u16;
            Uint16  u16;
        }address;

        /** rozmiar */
        Uint16 size;
};

//region rozpoczyna sie od slowa CRC
// starszy bajt jest znacznikiem uniewaznienia <>0 - oznacza ze region jest uniewazniony
// w mlodszym jest CRC
// @TODO dzieki temu nie trzeba byloby czytac calego regionu aby dowiedziec sie ze jest uniewazniony
//       rowneiz uniewaznienie polega na zapisaniu tylko pola crc

/** pozycja tresci regionu w obrazie regionu w eepromie
 * pierwsze dwa bajty sa przeznaczone na pole CRC */
#define REGION_DATA_OFFSET 2  //w bajtach

#define REGION_CRC_SIZE 2  //w bajtach

/** adres czesci informacyjnej w eepromie*/
#define NONVOLATILE_INFO_ADDRESS 0

/** rozmair czesci informacyjnej */
#define NONVOLATILE_INFO_SIZE 8

/** pozycja drugiej kopii regionow w pamieci eeprom (specyficzne dla pamieci 24lc01 (128 bajtow) */
//#define NONVOL_COPY_OFFSET 64 //w bajtach

/** pozycja drugiej kopii regionow w pamieci eeprom (specyficzne dla pamieci 24lc512 witualne stony 8 bajtowe (co 128 bajtow),
 * calkowita pojemnosc pamieci 65536 bajtow; wykorzystana przy wirtualnej stronie 8 bajtow wynosi 4096 */
#define NONVOL_COPY_OFFSET 0x0800 //=2048 //w bajtach

/** skalowanie adresu poczatkowego wynikajace z zastosowanego obejscia problemu duzej strony eepromu 24lc512
 * w stosunku do malego bufora FIFO w module i2c procesora (patrz opis w eeprom_i2c)
 */
#if EEPROM_VIRTUAL_PAGES
# define EEPROM_VIRT_SCALING 16
#else
# define EEPROM_VIRT_SCALING 1
#endif




struct region_memories_t{
    /** lokalizacja (w pamieci RAM) uzytkowej tresci do ktora ma byc nieulotna
     * rozmiar musi byc wielokronoscia dwoch bajtow - dane kopiowane slowami do obszaru data_int*/
    region_t data_ext;

    /** lokalizacja (w pamieci RAM) bufora pomocniczego, na ktorym sa wykonywane operacje z eepromem
     * moze zawierac tresc nieaktualna (np. podczas sprawdzania poprawnosci kopii)
     * dopiero kopiowanie do obszaru data_ext zapewnia ze kopiowane sa prawidlowe dane
     * Rozmiar musi zawierac pole crc (czyli przynajmniej rozmiar_ext+2,
     * ten rozmiar jest uzywany do kopiowania tresci z/do eepromem*/
    region_t data_int;

    /** lokalizacja w pamieci eeprom - adres
     * pole rozmiar nie jest chyba obecnie uzywane; zalecane wpisywanie rozmiaru calego obszaru
     * przechowujacego region*/
    region_t data_eeprom;
};

/** sekcja informacyjna zapisana na poczatku eepromu - jedna kopia*/
struct region_info_t{
    uint16_t crc; /**<@brief crc tej czesci informacyjnej*/
    struct data_s{
        uint16_t eeprom_version;
        uint16_t pcb_version;
        uint16_t eeprom_info_address;/**<@brief adres drugiej czesci informacyjnej*/
    }data;
};

/**@brief druga czesc informacyjna*/
struct region_info_ext_t{
    uint16_t res[4];
};

#define NONVOLATILE_NO_VALID_COPY 100
#define NONVOLATILE_TIMEOUT 200
#define NONVOLATILE_INVALID 10
#define NONVOLATILE_UNKNOWN 11
#define NONVOLATILE_OK 0

#define NONVOLATILE_COPY_0 0
#define NONVOLATILE_COPY_1 1

/*
* pojedyncza tranzakcja:
* zapis strony   - 7.5ms
* odczyt strony - 870us
*
* UWAGA
*  podczas odczytu moze odczytac dwie kopie (bo dopiero druga jest prawidlowa)
*  podczas zapisu musi najpierw odczytac kopie, zapisac tresc oraz uniewaznic kopie
* */

//typ wskaznika do funkcji wywolywany gdy juz gotowe do zapisu
//okreslilo miejsce zapisu
typedef Uint16 (*callback_copy_t)(Uint16 * shadow_buffer_internal, Uint16 shadow_buffer_size);


class nonvolatile_t{

public:
    const Uint16 regions_count;/**<@brief liczba regionow*/

    const region_memories_t * regions;/**<@brief opis regionow*/

    /** zapisuje do eepromu tresc
     * funkcja blokujaca
     * @param[in] region_index index regionu z tabeli regions
     * @param[in] timeout przeterminowanie w ms, =0 oznacza ze w przypadku bledu (zajetosci) wrocic od razu
     * @param[in] funkcja wywolywana gdy mozna juz skpiowac dane do bufora wewn.
     *            jako argument dostaje miejsce do skopiowania
     * @return zwraca 0 gdy sie udalo*/
    Uint16 save( Uint16 region_index, Uint64 timeout, callback_copy_t callback_copy = NULL) const;

    /** odczytuje z eepromu tresc
     * funkcja blokujaca
     * @param[in] region_index index regionu z tabeli regions
     * @param[in] timeout przeterminowanie w ms
     * Jesli adres ext jest =NULL to pomija kopiowanie do zewn. zmiennej
     * @return zwraca 0 gdy sie udalo odczytac */
    Uint16 retrieve(Uint16 region_index, Uint64 timeout) const;

    /** zapis czesci informacyjnych, tylko 1 czesc odczytywana
     * funkcja blokujaca
     * @return 0 gdy poprawny odczyt (zgodne crc)*/
    Uint16 write_info( const struct region_info_t* info, const struct region_info_ext_t * info_ext, Uint64 timeout = 0) const;

    /** odczyt czesci informacyjnych; tylko 1 czesc zapisywana
     * funkcja blokujaca
     * @return 0 gdy poprawny odczyt (zgodne crc)*/
    Uint16 read_info( struct region_info_t* info, struct region_info_ext_t * info_ext, Uint64 timeout = 0) const;

    uint16_t crc8_continue(const Uint16 * data, size_t size, Uint16 crc_init) const;
    uint16_t crc8(const Uint16 * data, size_t size) const;

private:
    // UWAGA wszystkie funkcje prywatne pobieraja timeout jako konczowa wartosc licznika RPC
    /**odczytuje region z pamieci eeprom do pamieci lokalnej (wewn.)*/
    Uint16 read( Uint16 region_index, Uint16 copy_offset, Uint64 timeout )const;

    /**odczytuje region z pamieci eeprom do pamieci lokalnej (wewn.)*/
    Uint16 write( Uint16 region_index, Uint16 copy_offset, Uint64 timeout )const;

    /**uniewaznaia kopie w pamieci eeprom*/
    Uint16 invalidate( Uint16 region_index, Uint16 copy_offset, Uint64 timeout )const;


    /**odczytuje region i sprawdza czy kopia jest poprawna
     * zwraca NONVOLATILE_OK gdy jest poprawna (albo NONVOLATILE_INVALID, NONVOLATILE_TIMEOUT)*/
    Uint16 is_copy_correct( Uint16 region_index, Uint16 copy_offset, Uint64 timeout)const;

    /**odszukuje ostatnia poprawna kopie
     * odczytana tresc w pamieci wewn.
     * zwraca index = NONVOLATILE_COPY_0, NONVOLATILE_COPY_1; NONVOLATILE_NO_VALID_COPY-brak poprawnej kopii,
     *  albo NONVOLATILE_TIMEOUT
     *  w pamieci data_int jest odczytana kopia
     *  @TODO uwaga, gdy bedzie robiona opt aby nie czytac calosci, tu potrzeba bedzie dodac odczyt */
    Uint16 find_last_correct_copy( Uint16 region_index, Uint64 timeout)const;

    /**odszukuje ostatnia poprawna kopie
     * odczytana tresc w pamieci wewn.
     * zwraca index = 0-kopia0, 1-kopia1, 100-brak poprawnej kopii   */
    Uint16 find_incorrect_copy( Uint16 region_index, Uint64 timeout )const;

    /** szybkie stwierdzenie czy kopia jest uniewazniona, bez czytania calej tresci*/
    Uint16 fast_is_copy_incorrect( Uint16 region_index, Uint16 copy_offset, Uint64 timeout )const;

public: //dla nv_read_CT_characteristic()
    /** ogolna funkcja oczekujaca na zakonczenie transackji*/
    Uint16 blocking_wait_for_finished( eeprom_i2c::event_t event, struct eeprom_i2c::event_region_xdata *xdata, Uint64 timeout) const;
};


#endif /* SOFTWARE_NONVOLATILE_H_ */
