#ifndef DECODE_HUFF_H
#define DECODE_HUFF_H

#include <stdint.h>
#include "bitstream.h"

typedef struct Huff_arb {
    uint8_t est_mot_de_code;
    uint8_t symbole;
    struct Huff_arb *fg, *fd;
} Huff_arb;

Huff_arb *create_node();
void insert_code(Huff_arb *arbre, uint16_t code, uint8_t symbole, uint16_t longueur);
int decode_dc(Huff_arb *arbre, int dc_init, BitStream *bs);
void free_arbre(Huff_arb *arbre);

int* decode_all_ac(Huff_arb *arbre, BitStream *bs);
#endif
