#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decde_Huff.h"
#include "bitstream.h"
// creer un bitstream
BitStream create_bitstream( BitStream *bs, const uint8_t *data, int size) {
    bs->data = data;
    bs->octet_posi = 0;
    bs->bit_posi = 0;
    bs->size = size;
    return *bs;
}

// creer une fonction qui prent en paramatetre un flux de bits et le consome bit par bit
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

// lire n bits
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


int decode_dc(Huff_arb *arbre , int *dc_init, BitStream *bs) {
    int code = 0;
    int i = 0;
    Huff_arb *courant = arbre;
    while (courant->est_mot_de_code == 0) {
        int bit = read_bit(bs);
        if (bit == 0) {
            courant = courant->fg;
        } else {
            courant = courant->fd;
        }
        code = (code << 1) | bit;
    int m = (code == 0) ? 0 : (int)floor(log2(abs(code))) + 1;
    int indice = read_bits(bs, m);

}