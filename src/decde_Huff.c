#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "decde_Huff.h"



// creation d'un noeud
Huff_arb *create_node() {
    Huff_arb* noeud = malloc(sizeof(Huff_arb)); 
    noeud->est_mot_de_code = 0;// valant 1 s'il s'agit d'un mot de code 
    noeud->symbole = 0;// le symbole stocke dans le feuille et code par un mot de code qui est represente par le chemin de la racine vers la feuille
    noeud->fg = NULL;// fils gauche
    noeud->fd = NULL;// fils droit
    return noeud;
}
// decode_bloc : elle prend en parametre l'arbre de decodage dc et ac, le flux de bits et la taille du bloc
// elle parcourt le flux de bits et decode les coefficients DC et AC , des qu'elle remplis un un vecteur de 64 coefficients, elle l'ajoute dans le tableau de blocs
// et elle passe au vecteur suivant
 MCU *decode_bloc(Huff_arb **arbre_dc, Huff_arb **arbre_ac, BitStream *bs,uint16_t nb_mcux,uint16_t nb_mcuy, uint8_t N_comp,
    uint8_t *huff_corr_dc,uint8_t *huff_corr_ac , uint16_t nb[]) {
    MCU *mcus = malloc(nb_mcux*nb_mcuy* sizeof(MCU));//allocation de la memoire pour le tableau de blocs
    
    for (int i = 0; i < nb_mcux*nb_mcuy; i++) {
        mcus[i].nb_comp = N_comp;
        mcus[i].comps = malloc(N_comp* sizeof(Comp));
        for (int j = 0 ; j< N_comp ; j++){
            mcus[i].comps[j].blocs=malloc(nb[j] * sizeof(Bloc));
            mcus[i].comps[j].nb_bloc = nb[j];  
        }
    }
    uint16_t blo_idx = 0;
    while (blo_idx < nb_mcux*nb_mcuy ) {// tant qu'on a pas consomme le flux de bits*
        //printf( " nouveau bloc %d", blo_idx);

        for ( int comp = 0 ; comp < N_comp ; comp++){  
            uint8_t idx_table_dc=huff_corr_dc[(comp==0)?0:1];
            uint8_t idx_table_ac=huff_corr_ac[(comp==0)?0:1];
            for ( int k=0 ; k<mcus[blo_idx].comps[comp].nb_bloc;k++){
                if (blo_idx == 0){
                    mcus[blo_idx].comps[comp].blocs[k].data[0]= decode_dc(arbre_dc[idx_table_dc], 0, bs);// le premier bloc n'a pas de valeur dc initiale
                }
                else {
                    mcus[blo_idx].comps[comp].blocs[k].data[0]= decode_dc(arbre_dc[idx_table_dc], mcus[blo_idx-1].comps[comp].blocs[k].data[0], bs);
                }
                int *coeffs_ac = decode_all_ac(arbre_ac[idx_table_ac], bs);// decodage des coefficients ac ( return 63 coefficients)
                for (int j = 1; j < 64; j++) {
                    mcus[blo_idx].comps[comp].blocs[k].data[j] = coeffs_ac[j-1];
                }
                free(coeffs_ac);
                
                    }}
        
        blo_idx++;
        }
       
       
    

    
    return mcus;// renvoie le tableau de blocs

    }


// insert_code : elle prend en parametre l'arbre de decodage, le code, le symbole et la longueur du code
// elle insere le code dans l'arbre de decodage en suivant le chemin de la racine vers la feuille
// elle cree un noeud pour chaque bit du code et elle stocke le symbole dans la feuille
void insert_code(Huff_arb *arbre, uint16_t code, uint8_t symbole, uint16_t longueur) {
    Huff_arb *courrent = arbre; 
    for (int i = longueur - 1; i >= 0; i--) {
        uint8_t bit = (code >> i) & 1;// lecture de code bit par bit
        if (bit == 0) {
            if (!courrent->fg) courrent->fg = create_node();
            courrent = courrent->fg;
        } else {
            if (!courrent->fd) courrent->fd = create_node();
            courrent = courrent->fd;
        }
    }
  
    courrent->est_mot_de_code = 1;// on a atteint une feuille , donc on passe a 1 
    courrent->symbole = symbole;// on stocke le symbole dans la feuille
}
// afficher_arbre : elle prend en parametre l'arbre de decodage
// elle affiche l'arbre de decodage d'une maniere recursive
// elle nous etait utile pour deboguer l'arbre de decodage
void afficher_arbre(Huff_arb *arbre) {
    if (arbre == NULL) return;// si l'arbre est vide, on ne fait rien

    if (arbre->est_mot_de_code) {
        printf("mot de code: 0x%x\n", arbre->symbole);
    } else {
        printf("noeud vide \n");
    }

    afficher_arbre(arbre->fg); // afficher le fils gauche
    afficher_arbre(arbre->fd); // afficher le fils droits
}

