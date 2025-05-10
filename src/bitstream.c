#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "bitstream.h"

// Créer un bitstream
BitStream create_bitstream(BitStream *bs,  uint8_t *data, int m) {
    bs->data = data;
    bs->octet_posi = 0;
    bs->bit_posi = 0;
    bs->size = m;
    return *bs;
}

// Fonction pour lire un bit du flux
int read_bit(BitStream *bs) {
    if (bs->octet_posi >= bs->size) {
        return -1; 
    }
    int bit = (bs->data[bs->octet_posi] >> (7 - bs->bit_posi)) & 1;
    bs->bit_posi++;
    if (bs->bit_posi == 8) {
        bs->bit_posi = 0;
        bs->octet_posi++;
    }
    return bit;
}

// Fonction pour lire n bits du flux
int read_bits(BitStream *bs, int n) {
    int result = 0;
    for (int i = 0; i < n; i++) {
        int bit = read_bit(bs);
        if (bit == -1) {
            return -1; 
        }
        result = (result << 1) | bit;
    }
    return result;
}

