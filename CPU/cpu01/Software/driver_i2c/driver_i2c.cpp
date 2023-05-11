/*
 * driver_i2c.cpp
 *
 *  Created on: 1 paü 2020
 *      Author: Piotr
 */

#include <driver_i2c/driver_i2c.h>
#include <debug/Dbg.h>


#if I2C_USE_INTERRUPTS
static __interrupt void i2cAISR(void);
#endif

i2c_t::i2c_t(){
    this->slave_address = 0;
    this->state = I2C_STATUS_RESET;
    this->_buffer = NULL;
}

const struct i2c_clock_config_s{
    uint16_t psc;
    uint16_t cl;
    uint16_t ch;
} i2c_clock_cfg = I2CPSC_CFG_VALUES;

status_code_t i2c_t::init(void){
    //inicjalizacja koncowek musi byc na zewnatrz - jest w klasie Init.GPIO
#if I2C_USE_INTERRUPTS
    EALLOW;
    PieVectTable.I2CA_INT = &i2cAISR; //rejestracja ISR dla przerwania od I2CA
    EDIS;
#endif

    EALLOW;
    CpuSysRegs.PCLKCR9.bit.I2C_A = 1;
    EDIS;

    I2caRegs.I2CMDR.bit.IRS = 0; //ustawienie i2c w reset na czas konfiguracji

    I2caRegs.I2CPSC.all = i2c_clock_cfg.psc;
    I2caRegs.I2CCLKL = i2c_clock_cfg.cl;
    I2caRegs.I2CCLKH = i2c_clock_cfg.ch;

//    I2caRegs.I2CSAR.all = SLAVE_ADDRESS;

    //I2caRegs.I2CMDR.bit.FREE = 1; usunieto, gdy =1 to i2c dziala przy pulapce dbg, =0 od razu wstrzymanie zegara
    mode_init();

#if I2C_USE_INTERRUPTS
    // Enable stop condition and register-access-ready interrupts
    I2caRegs.I2CIER.bit.SCD = 1;
    I2caRegs.I2CIER.bit.ARDY = 1;
#endif

    // FIFO configuration
    I2caRegs.I2CFFTX.bit.I2CFFEN = 1;
    I2caRegs.I2CFFTX.bit.TXFFRST = 1;

    //I2caRegs.I2CFFRX.bit.RXFFIENA = 1;// [PR] tego brakowalo w przykladzie TI
    I2caRegs.I2CFFRX.bit.RXFFRST = 1;


    //skasowanie stanu przerwan od fifo
    I2caRegs.I2CFFTX.bit.TXFFINTCLR = 1;
    I2caRegs.I2CFFRX.bit.RXFFINTCLR = 1;

    I2caRegs.I2CMDR.bit.IRS = 1; //wlaczenie modulu i2c



#if I2C_USE_INTERRUPTS

    PieCtrlRegs.PIEIER8.bit.INTx1 = 1;//Pie INT8.1
    IER | = M_INT8;

#endif

    state = I2C_STATUS_INACTIVE;

    return status_ok;
}

#define I2CMDR_NACKMOD   (1<<15) //uzywany tylko gdy pracuje jako odbiornik; do wysylania NACK - nieuzywane tutaj
#define I2CMDR_FREE_MASK (1<<14) //=0 stop CLK od razu gdy pulapka dbg, =1 normalnie pracuje
#define I2CMDR_STT_MASK  (1<<13) //START
#define I2CMDR_STP_MASK  (1<<11) //STOP
#define I2CMDR_MST_MASK  (1<<10) //=1 master
#define I2CMDR_TRX_MASK  (1<<9)  //=1 nadajnik
#define I2CMDR_XA_MASK   (1<<8)  //=0 normalny adres 7bitow ; expanded address
#define I2CMDR_RM_MASK   (1<<7)  //repeative mode
#define I2CMDR_DLB_MASK  (1<<6)  //digital loopback
#define I2CMDR_IRS_MASK  (1<<5) //=1 aby byl wlaczony modul i2c, =0 aby byl wylaczony
#define I2CMDR_STB_MASK  (1<<4) //=1 start byte mode bit, gdy dodatkowy bajt startu na i2c
#define I2CMDR_FDF_MASK  (1<<3) //=0 free data format wylaczone
#define I2CMDR_BC_8BITS  0      //0-7, =0 == 8bitow


