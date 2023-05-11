/*
 * Translator_map.cpp
 *
 *  Created on: 8 wrz 2021
 *      Author: Piotr
 */

#include "Translator_map.h"


translator_map Mdb_translator_map;

const struct translator_map::translator_map_item translator_map::translator_map_table[]={

     { translator_map::spec_get_ilim, 0, NULL},//       0000 - prad (wielkosc wczytywana z pliku konfiguracyjnego, obecnie 8 lub 16)
     { translator_map::float_to_int16, 340, (void*)1 },//       0001 - U1 [V] adres 340
     { translator_map::float_to_int16, 342, (void*)1 },//       0002 - U2 [V]
     { translator_map::float_to_int16, 344, (void*)1 },//       0003 - U3 [V]
     { translator_map::float_to_int16, 334, (void*)1000},// 0004 - THDU1 [0.1%] adres  334
     { translator_map::float_to_int16, 336, (void*)1000},// 0005 - THDU2
     { translator_map::float_to_int16, 338, (void*)1000},// 0006 - THDU3
     { translator_map::float_to_int16, 396, (void*)10},//   0007 - Czestotliwosc [0.1Hz] adres 396
     { translator_map::max2int16_to_int16, 140, (void*)1},//0008 - temperatura LKD [*C] max( Temp1, Temp2) adres 138, 139
     { translator_map::int16_to_int16, 142, (void*)1},//    0009 - temperatura zewnetrzna [*C] Temp3 adres 140
//
     { translator_map::float_to_int16, 346, (void*)1},//    0010 - Prad I1 [A] adres 346
     { translator_map::float_to_int16, 348, (void*)1},//    0011 - Prad I2 [A]
     { translator_map::float_to_int16, 350, (void*)1},//    0012 - Prad I3 [A]
     { translator_map::float_to_int16, 328, (void*)1000},// 0013 - THDI1 [0.1%] adres 328
     { translator_map::float_to_int16, 330, (void*)1000},// 0014 - THDI2 [0.1%]
     { translator_map::float_to_int16, 332, (void*)1000},// 0015 - THDI3 [0.1%]
     { translator_map::float_to_int16, 380, (void*)100000},// 0016 - PF1 [0.001%] adres 380
     { translator_map::float_to_int16, 380, (void*)100000},// 0017 - PF1 [0.001%]
     { translator_map::float_to_int16, 380, (void*)100000},// 0018 - PF1 [0.001%]
     { translator_map::float_to_int16, 360, (void*)1},// 0019 - Moc pozorna faza 1 [VA] adres 360
     { translator_map::float_to_int16, 362, (void*)1},// 0020 - Moc pozorna faza 2 [VA]
     { translator_map::float_to_int16, 364, (void*)1},//       0021 - Moc pozorna faza 3 [VA]
     { translator_map::float_to_int16, 268, (void*)1},//       0022 - Moc czynna P50Hz faza 1 [W] adres 268
     { translator_map::float_to_int16, 270, (void*)1},//       0023 - Moc czynna P50Hz faza 2 [W]
     { translator_map::float_to_int16, 272, (void*)1},//       0024 - Moc czynna P50Hz faza 3 [W]
     { translator_map::float_to_int16, 286, (void*)1},//       0025 - Moc bierna P50Hz faza 1 [Var] adres 286
     { translator_map::float_to_int16, 288, (void*)1},//       0026 - Moc bierna P50Hz faza 2 [Var]
     { translator_map::float_to_int16, 290, (void*)1},//       0027 - Moc bierna P50Hz faza 3 [Var]
     { translator_map::float_to_int16, 352, (void*)1},//       0028 - Prad kompensatora I1 [A] adres 352
     { translator_map::float_to_int16, 354, (void*)1},//       0029 - Prad kompensatora I2 [A]
     { translator_map::float_to_int16, 356, (void*)1},//       0030 - Prad kompensatora I3 [A]
     { translator_map::float_to_int16, 372, (void*)100},//       0031 - Zasoby faza 1 [%] adres 372
     { translator_map::float_to_int16, 374, (void*)100},//       0032 - Zasoby faza 2 [%]
     { translator_map::float_to_int16, 376, (void*)100},//       0033 - Zasoby faza 3 [%]
     { translator_map::float_to_int16, 298, (void*)1},//       0034 - Moc bierna kompensatora faza 1 [Var] adres 298
     { translator_map::float_to_int16, 300, (void*)1},//       0035 - Moc bierna kompensatora faza 2
     { translator_map::float_to_int16, 302, (void*)1},//       0036 - Moc bierna kompensatora faza 3
     { translator_map::float_to_int16, 310, (void*)1},//       0037 - Moc pozorna obciazenia 50Hz faza 1 [VA] adres 310
     { translator_map::float_to_int16, 312, (void*)1},//       0038 - Moc pozorna obciazenia 50Hz faza 2 [VA]
     { translator_map::float_to_int16, 314, (void*)1},//       0039 - Moc pozorna obciazenia 50Hz faza 3 [VA]
//
     { translator_map::float_to_int16, 274, (void*)1},//       0040 - Moc czynna obciazenia 50Hz faza 1 [W] adres 274
     { translator_map::float_to_int16, 276, (void*)1},//       0041 - Moc czynna obciazenia 50Hz faza 2 [W]
     { translator_map::float_to_int16, 278, (void*)1},//       0042 - Moc czynna obciazenia 50Hz faza 3 [W]
     { translator_map::float_to_int16, 292, (void*)1},//       0043 - Moc bierna obciazenia 50Hz faza 1 [Var] adres 292
     { translator_map::float_to_int16, 294, (void*)1},//       0044 - Moc bierna obciazenia 50Hz faza 2 [Var]
     { translator_map::float_to_int16, 296, (void*)1},//       0045 - Moc bierna obciazenia 50Hz faza 3 [Var]
     { translator_map::undefined, 0, NULL},          //       0046 - Reserved
     { translator_map::undefined, 0, NULL},         //       0047 - Reserved
     { translator_map::undefined, 0, NULL},//       0048 - Reserved
     { translator_map::undefined, 0, NULL},//       0049 - Reserved
//
     { translator_map::time_to_hour, 165, NULL},//       0050 - Time godzina
     { translator_map::time_to_min, 165, NULL},//       0051 - Time minuta
     { translator_map::time_to_sec, 165, NULL},//       0052 - Sekunda
     { translator_map::time_to_year, 165, NULL},//       0053 - Rok
     { translator_map::time_to_month, 165, NULL},//       0054 - Miesiac
     { translator_map::time_to_day, 165, NULL},//       0055 - Dzieñ
     { translator_map::int16_to_int16, 128, NULL},//       0056 - bit16_alarm0_m
     { translator_map::int16_to_int16, 129, NULL},//       0057 - bit16_alarm1_m
     { translator_map::undefined, 0, NULL},//              0058 - Reserved
     { translator_map::int16_to_int16, 132, NULL},//       0059 - bit16_status0_m
//
     { translator_map::int16_to_int16, 133, NULL},//       0060 - bit16_status1_m

     { translator_map::int16_to_int16, 134, NULL},//       0061 - bit16_status2_m
     { translator_map::int16_to_int16, 135, NULL},//       0062 - bit16_status3_m
     { translator_map::int16_to_int16, 170, NULL},//       0063 - bit16_alarm0_s
     { translator_map::int16_to_int16, 171, NULL},//       0064 - bit16_alarm1_s
     { translator_map::int16_to_int16, 172, NULL},//       0065 - bit16_alarm2_s
     { translator_map::int16_to_int16, 173, NULL},//       0066 - bit16_alarm3_s
     { translator_map::int16_to_int16, 178, NULL},//       0067 - bit16_status0_s
     { translator_map::undefined, 0, NULL},//       0068 - Reserved
     { translator_map::undefined, 0, NULL},//       0069 - Reserved

     { translator_map::int16_to_int16, 400 , NULL},//  0070 P_p0
     { translator_map::int16_to_int16,  401 , NULL},//
     { translator_map::int16_to_int16,  402 , NULL},//  P_p0b
     { translator_map::int16_to_int16,  403 , NULL},//
     { translator_map::int16_to_int16,  404 , NULL},//  P_p1
     { translator_map::int16_to_int16,  405 , NULL},//
     { translator_map::int16_to_int16,  406 , NULL},//  P_p1b
     { translator_map::int16_to_int16,  407 , NULL},//
     { translator_map::int16_to_int16,  408 , NULL},//  P_p2
     { translator_map::int16_to_int16,  409 , NULL},//
     { translator_map::int16_to_int16,  410 , NULL},//  P_p2b
     { translator_map::int16_to_int16,  411 , NULL},//
     { translator_map::int16_to_int16,  412 , NULL},//  P_n0
     { translator_map::int16_to_int16,  413 , NULL},//
     { translator_map::int16_to_int16,  414 , NULL},//  P_n0b
     { translator_map::int16_to_int16,  415 , NULL},//
     { translator_map::int16_to_int16,  416 , NULL},//  P_n1
     { translator_map::int16_to_int16,  417 , NULL},//
     { translator_map::int16_to_int16,  418 , NULL},//  P_n1b
     { translator_map::int16_to_int16,  419 , NULL},//
     { translator_map::int16_to_int16,  420 , NULL},//  P_n2
     { translator_map::int16_to_int16,  421 , NULL},//
     { translator_map::int16_to_int16,  422 , NULL},//  P_n2b
     { translator_map::int16_to_int16,  423 , NULL},//
     { translator_map::int16_to_int16,  424 , NULL},//  QI0
     { translator_map::int16_to_int16,  425 , NULL},//
     { translator_map::int16_to_int16,  426 , NULL},//  QI0b
     { translator_map::int16_to_int16,  427 , NULL},//
     { translator_map::int16_to_int16,  428 , NULL},//  QI1
     { translator_map::int16_to_int16,  429 , NULL},//
     { translator_map::int16_to_int16,  430 , NULL},//  QI1b
     { translator_map::int16_to_int16,  431 , NULL},//
     { translator_map::int16_to_int16,  432 , NULL},//  QI2
     { translator_map::int16_to_int16,  433 , NULL},//
     { translator_map::int16_to_int16,  434 , NULL},//  QI2b
     { translator_map::int16_to_int16,  435 , NULL},//
     { translator_map::int16_to_int16,  436 , NULL},//  QII0
     { translator_map::int16_to_int16,  437 , NULL},//
     { translator_map::int16_to_int16,  438 , NULL},//  QII0b
     { translator_map::int16_to_int16,  439 , NULL},//
     { translator_map::int16_to_int16,  440 , NULL},//  QII1
     { translator_map::int16_to_int16,  441 , NULL},//
     { translator_map::int16_to_int16,  442 , NULL},//  QII1b
     { translator_map::int16_to_int16,  443 , NULL},//
     { translator_map::int16_to_int16,  444 , NULL},//  QII2
     { translator_map::int16_to_int16,  445 , NULL},//
     { translator_map::int16_to_int16,  446 , NULL},//  QII2b
     { translator_map::int16_to_int16,  447 , NULL},//
     { translator_map::int16_to_int16,  448 , NULL},//  QIII0
     { translator_map::int16_to_int16,  449 , NULL},//
     { translator_map::int16_to_int16,  450 , NULL},//  QIII0b
     { translator_map::int16_to_int16,  451 , NULL},//
     { translator_map::int16_to_int16,  452 , NULL},//  QIII1
     { translator_map::int16_to_int16,  453 , NULL},//
     { translator_map::int16_to_int16,  454 , NULL},//  QIII1b
     { translator_map::int16_to_int16,  455 , NULL},//
     { translator_map::int16_to_int16,  456 , NULL},//  QIII2
     { translator_map::int16_to_int16,  457 , NULL},//
     { translator_map::int16_to_int16,  458 , NULL},//  QIII2b
     { translator_map::int16_to_int16,  459 , NULL},//
     { translator_map::int16_to_int16,  460 , NULL},//  QIV0
     { translator_map::int16_to_int16,  461 , NULL},//
     { translator_map::int16_to_int16,  462 , NULL},//  QIV0b
     { translator_map::int16_to_int16,  463 , NULL},//
     { translator_map::int16_to_int16,  464 , NULL},//  QIV1
     { translator_map::int16_to_int16,  465 , NULL},//
     { translator_map::int16_to_int16,  466 , NULL},//  QIV1b
     { translator_map::int16_to_int16,  467 , NULL},//
     { translator_map::int16_to_int16,  468 , NULL},//  QIV2
     { translator_map::int16_to_int16,  469 , NULL},//
     { translator_map::int16_to_int16,  470 , NULL},//  QIV2b
     { translator_map::int16_to_int16,  471 , NULL},//

     { translator_map::int16_to_int16,  472 , NULL},//  0142 vector_P_p
     { translator_map::int16_to_int16,  473 , NULL},//
     { translator_map::int16_to_int16,  474 , NULL},//  vector_P_pb
     { translator_map::int16_to_int16,  475 , NULL},//
     { translator_map::int16_to_int16,  476 , NULL},//  vector_P_n
     { translator_map::int16_to_int16,  477 , NULL},//
     { translator_map::int16_to_int16,  478 , NULL},//  vector_P_nb
     { translator_map::int16_to_int16,  479 , NULL},//
     { translator_map::int16_to_int16,  480 , NULL},//  vector_QI
     { translator_map::int16_to_int16,  481 , NULL},//
     { translator_map::int16_to_int16,  482 , NULL},//  vector_QIb
     { translator_map::int16_to_int16,  483 , NULL},//
     { translator_map::int16_to_int16,  484 , NULL},//  vector_QII
     { translator_map::int16_to_int16,  485 , NULL},//
     { translator_map::int16_to_int16,  486 , NULL},//  vector_QIIb
     { translator_map::int16_to_int16,  487 , NULL},//
     { translator_map::int16_to_int16,  488 , NULL},//  vector_QIII
     { translator_map::int16_to_int16,  489 , NULL},//
     { translator_map::int16_to_int16,  490 , NULL},//  vector_QIIIb
     { translator_map::int16_to_int16,  491 , NULL},//
     { translator_map::int16_to_int16,  492 , NULL},//  vector_QIV
     { translator_map::int16_to_int16,  493 , NULL},//
     { translator_map::int16_to_int16,  494 , NULL},//  vector_QIVb
     { translator_map::int16_to_int16,  495 , NULL},//

     { translator_map::int16_to_int16,  1136 , NULL},//  0166 algebraic_P_p
     { translator_map::int16_to_int16,  1137 , NULL},//
     { translator_map::int16_to_int16,  1138 , NULL},//  algebraic_P_pb
     { translator_map::int16_to_int16,  1139 , NULL},//
     { translator_map::int16_to_int16,  1140 , NULL},//  algebraic_P_n
     { translator_map::int16_to_int16,  1141 , NULL},//
     { translator_map::int16_to_int16,  1142 , NULL},//  algebraic_P_nb
     { translator_map::int16_to_int16,  1143 , NULL},//
     { translator_map::int16_to_int16,  1144 , NULL},//  algebraic_QI
     { translator_map::int16_to_int16,  1145 , NULL},//
     { translator_map::int16_to_int16,  1146 , NULL},//  algebraic_QIb
     { translator_map::int16_to_int16,  1147 , NULL},//
     { translator_map::int16_to_int16,  1148 , NULL},//  algebraic_QII
     { translator_map::int16_to_int16,  1149 , NULL},//
     { translator_map::int16_to_int16,  1150 , NULL},//  algebraic_QIIb
     { translator_map::int16_to_int16,  1151 , NULL},//
     { translator_map::int16_to_int16,  1152 , NULL},//  algebraic_QIII
     { translator_map::int16_to_int16,  1153 , NULL},//
     { translator_map::int16_to_int16,  1154 , NULL},//  algebraic_QIIIb
     { translator_map::int16_to_int16,  1155 , NULL},//
     { translator_map::int16_to_int16,  1156 , NULL},//  algebraic_QIV
     { translator_map::int16_to_int16,  1157 , NULL},//
     { translator_map::int16_to_int16,  1158 , NULL},//  algebraic_QIVb
     { translator_map::int16_to_int16,  1159 , NULL},//
                                                     //190

};

const Uint16 translator_map::translator_map_items = 190;

const struct translator_map::translator_map_item translator_map::undefined_dsc = {
      undefined, 0 , NULL
};

const struct translator_map::translator_map_item * translator_map::get_translation_descriptor( Uint16 index ){
    if( index >= translator_map_items )
        return &undefined_dsc;
    return &this->translator_map_table[index];
}


Uint16 translator_map::get_map_items(){
    return this->translator_map_items;
}
