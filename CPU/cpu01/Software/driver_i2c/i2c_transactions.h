/*
 * i2c_transactions.h
 *
 *  Created on: 31 paü 2022
 *      Author: Piotr
 */

#ifndef SOFTWARE_DRIVER_I2C_I2C_TRANSACTIONS_H_
#define SOFTWARE_DRIVER_I2C_I2C_TRANSACTIONS_H_


#include <driver_i2c/driver_i2c.h>
#include "Software/driver_i2c_cfg.h"
#include "debug/Dbg.h"


/**@brief typ danych obslugi transakcji  */
struct i2c_transaction_buffer{

    /**@brief stan transakcji*/
    typedef enum {
        idle = 0, /**<@brief przygotowana do wykonania*/
        busy,     /**<@brief w trakcie wykonywnaia*/
        done,     /**<@brief zakonczona pomyslnie*/
        error     /**<@brief zakonczona z bledem*/
    }state_t;

    /**@brief typy transakcji*/
    typedef enum {
        write_only = 0,   /**<@brief tylko zapis danych z bufora*/
        write_nostop_read, /**<@brief najpierw zapis z bufora a potem odczyt do niego
                                     pomiedzy zapisem i odczytem nie ma i2c-STOP*/
        polling           /**<@brief sam adres - poling dla eepromu
        nie uzywac! procesor nie wspiera */
    }type_t;

    uint16_t slave_address; /**<@brief adres urzadzenia do ktorego kierowana komunikacja*/

    uint16_t len_out; /**<@brief ile wyslac z bufora msg*/
    uint16_t len_in;  /**<@brief ile odebrac do bufora msg*/
    volatile state_t state; /**<@brief stan transakcji*/
    type_t type;      /**<@brief typ transakcji*/

    struct msg_buffer msg; /**<@brief bufor do wymiany danych, pola len i ready sa uaktualniane przez te klase
             ustawic na zera; wpisac tylko dane do wyslania do bufora */
};

/**@brief obsluga komunikacji i2c w trybie transakcji
 *
 * @note kiedy transakcja moze zakonczyc sie stanem error:
 *
 * I. przy starcie transakcji
 * transakcja write_only
 * - pole len_in != 0 lub len_out=0
 * transakcja write-nostop-read
 * - pole len_out albo len_in = 0
 * nieznany typ transakcji
 *
 * II. w metodzie ::process()
 * - nieznany typ transakcji
 * transakcja write_only
 * - pole len_in != 0 lub len_out=0
 * - nie udalo sie i2c_write
 * - niewlasciwy stan transakcji
 * transakcja write-nostop-read
 * - pole len_out albo len_in = 0
 * - nie udalo sie i2c.writenostop
 * - nie udalo sie i2c.read
 * - niewlasciwy stan transakcji
 */
class i2c_transactions_t{

    /**@brief etapy wykonywania transakcji, stan tego obiektu*/
    typedef enum {
        idle = 0, /**<@brief nic nie wykonywane na i2c*/
        write,    /**<@brief trwa zapis*/
        write_no_stop, /**<@brief trwa zapis bez oznaczania konca */
        read,     /**<@brief trwa odczyt */
        //terminated
    }state_t;

    /**@brief stan w sensie etapu sekwencji wymaganej przez transakcje
     * To czy mozna rozpoczac czy jest w trakcie transakcja jest rozpoznawane
     * na podstawie (current_transaction == NULL)*/
    state_t state;

    i2c_t i2c; /**<@brief klasa z interfejsem fizycznym i2c*/

    /**@brief aktualnie obslugiwana transakcja, gdy brak, lub sie zakonczy =NULL*/
    struct i2c_transaction_buffer *current_transaction;

public:

    i2c_transactions_t();

    /**@brief konfiguracja interfejsu i2c*/
    status_code_t init(volatile struct I2C_REGS * i2cregs, const struct i2c_t::i2c_clock_config_s * psc_cfg);



    /**@brief rozpoczyna transakcje
     * @param[in] trans bufor do opisu transakcji i wymiany danych
     * @return gdy trwa jakas inna zwraca err_busy, gdy rozpoczal status_ok */
    status_code_t start_transaction( struct i2c_transaction_buffer *trans);

    /**@brief wymuszanie zakonczenia transakcji/reset
     * TODO
     * */

    /**@brief obsluga procesu transakcji - wywolywac cyklicznie
     * ze wzgledow praktycznych w srodku sa tez na stale podczepione urzadzenia
     * na razie tylko jedna instancja */
    status_code_t process(void);

#if USE_SD_INT_FOR_I2C_DRIVER_EEPROM_BUS
    /**@brief wywolanie przerwania od i2c - boczne wejscie gdy wywolanie jest z SD_ISR*/
    status_code_t process_i2c_interrupt(void);
#endif

private:

    /**@brief obsluga samego procesu transakcji */
    status_code_t process_internal(void);

    // -------------------- lock i2c --------------
    /**@brief zwalnia dostep do i2c*/
    void _unlock(void){
        current_transaction = NULL;
    }

    /**@brief blokuje dostep do i2c (=mozliwosc rozpoczynania innych transakcji
     * i podczepia transakcje wykonywana)
     * @param[in] trans transakcja do rozpoczecia */
    status_code_t _lock( struct i2c_transaction_buffer *trans ){
        if( is_busy_transaction() )
             return err_busy;
        current_transaction = trans;
        return status_ok;
    }

    /**@brief sprawdza czy i2c jest zajete */
    uint16_t is_busy_transaction(void){
        return (current_transaction != NULL);
    }

    // --------------------
    /**@brief zwalnia dostep do i2c */
    void transaction_done(void){
        _unlock();
        change_state_to( idle );
    }




    /**@brief zakonczenie transakcji z bledem*/
    void transaction_error( void );

    /**@brief prawidlowe zakonczenie transakcji*/
    void transaction_fini( void );

    /**@brief zmiana etapu/stanu tego obiektu*/
    void change_state_to( state_t new_state ){
        this->state = new_state;
        dbg_marker('=');
        dbg_marker('0'+state);
    }

    /**@brief maszyna stanowa obslugi transakcji typu write_only*/
    status_code_t process_write_only( void );

    /**@brief maszyna stanowa obslugi transakcji typu write_nostop_read*/
    status_code_t process_write_nostop_read( void );

    /**@brief maszyna stanowa obslugi transakcji typu polling*/
    status_code_t process_polling( void );


    status_code_t i2c_write( i2c_transaction_buffer * buffer, uint16_t timeout ){
        buffer->msg.len = buffer->len_out;
        buffer->msg.ready = 0;
        return this->i2c.write(&buffer->msg, timeout);
    }

    status_code_t i2c_write_nostop( i2c_transaction_buffer * buffer, uint16_t timeout ){
        buffer->msg.len = buffer->len_out;
        buffer->msg.ready = 0;
        return this->i2c.write_nostop(&buffer->msg, timeout);
    }

    status_code_t i2c_read( i2c_transaction_buffer * buffer, uint16_t timeout ){
          buffer->msg.len = buffer->len_in;
          buffer->msg.ready = 0;
          return this->i2c.read(&buffer->msg, timeout);
    }



};

#endif /* SOFTWARE_DRIVER_I2C_I2C_TRANSACTIONS_H_ */