void i2c_t::mode_init(void){
    I2caRegs.I2CMDR.all = I2CMDR_MST_MASK;
}

void i2c_t::mode_start_write(void){
    I2caRegs.I2CMDR.all = I2CMDR_MST_MASK
            | I2CMDR_IRS_MASK
            | I2CMDR_TRX_MASK
            | I2CMDR_STT_MASK
            | I2CMDR_STP_MASK;
}

void i2c_t::mode_start_write_nostop(void){
    I2caRegs.I2CMDR.all = I2CMDR_MST_MASK
                | I2CMDR_IRS_MASK
                | I2CMDR_TRX_MASK
                | I2CMDR_STT_MASK;
}

void i2c_t::mode_start_read(void){
    I2caRegs.I2CMDR.all = I2CMDR_MST_MASK
            | I2CMDR_IRS_MASK
            | I2CMDR_STT_MASK
            | I2CMDR_STP_MASK;

}

status_code_t i2c_t::set_slave_address( uint16_t address ){
    this->slave_address = address;
    I2caRegs.I2CSAR.all = address;
    return status_ok;
}

status_code_t i2c_t::write( msg_buffer * buffer, uint16_t timeout )
{
        uint16_t i;
        if( buffer == NULL )
            return err_invalid;
        if( buffer->len > MAX_BUFFER_SIZE )
            return err_invalid;

        // Wait until the STP bit is cleared from any previous master
        // communication. Clearing of this bit by the module is delayed until after
        // the SCD bit is set. If this bit is not checked prior to initiating a new
        // message, the I2C could get confused.
        //
        if( I2caRegs.I2CMDR.bit.STP == 1 )
        {
            return err_stop_not_ready;
        }

        I2caRegs.I2CSAR.all = this->slave_address;

        // Check if bus busy
        if( I2caRegs.I2CSTR.bit.BB == 1 )
        {
            return err_bus_busy;
        }

        if( this->state != I2C_STATUS_INACTIVE ){
            return err_busy;
        }

        this->_buffer = buffer;
        this->_buffer->ready = 0;
        I2caRegs.I2CCNT = this->_buffer->len;

        for (i = 0; i < this->_buffer->len; i++) //zapisanie danych (wartosci do kolejnych rejestrow RTC)
            I2caRegs.I2CDXR.all = this->_buffer->data[i];

        // master send z STOP na koncu
        mode_start_write();

        this->state = I2C_STATUS_WRITE_BUSY;
        return status_ok;
}

status_code_t i2c_t::write_nostop( msg_buffer * buffer, uint16_t timeout )
{
       uint16_t i;
       if( buffer == NULL )
            return err_invalid;
       if( buffer->len > MAX_BUFFER_SIZE )
            return err_invalid;

       if( I2caRegs.I2CMDR.bit.STP == 1 )
       {
           return err_stop_not_ready;
       }
       if( this->state != I2C_STATUS_INACTIVE ){
           return err_busy;
       }

       if( I2caRegs.I2CSTR.bit.BB == 1 )
       {
           return err_bus_busy;
       }

       I2caRegs.I2CSAR.all = this->slave_address;

       this->_buffer = buffer;
       this->_buffer->ready = 0;
       I2caRegs.I2CCNT = this->_buffer->len;

       for (i = 0; i < this->_buffer->len; i++) //zapisanie danych (wartosci do kolejnych rejestrow RTC)
           I2caRegs.I2CDXR.all = this->_buffer->data[i];

       //master send bez STOP
       mode_start_write_nostop();

       state = I2C_STATUS_SEND_NOSTOP_BUSY;


       return status_ok;
}

status_code_t i2c_t::read( msg_buffer * buffer, uint16_t timeout )
{
        if( buffer == NULL )
            return err_invalid;
        if( buffer->len > MAX_BUFFER_SIZE )
            return err_invalid;

    //
        // Wait until the STP bit is cleared from any previous master
        // communication. Clearing of this bit by the module is delayed until after
        // the SCD bit is set. If this bit is not checked prior to initiating a new
        // message, the I2C could get confused.
        //
        if( I2caRegs.I2CMDR.bit.STP == 1 )
        {
            return err_stop_not_ready;
        }

        this->_buffer = buffer;
        this->_buffer->ready = 0;
        I2caRegs.I2CCNT = this->_buffer->len;

        I2caRegs.I2CSAR.all = this->slave_address;

        //master receive
        mode_start_read();

        state = I2C_STATUS_READ_BUSY;

        return status_ok;
}


