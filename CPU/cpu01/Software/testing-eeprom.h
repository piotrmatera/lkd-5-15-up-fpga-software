/*
 * testing-eeprom.h
 *
 *  Created on: 17 paü 2023
 *      Author: Piotr
 */

#ifndef SOFTWARE_TESTING_EEPROM_H_
#define SOFTWARE_TESTING_EEPROM_H_

#define TESTING_NONVOLATILE 0

#define TESTING_EEPROM_I2C 0

#if TESTING_NONVOLATILE
# define testing_eeprom_nv() test_nv()
void test_nv(void);

#elif TESTING_EEPROM_I2C
# define testing_eeprom_nv() test_eeprom()

#endif

#if TESTING_NONVOLATILE || TESTING_EEPROM_I2C
# define TESTING_EEPROM_NV 1
//# error UWAGA obiekt nonvolatile jest juz uzywany w kodzie zamiast FLASH_class, trzeba to odlaczyc na czas testow
#else
# define TESTING_EEPROM_NV 0
#endif

#if TESTING_NONVOLATILE && TESTING_EEPROM_I2C
# error Zdefiniuj tylko jeden test EEPROM albo NV
#endif




#endif /* SOFTWARE_TESTING_EEPROM_H_ */
