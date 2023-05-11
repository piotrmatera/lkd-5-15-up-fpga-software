//Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

#ifndef SOFTWARE_MODBUS_RTU_H_
#define SOFTWARE_MODBUS_RTU_H_

#include "stdafx.h"
#include "DbgGpio.h"
class Modbus_RTU_class
{
public:
    struct Modbus_RTU_parameters_struct
    {
        volatile struct SCI_REGS *SciRegs;
        volatile struct ECAP_REGS *ECapRegs;
        Uint16 TX_pin;
        Uint16 RX_pin;
        Uint16 DERE_pin;
        Uint16 use_DERE;
        Uint32 baudrate;
    };

    Modbus_RTU_class()
    {
        state = Modbus_RTU_init;
        data_ready = 0;
    }

    void init(struct Modbus_RTU_parameters_struct *Modbus_RTU_parameters);
    Uint16 send_data();
    void interrupt_task();
    Uint16 is_data_ready()
    {
        Uint16 data_ready_temp = data_ready;
        if(data_ready_temp) data_ready = 0;
        return data_ready_temp;
    }

    Uint16 is_data_ready_signalled(){
        return data_ready;
    }

    //Uwaga! wywolywac tylko gdy stwierdzono is_data_ready_signalled
    void signal_data_processed(){
        __HOOK_RTU_signal_data_processed;
        data_ready = 0;
    }

    char data_in[256];
    Uint16 data_in_length;
    char data_out[256];
    Uint16 data_out_length;

    enum state_enum
    {
        Modbus_RTU_init,
        Modbus_RTU_idle,
        Modbus_RTU_receive_request,
        Modbus_RTU_send_response,
    }state;

    Uint16 get_sci_id(void) const;
private:

    Uint16 data_ready;
    Uint32 Sci_words_x35;
    Uint32 Sci_words_x1;

    volatile struct SCI_REGS *SciRegs;
    volatile struct ECAP_REGS *ECapRegs;
    Uint16 TX_pin;
    Uint16 RX_pin;
    Uint16 DERE_pin;
    Uint16 use_DERE;
    Uint32 baudrate;

    Uint16 transmit_index;
};

#endif /* SOFTWARE_MODBUS_RTU_H_ */
