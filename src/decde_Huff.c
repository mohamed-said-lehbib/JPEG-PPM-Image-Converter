#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "decde_Huff.h"


// Création d'un noeud
Huff_arb *create_node() {
    Huff_arb* noeud = malloc(sizeof(Huff_arb)); 
    noeud->est_mot_de_code = 0;
    noeud->symbole = 0;
    noeud->fg = NULL;
    noeud->fd = NULL;
    return noeud;
}

// Insertion d'un mot de code
void insert_code(Huff_arb *arbre, uint16_t code, uint8_t symbole, uint16_t longueur) {
    Huff_arb *courrent = arbre; 
    for (int i = longueur - 1; i >= 0; i--) {
        uint8_t bit = (code >> i) & 1;
        if (bit == 0) {
            if (!courrent->fg) courrent->fg = create_node();
            courrent = courrent->fg;
        } else {
            if (!courrent->fd) courrent->fd = create_node();
            courrent = courrent->fd;
        }
    }
  
    courrent->est_mot_de_code = 1;
    courrent->symbole = symbole;
}
void afficher_arbre(Huff_arb *arbre) {
    if (arbre == NULL) return;

    if (arbre->est_mot_de_code) {
        printf("Feuille: symbole = 0x%02X\n", arbre->symbole);
    } else {
        printf("Noeud interne\n");
    }

    afficher_arbre(arbre->fg); 
    afficher_arbre(arbre->fd); 
}

// Décodage du coefficient DC
int decode_dc(Huff_arb *arbre, int dc_init, BitStream *bs) {
    if (arbre == NULL || bs == NULL) {
        fprintf(stderr, "Erreur : arbre Huffman ou BitStream NULL dans decode_dc\n");
        exit(EXIT_FAILURE);
    }
    Huff_arb *courant = arbre;
    while (courant->est_mot_de_code == 0) {
        int bit = read_bit(bs);
        if (bit == 0) {
            if (courant->fg == NULL) {
                fprintf(stderr, "Erreur : noeud gauche NULL dans decode_dc\n");
                exit(EXIT_FAILURE);
            }
            courant = courant->fg;
        } else {
            if (courant->fd == NULL) {
                fprintf(stderr, "Erreur : noeud droit NULL dans decode_dc\n");
                exit(EXIT_FAILURE);
            }
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
/*

void free_arbre(Huff_arb *arbre) {
    if (!arbre) return;
    free_arbre(arbre->fg);
    free_arbre(arbre->fd);
    free(arbre);
}
int main(void) {
    uint8_t table[16] = { 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00}; // exemple
    uint8_t symbols[2] = {0x07}; // exemple de symboles

    Huff_arb *arbre = create_node();
    uint16_t code = 0;
    int k = 0;
    for (int i = 0; i < 16; i++) {
        int len = i + 1;
        for (int j = 0; j < table[i]; j++) {
          
            insert_code(arbre, code, symbols[k], len);
            code++;
            k++;
        }
        code <<= 1;
    }
    
    printf("Arbre Huffman construit avec succès.\n");
    afficher_arbre(arbre);
    uint8_t data[] = { 0b01111100 };
    BitStream bs;
    create_bitstream(&bs, data, 1);
    int dc_init = 0;
    int dc = decode_dc(arbre, dc_init, &bs);
    printf("Valeur DC décodée : %d\n", dc);
    free_arbre(arbre);
    return 0;
}
*/
