#ifndef DECODE_HUFF_H
#define DECODE_HUFF_H

#include <stdint.h>

typedef struct Huff_arb {
    uint8_t est_mot_de_code;
    uint8_t symbole;
    struct Huff_arb *fg, *fd;
} Huff_arb;

Huff_arb *create_node();
void insert_code(Huff_arb *arbre, uint16_t code, uint8_t symbole, uint8_t longueur);
Huff_arb *build_huffman_tree(const uint8_t *Li, const uint8_t *symbols);
int decode_dc_coefficient(Huff_arb *arbre_dc, int *previous_dc);

#endif
