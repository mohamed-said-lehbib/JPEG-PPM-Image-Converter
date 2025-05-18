#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "decde_Huff.h"



// creation d'un noeud
Huff_arb *creer_noeud() {
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
 MCU *decode_huff_image(Huff_arb **arbre_dc, Huff_arb **arbre_ac, BitStream *bs,uint16_t nb_mcux,uint16_t nb_mcuy, uint8_t N_comp,
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
    uint32_t blo_idx = 0;
    int16_t temp[3] = {0,0,0};
    while (blo_idx < nb_mcux*nb_mcuy ) {// tant qu'on a pas consomme le flux de bits*
        //printf( " nouveau bloc %d", blo_idx);

        for ( int comp = 0 ; comp < N_comp ; comp++){  
            uint8_t idx_table_dc=huff_corr_dc[(comp==0)?0:1];
            uint8_t idx_table_ac=huff_corr_ac[(comp==0)?0:1];
            for ( int k=0 ; k<mcus[blo_idx].comps[comp].nb_bloc;k++){
                mcus[blo_idx].comps[comp].blocs[k].data[0]= decode_dc(arbre_dc[idx_table_dc], temp[comp], bs);
                temp[comp]=mcus[blo_idx].comps[comp].blocs[k].data[0];

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
void inserer_code(Huff_arb *arbre, uint16_t code, uint8_t symbole, uint16_t longueur) {
    Huff_arb *courrent = arbre; 
    for (int i = longueur - 1; i >= 0; i--) {
        uint8_t bit = (code >> i) & 1;// lecture de code bit par bit
        if (bit == 0) {
            if (!courrent->fg) courrent->fg = creer_noeud();
            courrent = courrent->fg;
        } else {
            if (!courrent->fd) courrent->fd = creer_noeud();
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
