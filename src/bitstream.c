#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "bitstream.h"

// Creer un bitstream
BitStream create_bitstream(BitStream *bs,  uint8_t *data, int m) {
    bs->data = data;
    bs->octet_posi = 0;
    bs->bit_posi = 0;
    bs->size = m;
    return *bs;
}

// Fonction pour lire un bit du flux
int read_bit(BitStream *bs) {
    int bit = (bs->data[bs->octet_posi] >> (7 - bs->bit_posi)) & 1;
    bs->bit_posi++;
    if (bs->bit_posi == 8) {
        bs->bit_posi = 0;
        bs->octet_posi++;
        if (bs->data[bs->octet_posi - 1] == 0xff) {
           bs->octet_posi += 1 ;
        }

    }
    return bit;
}

// Fonction pour lire n bits du flux
int read_bits(BitStream *bs, int n) {
    int result = 0;
    for (int i = 0; i < n; i++) {
        int bit = read_bit(bs);
        result = (result << 1) | bit;
    }
    return result;
}

