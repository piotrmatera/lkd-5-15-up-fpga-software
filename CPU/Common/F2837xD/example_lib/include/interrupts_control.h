/*
 * interrupts_control.h
 *
 *  Created on: 21 paü 2023
 *      Author: Piotr
 *
 *  2023(c) Lopi elektronika
 */

#ifndef SOFTWARE_COMMON_INTERRUPTS_CONTROL_H_
#define SOFTWARE_COMMON_INTERRUPTS_CONTROL_H_

#ifdef __cplusplus
extern "C"{
#endif

/** wylacza przerwania i zwraca stan przed wylaczeniem
 * UWAGA! moze byc nieatomowy test-set
 * potencjalnie grozne jesli wtraci sie przerwanie, ktore wylaczy przerwania
 * Funkcja zdefiniowana w common/interrupts_control.asm
 */
Uint16 _custom_disable_interrupts(void);


/**przywraca stan przerwan przed wywolaniem _custom_disable_interrupts()
 * @param[in] previous poprzedni stan przerwan zwrocony przez _custom_disable_interrupts()
 */
static inline void _custom_restore_interrupts(Uint16 previous){
    if( 0U == (previous & 0x01) )
        EINT;
}

#ifdef __cplusplus
}
#endif

#endif /* SOFTWARE_COMMON_INTERRUPTS_CONTROL_H_ */
