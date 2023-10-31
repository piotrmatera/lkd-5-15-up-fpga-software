/*
 * nv_section_types.h
 *
 *  Created on: 27 paŸ 2023
 *      Author: Piotr
 */

#ifndef SOFTWARE_NV_SECTION_TYPES_H_
#define SOFTWARE_NV_SECTION_TYPES_H_

/** typ do wyboru ktore dane beda odczytane, uzywnae przez nv_data_t oraz SD_card_class, do uogolnienia*/
typedef enum{
        sec_CT_characteristic,
        sec_settings,
        sec_H_settings,
        sec_calibration_data,
        sec_meter_data,

        sec_type_max
    } section_type_t;

/** przelaczalnie w trakcie kompilacji, jaki obiekt bedzie uzywany do zapisu ustawien konfiguracyjnych
 *  a zarazem gdzie beda zapisywane te ustawienia
 *
 *  =1 nv_data_t     eeprom
 *  =0 SD_card_class karta_SD
 */
#define NV_IN_EEPROM 1


#if NV_IN_EEPROM

/* klasa zastepuje operacje na plikacjh konfiguracyjnych wykonywane przez SD_card_class
 * ze wzgledu na wpasowanie do istniejacego kodu dane zapisuje do/z struktur w SD_card_t
 *
 * Zamiast na karte SD dane beda zapisywane do EEPROMU
 */
class nv_data_t{
public:
        /** odczyt danych konfiguracyjnych z eepromu do struktur w SD_card
         * @param[in] wybor, ktore dane odczytac
         * @return 0 gdy sie udalo
         */
        Uint16 read( section_type_t section );

        /**zapis danych konfiguracyjnych z SD_card do eepromu
         * @param[in] wybor, ktore dane odczytac
         * @return 0 gdy sie udalo
         */
        Uint16 save( section_type_t section );

//private: publiczne dla callbackow
        //wewnetrzne typy danych uzywane przez callbacki
        typedef enum {
            //UWAGA! nie zmieniac kolejnosci ani nie dopisywac do srodka - wart. enum sa zapisane w eepromie
            SETTINGS_STATIC_Q_COMPENSATION_A,
            SETTINGS_STATIC_Q_COMPENSATION_B,
            SETTINGS_STATIC_Q_COMPENSATION_C,
            SETTINGS_ENABLE_Q_COMPENSATION_A,
            SETTINGS_ENABLE_Q_COMPENSATION_B,
            SETTINGS_ENABLE_Q_COMPENSATION_C,
            SETTINGS_ENABLE_P_SYMMETRIZATION,
            SETTINGS_ENABLE_H_COMPENSATION,
            SETTINGS_VERSION_P_SYMMETRIZATION,
            SETTINGS_VERSION_Q_COMPENSATION_A,
            SETTINGS_VERSION_Q_COMPENSATION_B,
            SETTINGS_VERSION_Q_COMPENSATION_C,
            SETTINGS_TANGENS_RANGE_A_HIGH,
            SETTINGS_TANGENS_RANGE_B_HIGH,
            SETTINGS_TANGENS_RANGE_C_HIGH,
            SETTINGS_TANGENS_RANGE_A_LOW,
            SETTINGS_TANGENS_RANGE_B_LOW,
            SETTINGS_TANGENS_RANGE_C_LOW,
            SETTINGS_BAUDRATE,
            SETTINGS_EXT_SERVER_ID,
            SETTINGS_WIFI_ONOFF,
            SETTINGS_C_DC,
            SETTINGS_L,
            SETTINGS_C,
            SETTINGS_I_LIM,
            SETTINGS_NUMBER_OF_SLAVES,
            SETTINGS_NO_NEUTRAL,
            SETTINGS_MAX
        } settings_t;

        /** format zapisanych danych w sekcji settings*/
        class settings_item{
        public:
                Uint16 type; //typ wg settings_t
        private:
                Uint16 _float[2]; //recznie skladane bo kompilator dodawal przez u16 dla wyrownania
        public:
                float get_value(void) const{
                    float v = 0;
                    Uint16 * p_float = (Uint16*)(void*)&v;
                    p_float[0] = this->_float[0];
                    p_float[1] = this->_float[1];
                    return v;
                }
                void set_value(float v){
                    Uint16 * p_float = (Uint16*)(void*)&v;
                    this->_float[0] = p_float[0];
                    this->_float[1] = p_float[1];
                }
        };

        //** format zapisanych danych do sekcji harmonicznych*/
        struct harmon_item{
            Uint16 a:4;
            Uint16 b:4;
            Uint16 c:4;
            Uint16 res:4;
        };
private:
   //funkcje zapisuja dane do SD_card
        Uint16 read_settings();
        Uint16 read_H_settings();
        Uint16 read_calibration_data();
        Uint16 read_meter_data();
        Uint16 read_CT_characteristic();

   //funkcje odczytuja dane do obiektu SD_card
   //uwaga: wlasciwe kopiowanie odbywa sie w callbackach
        Uint16 save_settings();
        Uint16 save_H_settings();
        Uint16 save_calibration_data();
        Uint16 save_meter_data();

};

extern nv_data_t nv_data;

# define NV_CLASS_read( section ) nv_data.read( section )
# define NV_CLASS_save( section ) nv_data.save( section )
#else
# define NV_CLASS_read( section ) SD_card.read( section )
# define NV_CLASS_save( section ) SD_card.save( section )
#endif

#endif /* SOFTWARE_NV_SECTION_TYPES_H_ */
