/*
 * MosfetCtrlApp.h
 *
 *  Created on: 25 lut 2022
 *      Author: Piotr Romaniuk
 *
 *
 *
 *
 *  Uzycie:
 *
 *  utworzyc instancje maszyny stanowej:
 *
 *      MosfetCtrlApp mc_app;
 *
 *  Zainicjowac:
 *
 *      mc_app.init();
 *
 *
 *  Wywolywanie okresowe:
 *
 *     mc_app.process();
 *
 *
 *  Rozpoczac konfiguracje w jakims stanie:
 *
 *      statuc_code_t retc = mc_app.process_event( MosfetCtrlApp::event_configure );
 *      if( retc != status_ok ){
 *          //BLAD!!!
 *      }
 *
 *      okresowo sprawdzac czy juz skonczyla sie konfiguracja
 *
 *      if( mc_app.get_state == MosfetCtrlApp::state_idle ){
 *          prawidlowe wykonanie
 *      }
 *
 *      if( mc_app.get_state == MosfetCtrlApp::state_error ){
 *          wystapil blad
 *      }
 *
 *
 *
 */

#ifndef SOFTWARE_MOSFETCTRLAPP_H_
#define SOFTWARE_MOSFETCTRLAPP_H_


#include "driver_mosfet/MosfetDriver.h"

#define MAX_MOS_DRIVERS 8
#define MAX_CFG_REGISTERS CHIP1ED389_CONFIG_REGS_COUNT

//#define SM_YIELD() state = (state_t)__LINE__; \
//    return retc;\
//    case __LINE__:


#define SM_YIELD() this->change_state_to((state_t)__LINE__); \
    return retc;\
    case __LINE__:

class MosfetCtrlApp{
public:
    typedef enum{
      state_reset = 0,
      state_idle,        // 1 przyjmowanie polecen
      state_configuring, // 2 konfigruacja ukladow
      state_getting_status,//3 odczyt stanu
      state_error        // 4 blad
    } state_t;

    typedef enum{
        logpoint_config = 0,
        logpoint_getstatus,
        logpoint_precfg,
        logpoint_softreset,

        logpoint_max
    } logpoint_t;
private:
    state_t state;

    uint16_t loop_cnt;

    uint16_t error_state[logpoint_max]; //osobne bledy dla roznych operacji

    struct MosfetDriver::xdata_event_regs data_dsc;

    uint16_t buffer[MAX_CFG_REGISTERS];

    uint16_t buffer_in[MAX_MOS_DRIVERS][ CHIP1ED389_STATUS_REGS_COUNT ];

    uint16_t values[2];
public:

    typedef enum{
        event_periodic = 0x10, /**<@brief zdarzenie wysylane okresowo, nie wysylac samodzielnie, do uzytku wewnetrznego*/
        event_configure,
        event_get_status,
        event_restart
    } event_t;

    MosfetDriver::sm_status_t sm_status_info[2];

    MosfetCtrlApp();

    /**@brief Inicjalizacja*/
    status_code_t init();


   /**@brief ogolna funkcja do wyslania polecen w postaci zdarzenia
    * @param[in] event wysylane zdarzenie - odpowiada roznym poleceniom
    * @param[in] xdata argument zdarzenia - obecnie tylko dla event_setup_time
    * @return status_ok gdy wlasciwie przekazano, uwaga! to nie znaczy jeszcze prawidlowego wykonania - bo wykonanie jest pozniej*/
    status_code_t process_event( event_t event, void * xdata = NULL);

   /**@brief funkcja zastepujaca przerwania, nalezy ja wywolywac okresowo
    * wtedy wykonywane sa reakcje na zmiany stanu i2c (np. wyslanie danych)*/
    void process( void );

    /**odczyt stanu, aby wiedziec czy zakonczyla sie sekwencja*/
    state_t getState();

    /**odczyt bledow dla poszczegolnych ukladow*/
    uint16_t getErrorsGetStatus();

    /**odczyt bledow dla poszczegolnych ukladow*/
    uint16_t getErrorsConfig();

    uint16_t getErrorsAtPoint(logpoint_t point);

    /**odczyt bledow dla poszczegolnych ukladow*/
    uint16_t getErrorsConfigPre();

    /** przedefiniowuje konfiguracje GPIO oraz ustawia w tryb aby mozna bylo komunikowac sie z ukladami*/
    status_code_t configure_gpio(void);

    /** przywraca konfiguracje koncowek wymagana przez LKD*/
    status_code_t restore_gpio_configuration(void);

    bool finished( void );

    uint16_t buffer_item( uint16_t device, uint16_t reg );



private:
    /**@brief funkcja zmiany stanu maszyny stanowej, wspiera logowanie zachowania
       * @param[in] new_state nowy stan na ktory nalezy sie przelaczyc */
    void change_state_to( state_t new_state );

    /**@brief wydzielenie sprawdzania warunkow w petli*/
    status_code_t MosfetDriver_Status(uint16_t loop_cnt, uint16_t * p_error_state);

    //funkcje upraszczajace wywolanie, argumenty na liscie argumentow a nie w strukturze
    status_code_t MosfetDriver_WriteRegs( uint16_t address, uint16_t count, uint16_t * data );
    status_code_t MosfetDriver_ReadRegs( uint16_t address, uint16_t count, uint16_t * data );


};

extern MosfetCtrlApp mosfet_ctrl_app;

#endif /* SOFTWARE_MOSFETCTRLAPP_H_ */
