/*
 * Flash.h
 *
 *  Created on: 19.11.2017
 *      Author: Mr.Tea
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "F28x_Project.h"
#include "F021_F2837xD_C28x.h"

enum Sector_enum
{
    SectorA,
    SectorB,
    SectorC,
    SectorD,
    SectorE,
    SectorF,
    SectorG,
    SectorH,
    SectorI,
    SectorJ,
    SectorK,
    SectorL,
    SectorM,
    SectorN
};

class FLASH_class
{
public:
    Uint16 *address[10];
    Uint16 size16_each[10];
    enum Sector_enum sector;

    void save(void) const;
    Uint16 retrieve(Uint16 offset_from_last = 0) const;

private:
    void erase(void) const;
    Uint16 *find(Uint16 offset_from_last = 0) const;

    void erase_and_check(Uint32 sector_start_address) const;
    void save_and_check( void * bufor_FLASH, Uint16 * bufor) const;
};


#endif /* FLASH_H_ */
