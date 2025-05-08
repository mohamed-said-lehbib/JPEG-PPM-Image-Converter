#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include "decde_Huff.h"

typedef struct {
    const uint8_t *data;
    int octet_posi;       
    int bit_posi;          
    int size;             
} BitStream;


BitStream create_bitstream(const uint8_t *data, int size);


int read_bit(BitStream *bs);


int read_bits(BitStream *bs, int n);
int decode_dc(Huff_arb *arbre, int dc_init, BitStream *bs)
int* decode_all_ac(Huff_arb *arbre, BitStream *bs)
#endif