bool i2c_t::is_msg_finished( void ){
    return (state == i2c_t::I2C_STATUS_INACTIVE);
}

bool i2c_t::is_idle( void ){
    return (state == i2c_t::I2C_STATUS_INACTIVE);
}

bool i2c_t::is_msg_finished_nostop(void){
    return (state == i2c_t::I2C_STATUS_RESTART);
}

#if I2C_USE_INTERRUPTS
__interrupt void i2cAISR(void)
{
    i2c_t::I2C_InterruptSource intSource = (i2c_t::I2C_InterruptSource)(I2caRegs.I2CISRC.bit.INTCODE & I2C_ISRC_INTCODE_M);

    _i2c->interrupt_process( intSource );
    // Issue ACK to enable future group 8 interrupts
     PieCtrlRegs.PIEACK.bit.ACK8 = 1;
}
#endif

#define I2CSTR_ARDY_MASK (1<<2)
#define I2CSTR_SCD_MASK  (1<<5)

#if !(I2C_USE_INTERRUPTS)
void i2c_t::interrupt_process( void ){
    i2c_t::I2C_InterruptSource intSource = I2C_INTSRC_NONE;
    uint16_t i2cstreg = I2caRegs.I2CSTR.all;//zmieniony sposob odwolania do STR
    if( i2cstreg & I2CSTR_ARDY_MASK ){      // 1) jeden odczyt z rejestu zamiast 2ch
        I2caRegs.I2CSTR.bit.ARDY = 1;       // 2) skasowanie jak najszybciej a nie po int_process()
        intSource = I2C_INTSRC_REG_ACCESS_RDY;
        dbg_marker('d');
    }else if( i2cstreg & I2CSTR_SCD_MASK ){
        I2caRegs.I2CSTR.bit.SCD = 1;
        intSource = I2C_INTSRC_STOP_CONDITION;
        dbg_marker('g');
    }

    if( intSource != I2C_INTSRC_NONE )
        interrupt_process( intSource );

}

#endif


void i2c_t::interrupt_process( i2c_t::I2C_InterruptSource intSource ){

    uint16_t i;
    //dbg_marker('%');
    //dbg_marker('0'+state);

    if( _buffer == NULL )
         return;

    if(intSource == i2c_t::I2C_INTSRC_STOP_CONDITION)
    {
        if( state == i2c_t::I2C_STATUS_WRITE_BUSY){
            // jesli wlasnie zakonczono wiadomosc wysylania polecenia
            _buffer->ready = 1;
            state = i2c_t::I2C_STATUS_INACTIVE;
        }else if( state == i2c_t::I2C_STATUS_SEND_NOSTOP_BUSY){
            //state = i2c_t::MSG_STATUS_SEND_NOSTOP; //przygotowanie do ponownej proby, gdy brak bylo ACK
            //usunieto, bo
            return;
        }else if( state == i2c_t::I2C_STATUS_READ_BUSY){
            //zakonczono odczyt danych, odczytanie z fifo

            for(i=0; i < _buffer->len; i++) {
                if( i >= MAX_BUFFER_SIZE ) break;
                _buffer->data[i] = I2caRegs.I2CDRR.bit.DATA;
            }
            _buffer->ready = 1;
            state = i2c_t::I2C_STATUS_INACTIVE;
        }
    }else if(intSource == i2c_t::I2C_INTSRC_REG_ACCESS_RDY){

        // tutaj wykrywane jest zakonczenie 1 fazy odczytu danych
        // poniewaz nie bylo STOP (zamierzone i celowe) to ta opcja uzywa ARDY zamiast flagi SCD

        // If a NACK is received, clear the NACK bit and command a stop.
        // Otherwise, move on to the read data portion of the communication.

        if(( I2caRegs.I2CSTR.bit.NACK) != 0){
            I2caRegs.I2CMDR.bit.STP = 1;
            I2caRegs.I2CSTR.bit.NACK = 1;

        }else if( state == i2c_t::I2C_STATUS_SEND_NOSTOP_BUSY){
            state = i2c_t::I2C_STATUS_RESTART; //sygnalizuje ze mozna uruchamiac druga faze odczytu
            _buffer->ready = 1;
        }
    }else if(intSource == i2c_t::I2C_INTSRC_NONE ){
        return;

    }else{
        // nieoczekiwane zrodlo przerwania
        // obecnie ignorowane
    }

}