// decode_dc : elle prend en parametre l'arbre de decodage, la valeur dc initiale et le flux de bits
// elle decode la valeur dc en parcourant l'arbre de decodage et en lisant les bits du flux de bits
int decode_dc(Huff_arb *arbre, int dc_init, BitStream *bs) {
    if (arbre == NULL || bs == NULL) {
        fprintf(stderr, " l'arbre ou le flux debit est nulle \n");
        exit(EXIT_FAILURE);
    }
    Huff_arb *courant = arbre;
    while (courant->est_mot_de_code == 0) {//tant qu'on n'est pas sur un mot de code
        int bit = read_bits(bs,1);// on lit le flux bit par bit
        if (bit == 0) {
            if (courant->fg == NULL) {
                fprintf(stderr, "noeud gauche NULL dans decode_dc\n");
                exit(EXIT_FAILURE);
            }
            courant = courant->fg;
        } else {
            if (courant->fd == NULL) {
                fprintf(stderr, "noeud droit NULL dans decode_dc\n");
                exit(EXIT_FAILURE);
            }
            courant = courant->fd;
        }
    }
    int m = courant->symbole;// le symbole recupere est la magnitude
    int indice = read_bits(bs, m);//on lit m fois bits suivant repransentant l'indice du coefficient dc dans la classe de magnitude
    int DC = 0;
    if (m == 0) {
        DC = dc_init;// si m = 0, on ne change pas la valeur dc
    }
    else if ((indice >> (m - 1)) & 1) {
        DC = indice + dc_init;// si le premier bit est 1, alors l'element cherche est l'indice lui meme puisque on est sur la deusieme moitie de la classe de magnitude 
    }//on prend tjrs en compte le dc initiale
    else {
        DC = indice - (1 << m) + 1;// sinon , l'element cherche se trouve a une distance de indice + 1 de -2^(m-1) 
        DC = DC + dc_init;
    }
    return DC;
}

// decode_all_ac : elle prend en parametre l'arbre de decodage et le flux de bits
// elle decode les coefficients ac en parcourant l'arbre de decodage et en lisant les bits du flux de bits
// elle renvoie un tableau de 63 coefficients ac
int* decode_all_ac(Huff_arb *arbre, BitStream *bs) {
    int *coef_ac = malloc(63 * sizeof(int));

    int k = 0;
    while (k < 63) {
        Huff_arb *courant = arbre;
        while (!courant->est_mot_de_code) {
            int bit = read_bits(bs,1);// on lit le flux bit par bit
            courant = (bit == 0) ? courant->fg : courant->fd;// on avance dans l'arbre de decodage
        }

        int symbol = courant->symbole;// on tombe sur le symbole qui contien la magnitude et le rle
        int rle = symbol >> 4;  // le rle est stocke dans les 4 premiers bits( representant le nombre de zeros a ajouter avant le coefficient non nul suivant)
        int m = symbol & 0x0F;  // la magnitude

        //Des qu'on trouve 0 c'est fini , tout les coefficients restants sont nuls, car si non , il  devrait etre cache dans le rle du prochaine coef non nul 
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

        // remplir rle fois 0 avant le ac 
        for (int i = 0; i < rle; i++) coef_ac[k++] = 0;
        coef_ac[k++] = AC;
    }

    return coef_ac;
}

    
//libere la memoire allouee pour l'arbre de decodage
void free_arbre(Huff_arb *arbre) {
    if (!arbre) return;
    free_arbre(arbre->fg);
    free_arbre(arbre->fd);
    free(arbre);
}

