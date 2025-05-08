#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decde_Huff.h"
#include "bitstream.h"

// Créer un bitstream
BitStream create_bitstream(BitStream *bs, const uint8_t *data, int m) {
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

// Décodage du coefficient DC
int decode_dc(Huff_arb *arbre, int dc_init, BitStream *bs) {
    Huff_arb *courant = arbre;
    while (courant->est_mot_de_code == 0) {
        int bit = read_bit(bs);
        if (bit == 0) {
            courant = courant->fg;
        } else {
            courant = courant->fd;
        }
    }
    int m = courant->symbole;
    int indice = read_bits(bs, m);
    int DC = 0;
    if (m == 0) {
        DC = dc_init;
    }
    else if ((indice >> (m - 1)) & 1) {
        DC = indice + dc_init;
    }
    else {
        DC = indice - (1 << m) + 1;
        DC = DC + dc_init;
    }
    return DC;
}

// Décodage de tous les coefficients AC
int* decode_all_ac(Huff_arb *arbre, BitStream *bs) {
    int *coef_ac = malloc(63 * sizeof(int));
    if (coef_ac == NULL) {
        fprintf(stderr, "erreur d'allocation memoire pour les coefficients AC\n");
        exit(1); 
    }

    int k = 0;
    while (k < 63) {
        Huff_arb *courant = arbre;
        while (!courant->est_mot_de_code) {
            int bit = read_bit(bs);
            courant = (bit == 0) ? courant->fg : courant->fd;
        }

        int symbol = courant->symbole;
        int rle = symbol >> 4;  
        int m = symbol & 0x0F;  // la magnitude

        //fin de la séquence (0x00)
        if (symbol == 0x00) {
            while (k < 63) coef_ac[k++] = 0;
            break;
        }

        // Cas ou il faut ajouter 16 zeros (0xF0)
        if (symbol == 0xF0) {
            for (int i = 0; i < 16; i++) coef_ac[k++] = 0;
            continue;
        }

        // Lire l'indice du coefficient AC
        int indice = read_bits(bs, m);
        int AC = ((indice >> (m - 1)) & 1) ? indice : indice - (1 << m) + 1;


        for (int i = 0; i < rle; i++) coef_ac[k++] = 0;
        coef_ac[k++] = AC;
    }

    return coef_ac;
}
