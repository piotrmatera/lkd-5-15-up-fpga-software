/*
 * driver_i2c.h
 *
 *  Created on: 1 paü 2020
 *      Author: Piotr
 *
 *  Na podstawie przykladu z C2000Ware od ST, pliku i2c_ex2_eeprom.c
 */

#ifndef SOFTWARE_DRIVER_I2C_DRIVER_I2C_H_
#define SOFTWARE_DRIVER_I2C_DRIVER_I2C_H_

#include "stdafx.h"
#include "stdint.h"
#include "status_codes.h"


#define I2C_USE_INTERRUPTS 0



#define MAX_BUFFER_SIZE             14      // Max 16 bajtowe fifo w module i2c

#define I2C_400kHz_DUTY1_3 {16, 13, 3}
#define I2C_200kHz_DUTY1_3 {19, 25, 10}
#define I2C_100kHz_DUTY1_3 {19, 60, 26}
#define I2C_220kHz_DUTY1_2 {16, 20, 20}




//typ danych do przekazywania wiadomosci
struct msg_buffer{
    uint16_t len;   //ile danych odebrac/wyslac
    volatile uint16_t ready; //do sygnalizacji zakonczenia tx/rx tej wiadomosci
    uint16_t data[MAX_BUFFER_SIZE+2]; //dane do wyslania/ew. dane odebrane
    //musi byc wiecej bo w tym buforze jest zapisywany tez adres rejestru
};


#define MSG_NOTREADY 0  //wiadomosc przygotowywana do przeslania/odebrania
//uwaga w kodzie sa testy oczekujace na .ready!=0
#define MSG_READY 1     //wiadomosc odebrana/wyslana
#define MSG_READY_W_ERROR 2 //wiadomosci nie udalo sie wyslac, najprawdopodobniej z powodu NACK
//gdy przekazuje sie wiadomosc do wyslania/odebrania, pole .ready mozna interpretowac jedynie gdy
// fn rozpoczynajaca zwroci status_ok (moze zwrocic busy=zajete i2c, invalid=niewlasciwe dane)

//klasa do komunikacji po i2c w trybie master
class i2c_t{
    uint16_t slave_address;
    volatile struct I2C_REGS * i2cregs;

public:

    //typ danych do stanu interfejsu i2c - rodzaj wykonywanej transakcji
    typedef enum{
        I2C_STATUS_RESET = 0x00FF, //stan reset, zanim wywola sie init()

        I2C_STATUS_INACTIVE = 0x0000, // interfejs dostepny

        I2C_STATUS_WRITE_BUSY    = 0x0011, // trwa wysylanie
        I2C_STATUS_SEND_NOSTOP_BUSY = 0x0021, // trwa wysylanie bez STOP na koncu
        I2C_STATUS_RESTART          = 0x0022, // wykonano wysylanie bez STOP na koncu, mozna kontynuowac (wyslac READ)
        I2C_STATUS_READ_BUSY        = 0x0023 // trwa odczytywanie
    } i2c_state_t;

    //rodzaje przerwan
    typedef enum
    {
        I2C_INTSRC_NONE,                //!< No interrupt pending
        I2C_INTSRC_ARB_LOST,            //!< Arbitration-lost interrupt
        I2C_INTSRC_NO_ACK,              //!< NACK interrupt
        I2C_INTSRC_REG_ACCESS_RDY,      //!< Register-access-ready interrupt
        I2C_INTSRC_RX_DATA_RDY,         //!< Receive-data-ready interrupt
        I2C_INTSRC_TX_DATA_RDY,         //!< Transmit-data-ready interrupt
        I2C_INTSRC_STOP_CONDITION,      //!< Stop condition detected
        I2C_INTSRC_ADDR_SLAVE,          //!< Addressed as slave interrupt
    } I2C_InterruptSource;

    struct i2c_clock_config_s{
        uint16_t psc;
        uint16_t cl;
        uint16_t ch;
    } i2c_clock_cfg;



    i2c_state_t state; //stan interfejsu

    msg_buffer * _buffer; //bufor do wymiany danych (podczpiany przy rozp. transakcji)

    i2c_t();

    //inicjalizacja interfejsu
    status_code_t init(volatile struct I2C_REGS * i2cregs, const struct i2c_clock_config_s * psc_cfg);

    //ustalenie adresu i2c urzadzenia slave
    status_code_t set_slave_address( uint16_t address );

    //rozpoczecie transakcji zapisu z STOP na koncu
    //@param[in] buffer bufor do wyslania; po wyslaniu .ready=1
    //@param[in] timeout obecnie nieuzywane
    // stan i2c jest zmieniany na I2C_STATUS_INACTIVE, mozna to sprawdzic przez:
    //  is_msg_finished()
    status_code_t write( msg_buffer * buffer, uint16_t timeout );

    //rozpoczecie transakcji zapisu bez STOP na koncu (jako 1 faza odczytu po i2c)
    //@param[in] buffer bufor do wyslania; po wyslaniu .ready=1
    //@param[in] timeout obecnie nieuzywane
    // stan i2c jest zmieniany na I2C_STATUS_RESTART, mozna to sprawdzic przez:
    //  is_msg_finished_nostop()
    status_code_t write_nostop( msg_buffer * buffer, uint16_t timeout );

    //rozpoczecie transakcji odczytu (jako 2 faza odczytu po i2c)
    //@param[in] buffer bufor do odebrania; po odebraniu .ready=1
    //@param[in] timeout obecnie nieuzywane
    // stan i2c jest zmieniany na I2C_STATUS_INACTIVE, mozna to sprawdzic przez:
    //  is_msg_finished()
    status_code_t read( msg_buffer * buffer, uint16_t timeout );

    //sprawdza czy mozna rozpoczynac nowa transakcje na i2c
    //wewnetrznie tozsame z is_msg_finished()
    bool is_idle( void );

    //testowanie czy wiadomosc zostala wyslana/odebrana
    //wewnetrznie tozsame z is_idle()
    //jesli byl odczyt to tym mozna rozpoznac zakonczenie po .ready w strykturze wiadomosci
    bool is_msg_finished( void );

    //testowanie czy wiadomosc zostala wyslana (1.faza odbioru)
    // to jest uzywane do wykrycia kiedy rozpoczac 2. faze odbioru
    bool is_msg_finished_nostop(void);

#if !(I2C_USE_INTERRUPTS)
    status_code_t interrupt_process(void); //gdy bez przerwan wywolywac cyklicznie
#endif

    status_code_t get_error(void){ return error;}

private:
    void mode_init(void);
    void mode_start_write(void);
    void mode_start_write_nostop(void);
    void mode_start_read(void);

    status_code_t error;
#if I2C_USE_INTERRUPTS
public:
#endif
    //musi byc public aby sie dostac do niej z ISR
    //nie uzywac samodzielnie, nie wywolywac
    void interrupt_process( i2c_t::I2C_InterruptSource intSource,uint16_t i2cstreg );


};

#if I2C_USE_INTERRUPTS
extern i2c_t * _i2c;
#endif

#endif /* SOFTWARE_DRIVER_I2C_DRIVER_I2C_H_ */