/*
int main(void) {
    // 1. Table fictive Huffman DC et AC
    uint8_t table_dc[16] = {   0x01, 0x01, 0x00,
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00 }; 
    uint8_t symbols_dc[2] = { 0x07, 0x00 };

    uint8_t table_ac[16] = { 0x00 ,0x02, 0x02, 0x01, 0x04,
        0x02, 0x03, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00}; 
    uint8_t symbols_ac[15] = { 0x01, 0x03, 0x02, 0x04,
        0x05, 0x06, 0x11, 0x12,
        0x21, 0x07, 0x13, 0x00,
        0x14, 0x31, 0x33
    };

    //Création des arbres Huffman
    Huff_arb *arbre_dc = create_node();
    Huff_arb *arbre_ac = create_node();
// remplir l'arbre dc
    uint16_t code = 0;
    int k = 0;
    for (int i = 0; i < 16; i++) {
        int len = i + 1;
        for (int j = 0; j < table_dc[i]; j++) {
            insert_code(arbre_dc, code, symbols_dc[k], len);
            code++;
            k++;
        }
        code <<= 1;
    }
    printf( "l'arbre DC est rempli\n");
// remplir l'arbre AC
    code = 0; k = 0;
    for (int i = 0; i < 16; i++) {
        int len = i + 1;
        for (int j = 0; j < table_ac[i]; j++) {
            insert_code(arbre_ac, code, symbols_ac[k], len);
            code++;
            k++;
        }
        code <<= 1;
    }
    
    afficher_arbre(arbre_dc);
    printf( "l'arbre AC est rempli\n");
    afficher_arbre(arbre_ac);
    // 3. Données simulées (DC + AC de plusieurs blocs consécutifs)
    uint8_t data[] = {
        0x47, 0xF0, 0x7B, 0x31, 0x16, 0xF4, 0xD0, 0xC1, 0xD7, 0x7B,
        0x31, 0x56, 0x68, 0x29, 0xA3, 0x35, 0x5D, 0x22, 0x0A, 0x75,
        0x87, 0xCF, 0xD6, 0xC4, 0x3B, 0xDB, 0xCA, 0x73, 0x87, 0x14,
        0x15, 0x9D, 0x87, 0x0F, 0xEB, 0xD8, 0x00, 0xEC, 0x6F, 0x39,
        0x54, 0xF1, 0xCE, 0x63, 0x0F, 0x7F, 0x30, 0x9C, 0x72, 0x9D,
        0xA9, 0x23, 0xF5, 0x16, 0xBB, 0x54, 0x2C, 0xC0, 0x03, 0xED,
        0x69, 0x8A, 0xC3, 0x17, 0xCB, 0x89, 0xDA, 0x03, 0xB3, 0xFA,
        0x7A, 0xDB, 0xAE, 0x81, 0x6F, 0x8D, 0x75, 0xB5, 0x4C, 0x1A,
        0xAC, 0x51, 0xAF, 0xA3, 0xF4, 0xFB, 0xB2, 0x12, 0x62, 0xE2,
        0xEB, 0x97, 0x9C, 0x9A, 0xA5, 0xB4, 0x66, 0x25, 0xEE, 0x32,
        0x63, 0x3F, 0x63, 0x12, 0x22, 0x0C, 0x47, 0x7B, 0x48, 0x1D,
        0x8F, 0x67, 0xE1, 0x65, 0x8D, 0x67, 0x67, 0x50, 0xEB, 0xFC,
        0xF5, 0x0D, 0x27, 0x7E, 0x85, 0xDD, 0x21, 0x46, 0xCD, 0x68,
        0x54, 0xBC, 0xAA, 0x8A, 0x4C, 0x2E, 0x49, 0x35, 0xE5, 0x06,
        0xB0, 0x45, 0x50, 0x84, 0x0C, 0xE6, 0xD9, 0x96, 0x16, 0x44,
        0x46, 0x3B, 0xAA, 0x3C, 0x47, 0x02, 0x07, 0xCF
    };

    BitStream bs;
    create_bitstream(&bs, data, sizeof(data));


    int **blocs = decode_bloc(arbre_dc, arbre_ac, &bs);
    printf("je suis pas encore la ");

    //  affichage
    for (int b = 0; b < 4; b++) {
        if (blocs[b] == NULL) {
            printf("eera3ini");
            break;}
        printf("Bloc %d:\n", b);
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                printf("%4x ", blocs[b][i * 8 + j]);
            }
            printf("\n");
        }
        printf("\n");
    
    }
        

    // Libération mémoire
    free(blocs);
    free_arbre(arbre_dc);
    free_arbre(arbre_ac);

    return 0;
}
*/
/*
int main(void) {
    // Table Huffman AC (table & symboles fictifs ici, adapte selon ton vrai tableau JPEG)
    uint8_t table[16] = {
        0x00, 0x02, 0x01, 0x02,
        0x07, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    
    uint8_t symbols[12] = {
        0x17, 0x18,           // longueur 2
        0x15,                 // longueur 3
        0x08, 0x19,           // longueur 4
        0x00, 0x09, 0x13, 0x23, 0x28, 0x29, 0x37  // longueur 5
    };

    // Construire l'arbre Huffman
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

    // Données JPEG AC simulées
    uint8_t data[] = {
        0xD1, 0xCA, 0xCA, 0xDC, 0x76, 0xDA, 0x4D,
        0x6A, 0x00, 0x15, 0xED, 0x41, 0xF1, 0x2D, 0x3A,
        0xDC, 0x70, 0x8B, 0x16, 0xBE, 0x4C, 0xC9, 0xBB,
        0xB3, 0x4F, 0xFB, 0x35, 0xB8, 0x7D, 0x13, 0xAB,
        0x12, 0x9D, 0x0F, 0x0E, 0x1F, 0x4E, 0x1D, 0xE3
    };
    size_t data_len = sizeof(data);

    BitStream bs;
    create_bitstream(&bs, data, data_len);

    // Décoder les coefficients AC
    int *coeffs = decode_all_ac(arbre, &bs);

    // Affichage
    printf("Coefficients AC décodés:\n");
    for (int i = 0; i < 63; i++) {
        printf("%x ", coeffs[i]);
    }
    printf("\n");

    // Libération mémoire
    free(coeffs);
    free_arbre(arbre);

    return 0;
}
*/
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
