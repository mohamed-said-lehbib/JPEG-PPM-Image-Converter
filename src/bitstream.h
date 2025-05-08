#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>


typedef struct {
    const uint8_t *data;
    int octet_posi;       
    int bit_posi;          
    int size;             
} BitStream;


BitStream create_bitstream(const uint8_t *data, int size);


int read_bit(BitStream *bs);


int read_bits(BitStream *bs, int n);

#endif
