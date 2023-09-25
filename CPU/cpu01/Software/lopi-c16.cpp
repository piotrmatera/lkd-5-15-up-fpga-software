#include <stdint.h>

#pragma RETAIN

const uint16_t out_enc[] = {
  0x1d9c, 0x562b, 0xf0e7, 0x946c, 0x8a65, 0x707a,
  0x7082, 0x7a72, 0x878a, 0x7881, 0xef34, 0x9f78,
  0xadd8, 0x0c7e, 0xf380, 0x6533
};

//unsigned int out_enc_len = 16;
/*
 * zapisac plik binarny z pamieci w debugerze SaveMemory
 * TI Raw Data
 * 16
 * 16bits
 *
 * Plik wynikowy ma zamieniona kolejnosc bajtow
 *
 * xxd -g2 -e plik.dat > plik.hexdump
 *
 * 00000000: 1d9c 562b f0e7 946c 8a65 707a 7082 7a72  ..+V..l.e.zp.prz
 * 00000010: 878a 7881 ef34 9f78 add8 0c7e f380 6533  ...x4.x...~...3e
 *
 * edycja edytorem
 * dodanie spacji co dwa znaki, usuniecie ascii a konca
 * potem
 *
 * xxd -r plik.hexdump > plik-bytes-ok.bin
 *
 * uzyc skryptu:
 *  encrypt-file.sh -d plik-bytes-ok.bin
 *
 * wynik w pliku out.dec:
 *
 *  2019 (c) LOPI Elektronika
 * */
