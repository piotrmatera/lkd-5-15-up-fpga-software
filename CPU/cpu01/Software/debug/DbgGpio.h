/*
 * DbgGpio.h
 *
 *  Created on: 13 sty 2022
 *      Author: Piotr
 */

#ifndef SOFTWARE_DEBUG_DBGGPIO_H_
#define SOFTWARE_DEBUG_DBGGPIO_H_


//sygnalizacja koncowkami GPIO zaleznosci czasowych
// uzywane LEDy LED1, LED2, LED4, LED5

#define DBG_AT_GPIO  0

#define DBG_AT_GPIO_INIT_TEST_PATTERN 1

#define __NOTHING() do{ }while(0)

#if DBG_AT_GPIO
# define __HOOK_ENTRY_SMachine_Background     GPIO_WRITE(LED1,1)
# define __HOOK_EXIT_SMachine_Background      GPIO_WRITE(LED1,0)
# define __HOOK_ADU_slave_general_Task_w_data GPIO_TOGGLE(LED2)
# define __HOOK_ENTRY_RTU_interrupt_task __NOTHING()  // GPIO_WRITE(LED1,1)
# define __HOOK_EXIT_RTU_interrupt_task  __NOTHING()  // GPIO_WRITE(LED1,0)
# define __HOOK_RTU_int_task_char_received __NOTHING() //GPIO_TOGGLE(LED2)
# define __HOOK_RTU_int_task_DERE_on __NOTHING()            //    GPIO_WRITE(LED5,1)
# define __HOOK_RTU_int_task_DERE_off __NOTHING()            //    GPIO_WRITE(LED5,0)
# define __HOOK_RTU_int_task_MsgReceived    GPIO_TOGGLE(LED4)
# define __HOOK_RTU_signal_data_processed   GPIO_TOGGLE(LED5)
#else
# define __HOOK_ENTRY_SMachine_Background     __NOTHING()
# define __HOOK_EXIT_SMachine_Background      __NOTHING()
# define __HOOK_ADU_slave_general_Task_w_data __NOTHING()
# define __HOOK_ENTRY_RTU_interrupt_task      __NOTHING()
# define __HOOK_EXIT_RTU_interrupt_task       __NOTHING()
# define __HOOK_RTU_int_task_char_received    __NOTHING()
# define __HOOK_RTU_int_task_DERE_on          __NOTHING()
# define __HOOK_RTU_int_task_DERE_off         __NOTHING()
# define __HOOK_RTU_int_task_MsgReceived      __NOTHING()
# define __HOOK_RTU_signal_data_processed     __NOTHING()

#endif

#endif /* SOFTWARE_DEBUG_DBGGPIO_H_ */
