//Tomasz  Œwiêchowicz swiechowicz.tomasz@gmail.com

#include <Modbus_RTU.h>

#define LSPCLK 50e6
#define CPU_CLK 200e6
#define CALC_BAUDRATE(x) (LSPCLK/((float)x*8.0f)-1.0f)

Uint16 Modbus_RTU_class::send_data()
{
    if(state != Modbus_RTU_idle || data_out_length == 0) return 1;
    transmit_index = 0;
    EALLOW;
    *(&InputXbarRegs.INPUT7SELECT + (ECapRegs - &ECap1Regs)) = TX_pin;
    EDIS;
    ECapRegs->TSCTR = 0;
    state = Modbus_RTU_send_response;
    return 0;
}

void Modbus_RTU_class::init(struct Modbus_RTU_parameters_struct *Modbus_RTU_parameters)
{
    this->DERE_pin = Modbus_RTU_parameters->DERE_pin;
    this->TX_pin = Modbus_RTU_parameters->TX_pin;
    this->RX_pin = Modbus_RTU_parameters->RX_pin;
    this->baudrate = Modbus_RTU_parameters->baudrate;
    this->use_DERE = Modbus_RTU_parameters->use_DERE;
    this->ECapRegs = Modbus_RTU_parameters->ECapRegs;
    this->SciRegs = Modbus_RTU_parameters->SciRegs;

    GPIO_Setup(RX_pin);

    if(use_DERE)
    {
        GPIO_Setup(DERE_pin);
    }

    EALLOW;
    CpuSysRegs.PCLKCR7.all |= 1<<(SciRegs - &SciaRegs);
    EDIS;

    SciRegs->SCICTL1.bit.SWRESET = 0;
    SciRegs->SCIFFTX.bit.SCIRST = 0;
    SciRegs->SCIFFTX.bit.TXFIFORESET = 0;
    SciRegs->SCIFFRX.bit.RXFIFORESET = 0;

    SciRegs->SCICCR.all = 0x0007;      // 1 stop bit,  No loopback
                                     // No parity,8 char bits,
                                     // async mode, idle-line protocol
    SciRegs->SCICTL1.bit.TXENA = 1;
    SciRegs->SCICTL1.bit.RXENA = 1;

    Uint16 Baudrate_reg = CALC_BAUDRATE(baudrate);
    SciRegs->SCIHBAUD.bit.BAUD = Baudrate_reg >> 8;
    SciRegs->SCILBAUD.bit.BAUD = Baudrate_reg;
    SciRegs->SCIFFTX.bit.SCIFFENA = 1;

    SciRegs->SCICTL1.bit.SWRESET = 1;
    SciRegs->SCIFFTX.bit.SCIRST = 1;
    SciRegs->SCIFFTX.bit.TXFIFORESET = 1;
    SciRegs->SCIFFRX.bit.RXFIFORESET = 1;

    SciRegs->SCITXBUF.all = 0xA;

    EALLOW;
    CpuSysRegs.PCLKCR3.all |= 1<<(ECapRegs - &ECap1Regs);
    EDIS;

    EALLOW;
    *(&InputXbarRegs.INPUT7SELECT + (ECapRegs - &ECap1Regs)) = RX_pin;         // Set eCAPx source to GPIO-pin
    EDIS;

    //
    // Configure peripheral registers
    //
    ECapRegs->ECCTL2.bit.CONT_ONESHT = 0;   // Continuous
    ECapRegs->ECCTL2.bit.STOP_WRAP = 1;     // Stop at 2 events
    ECapRegs->ECCTL1.bit.CAP1POL = 0;       // Rising edge
    ECapRegs->ECCTL1.bit.CAP2POL = 1;       // Falling edge
    ECapRegs->ECCTL1.bit.CTRRST1 = 1;       // Reset counter after latch
    ECapRegs->ECCTL1.bit.CTRRST2 = 1;       // Reset counter after latch
    ECapRegs->ECCTL1.bit.CAPLDEN = 1;       // Enable capture units

    ECapRegs->ECCTL2.bit.TSCTRSTOP = 1;     // Start Counter
    ECapRegs->ECCTL2.bit.REARM = 1;         // arm one-shot
    ECapRegs->ECCTL1.bit.CAPLDEN = 1;       // Enable CAP1-CAP4 register loads

    Sci_words_x35 = ((float)CPU_CLK / (float)baudrate) * 3.5f * 10.0f;//3.5 times 10bits
    Sci_words_x1 = ((float)CPU_CLK / (float)baudrate) * 1.0f * 10.0f;//1.0 times 10bits

    while(!SciRegs->SCICTL2.bit.TXRDY);
    GPIO_Setup(TX_pin);

    state = Modbus_RTU_idle;

}

Uint16 Modbus_RTU_class::get_sci_id(void) const{
    if( this->SciRegs == &SciaRegs ) return 1;
    if( this->SciRegs == &ScibRegs ) return 2;
    if( this->SciRegs == &ScicRegs ) return 3;
    if( this->SciRegs == &ScidRegs ) return 4;
    return 0;
}

