#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../include/bitstream.h"

// Creer un bitstream
BitStream create_bitstream(BitStream *bs,  uint8_t *data, int m) {
    bs->data = data; // le vecteur de donnees 
    bs->octet_posi = 0; // position de l'octet
    bs->bit_posi = 0; // position du bit
    bs->size = m; // taille du vecteur de donnees
    return *bs; 
}

// Fonction pour lire n bits du flux
int read_bits(BitStream *bs, int n) {
    int result = 0;
    for (int i = 0; i < n; i++) {
        int bit = (bs->data[bs->octet_posi] >> (7 - bs->bit_posi)) & 1; // on recupere le premier bit a partir de la gauche
        bs->bit_posi++;
        if (bs->bit_posi == 8) { // si on a lu 8 bits, on passe a l'octet suivant
            bs->bit_posi = 0;
            bs->octet_posi++;
            if (bs->data[bs->octet_posi - 1] == 0xff) {// le cas du byte stuff 
            bs->octet_posi += 1 ;
            }

        }
        result = (result << 1) | bit;
    }
    return result;
}

