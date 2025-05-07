#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// Déclaration de la structure
typedef struct Huff_arb {
    uint8_t est_mot_de_code;           
    uint8_t symbole;              
    struct Huff_arb *fg;   
    struct Huff_arb *fd;   
} Huff_arb;

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


int main(void) {
    uint8_t table[16] = {0, 2, 3, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // exemple
    uint8_t symbols[256] = {10, 20, 30, 40, 50, 60}; // exemple de symboles

    uint8_t Li[16];  
    for (uint16_t i = 0; i < 16; i++) {
        Li[i] = table[i];
    }

    Huff_arb *arbre = create_node();
    uint16_t code = 0;
    int k = 0;
    for (int i = 0; i < 16; i++) {
        int len = i + 1;
        for (int j = 0; j < Li[i]; j++) {
            insert_code(arbre, code, symbols[k], len);
            code++;
            k++;
        }
        code <<= 1;
    }

    printf("Arbre Huffman construit avec succès.\n");
    return 0;
}