#pragma CODE_SECTION(".TI.ramfunc");
void Modbus_RTU_class::interrupt_task()
{
    __HOOK_ENTRY_RTU_interrupt_task;

    if(SciRegs->SCIRXST.bit.RXERROR)
    {
        SciRegs->SCICTL1.bit.SWRESET =
        SciRegs->SCIFFTX.bit.SCIRST =
        SciRegs->SCIFFTX.bit.TXFIFORESET =
        SciRegs->SCIFFRX.bit.RXFIFORESET = 0;
        SciRegs->SCICTL1.bit.SWRESET =
        SciRegs->SCIFFTX.bit.SCIRST =
        SciRegs->SCIFFTX.bit.TXFIFORESET =
        SciRegs->SCIFFRX.bit.RXFIFORESET = 1;
    }

    switch(state)
    {
        case Modbus_RTU_init:
        {
            break;
        }

        case Modbus_RTU_idle:
        {
            if(use_DERE) GPIO_CLEAR(DERE_pin);

            // Uwaga! [PR 2022-01-13]
            //       potencjalny problem wynikajacy z jednokierunkowej synchronizacji.
            //       Jest sygnalizacja RTU.interupt_task->ADU.task (=ze jest wiadomosc do przetworzenia),
            //       ale brakuje sygnalizacji zwrotnej ADU.task->RTU.interrupt_task (=ze wiad. jest skopiowana =>bufor jest zwolniony,
            //                                                                  mozna odbierac nastepna wiadomosc)
            //
            //  Task z petli glownej (obsluga modbus ADU), gdy wykorzysta dane sygnalizuje data_ready=0,
            //  jednakze interrupt_task nie zwraca uwagi na to czy wiadomosc zostala juz wykorzystana czy jeszcze nie.
            //  Jesli zdarzy sie, ze przyjda jakies dane a task jeszcze tego nie przetworzy,
            //  to bufor odbiorczy bedzie zamazywany nowo odbieranymi danymi.
            //
            //  Aby to naprawic nalezy powstrzymac sie od wpisywania odbieranych danych do bufora odbiorczego data_in[]
            //  do czasu uzyskania data_ready = 0 (zapewne w stanie Modbus_RTU_idle).
            //
            //  Do przemyslenia wplyw takiej poprawki z uwzglednieniem:
            //  1.wplyw timera
            //  2.nic nie robic, czy tylko odczytywac z uarta i nigdzie nie zapisywac?
            //  3.zachowanie gdy bedzie wiele masterow na modbus lub beda odbierane wiadomsoci do innych urzadzen na tej mag. modbus
            //
            //  Spotrzezenie:
            //  -------------
            //  Mechanizm przekazywania danych z RTU.interrupt_task a ADU.task mozna rozumiec
            //  jako MailBox (=kolejka o pojemnosci 1 elementu).
            //
            //  Taki obiekt moze byc zajety albo wolny (do tego sluzy uzywana tu flaga data_ready).
            //  Aby to bylo wlasciwie zsynchronizowane:
            //   - RTU_int_task->ADU.task wskazuje ze cos zostalo wstawione do MBoxa
            //   - ale tez RTU_int nie moze wstawiac (ani modyfikowac) MBox dopoki MBox nie zostanie zwolniony (=do czasu gdy ADU.task nie uzyje
            //     tresci zawartej w MBox i nie zasygnalizuje tego)
            //


            if(SciRegs->SCIFFRX.bit.RXFFST)
            {
                data_in_length = 0;
                EALLOW;
                *(&InputXbarRegs.INPUT7SELECT + (ECapRegs - &ECap1Regs)) = RX_pin;
                EDIS;
                ECapRegs->TSCTR = 0;
                state = Modbus_RTU_receive_request;
            }
            break;
        }

        case Modbus_RTU_receive_request:
        {
            while(SciRegs->SCIFFRX.bit.RXFFST)
            {
                if(data_in_length >= 256) data_in_length = 255;
                __HOOK_RTU_int_task_char_received;
                data_in[data_in_length++] = SciRegs->SCIRXBUF.bit.SAR;
            }

            if(ECapRegs->TSCTR > Sci_words_x35 && !SciRegs->SCIFFRX.bit.RXFFST)
            {
                __HOOK_RTU_int_task_MsgReceived;
                data_ready = 1;
                state = Modbus_RTU_idle;
            }
            break;
        }

        case Modbus_RTU_send_response:
        {
            if(use_DERE) {
                GPIO_SET(DERE_pin);
                __HOOK_RTU_int_task_DERE_on;
            }
            if(transmit_index < data_out_length)
            {
                while(SciRegs->SCIFFTX.bit.TXFFST < 16 && transmit_index < data_out_length)
                {
                    SciRegs->SCITXBUF.all = data_out[transmit_index++];
                }
            }
            else if(ECapRegs->TSCTR > Sci_words_x1 && SciRegs->SCICTL2.bit.TXEMPTY)
            {
                if(use_DERE) {
                    GPIO_CLEAR(DERE_pin);
                    __HOOK_RTU_int_task_DERE_off;
                }
                state = Modbus_RTU_idle;
            }
            break;
        }

        default:
        {
            state = Modbus_RTU_init;
            break;
        }
    }
    __HOOK_EXIT_RTU_interrupt_task;
}
