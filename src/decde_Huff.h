#ifndef DECODE_HUFF_H
#define DECODE_HUFF_H

#include <stdint.h>
#include "bitstream.h"
typedef struct Huff_arb {
    uint8_t est_mot_de_code;
    uint8_t symbole;
    struct Huff_arb *fg, *fd;
} Huff_arb;
typedef struct {
    int16_t** data;
} matrice ;
typedef struct {
    int16_t data[64];
} Bloc;// definir une structure Bloc qui contient un tableau qui stocke les coefficient d'un bloc apres le decodage de Huffman 
typedef struct {
    uint8_t nb_bloc ; 
    Bloc *blocs;
} Comp ; 
typedef struct {
    Comp *comps;
    uint8_t nb_comp;
} MCU ; 

Huff_arb *create_node();
void insert_code(Huff_arb *arbre, uint16_t code, uint8_t symbole, uint16_t longueur);
int decode_dc(Huff_arb *arbre, int dc_init, BitStream *bs);
void free_arbre(Huff_arb *arbre);
void afficher_arbre(Huff_arb *arbre);
MCU *decode_bloc(Huff_arb **arbre_dc, Huff_arb **arbre_ac, BitStream *bs,uint16_t nb_mcux,uint16_t nb_mcuy,uint8_t N_comp,uint8_t *huff_corr_dc,uint8_t *huff_corr_ac, uint16_t nb[] );
int* decode_all_ac(Huff_arb *arbre, BitStream *bs);
#endif
