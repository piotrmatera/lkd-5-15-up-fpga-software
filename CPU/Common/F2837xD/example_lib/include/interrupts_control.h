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

/** odczytanie rejestru ST1 */
Uint16 _custom_read_st1(void);

/** odpowiednik ReadIpcTimer z biblioteki ale zabezp. przed
 * przerwaniem w trakcie czytania rejestrow
 * zoptymalizowane w asemblerze
 *
 * Uwagi:
 * - zabezpieczone przed wtraceniem ISR
 * - wylacza przerwania na 3 cykle CPU
 * - odtwarza stan przerwan po odczycie rejestrow
 *
 */
Uint64 _custom_read_ipc_timer(void);

/** okresla czy przerwnia sa zamaskowane (wylaczone)*/
#define _are_interrupts_masked() ( (_custom_read_st1() & 0x01) == 1U )



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
