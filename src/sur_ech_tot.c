#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "structs.h"

extern uint16_t hauteur;
extern uint16_t largeur;

uint16_t ***sur_ech_horiz(uint16_t ***com, infos_comp **infos_img){
    
    //-----------recuperation des donnees------------------//

    uint16_t **y = com[0];           //  recuperation des composantes
    uint16_t **cb = com[1];
    uint16_t **cr = com[2];

    uint8_t hy = 8*infos_img[0]->h_i; // recuperation des dimensions des composantes
    uint8_t vy = 8*infos_img[0]->v_i;         
    uint8_t hcb = 8*infos_img[1]->h_i;
    uint8_t vcb = 8*infos_img[1]->v_i;    
    uint8_t hcr = 8*infos_img[2]->h_i;
    uint8_t vcr = 8*infos_img[2]->v_i; 

    uint8_t facteur_hcb = hy/hcb;   // les facteurs d'echantillonnage
    uint8_t facteur_hcr = hy/hcr;  

    //--------------Allocation de l'espace ---------------//

    uint16_t** new_cb_block = malloc(vcb*sizeof(uint16_t*));
    uint16_t** new_cr_block = malloc(vcr*sizeof(uint16_t*));
    for (uint8_t i = 0; i < vcb; i++) {
        new_cb_block[i] = malloc(hy * sizeof(uint16_t));
        new_cr_block[i] = malloc(hy * sizeof(uint16_t));
    }

    //--------------remplissage de la mcu---------------//

                //----Pour Cb------//

    for (int i =0;i< vy;i++){
        uint16_t decale=0;
        for (int j = 0 ; j<hcb; j++){
            decale = facteur_hcb*j;       // pour commencer là oû on est arreter l'iteration precedente
            for (int k=0 ; k < facteur_hcb; k++){
                new_cb_block[i][k+decale] = cb[i][j];
            }
        }
    }
                 //----Pour Cr------//
    for (int i =0;i< vy;i++){
        uint16_t decale=0;
        for (int j = 0 ; j<hcr; j++){
            /*uint16_t valeur = cr[i][j];*/
            decale = facteur_hcb*j;       // pour commencer là oû on est arreter l'iteration precedente
            for (int k=0 ; k < facteur_hcb; k++){
                new_cr_block[i][k+decale] = cr[i][j];
            }
        }
    }
    com[1] = new_cb_block;
    com[2] = new_cr_block;
    //------------LiBERTEEEEEEEE----------//
    for (uint8_t i = 0; i < vcb; i++) {
        free(cb[i]);
    }
    free(cb);

    for (uint8_t i = 0; i < vcr; i++) {
        free(cr[i]);
    }
    free(cr);

    return com;
}





//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//





uint16_t ***sur_ech_ver(uint16_t ***com, infos_comp **infos_img){
    
    //-----------recuperation des donnees------------------//

    uint16_t **y = com[0];           //  recuperation des composantes
    uint16_t **cb = com[1];
    uint16_t **cr = com[2];

    uint8_t hy = infos_img[0]->h_i; // recuperation des dimensions des composantes
    uint8_t vy = infos_img[0]->v_i;         
    uint8_t hcb = infos_img[1]->h_i;
    uint8_t vcb = infos_img[1]->v_i;    
    uint8_t hcr = infos_img[2]->h_i;
    uint8_t vcr = infos_img[2]->v_i; 

    uint8_t facteur_vcb = vy/vcb;  // les facteurs d'echantillonnage
    uint8_t facteur_vcr = vy/vcr;   

    //--------------Allocation de l'espace ---------------//

    uint16_t** new_cb_block = malloc(vy*sizeof(uint16_t*));
    uint16_t** new_cr_block = malloc(vy*sizeof(uint16_t*));
    for (uint8_t i = 0; i < vy; i++) {
        new_cb_block[i] = malloc(hcb * sizeof(uint16_t));
        new_cr_block[i] = malloc(hcr * sizeof(uint16_t));
    }

    //--------------remplissage de la mcu---------------//


                //----Pour Cb------//



    for (int i =0;i< vy;i++){
        uint16_t decale=0;
        for (int j = 0 ; j<hcb; j++){
            decale = facteur_vcb*i;       // pour commencer là oû on est arreter l'iteration precedente
            for (int k=0 ; k < facteur_vcb; k++){
                new_cb_block[k+decale][j] = cb[i][j];
            }
        }
    }

                 //----Pour Cr------//



    for (int i =0;i< vcr;i++){
        uint16_t decale=0;
        for (int j = 0 ; j<hcr; j++){
            /*uint16_t valeur = cr[i][j];*/
            decale = facteur_vcr*i;       // pour commencer là oû on est arreter l'iteration precedente
            for (int k=0 ; k < facteur_vcr; k++){
                new_cr_block[k+decale][j] = cr[i][j];
            }
        }
    }

    com[1] = new_cb_block;
    com[2] = new_cr_block;

    //------------LiBERTEEEEEEEE----------//

    for (uint8_t i = 0; i < vcb; i++) {
        free(cb[i]);
    }
    free(cb);

    for (uint8_t i = 0; i < vcr; i++) {
        free(cr[i]);
    }
    free(cr);

    return com;
}



uint16_t ***sur_ech(uint16_t ***com, infos_comp **infos_img){

    uint8_t hy = infos_img[0]->h_i; // recuperation des dimensions des composantes
    uint8_t vy = infos_img[0]->v_i;         
    uint8_t hcb = infos_img[1]->h_i;
    uint8_t vcb = infos_img[1]->v_i;    
    uint8_t hcr = infos_img[2]->h_i;
    uint8_t vcr = infos_img[2]->v_i; 



    //--------Cas de sous echantollage verticale----------------//
    if ( hy==hcb && hy == hcr){
        return sur_ech_ver(com, infos_img);
    }

    //--------Cas de sous echantollage horizontalle-------------//
    else if ( vy==vcb && vy == vcr){
        return sur_ech_horiz(com, infos_img);
    }


    //--------Cas de sous echantollage horizontalle et verticale-------------//
    else{
        uint16_t*** new_com = sur_ech_horiz(com, infos_img);
        return sur_ech_ver(new_com,infos_img);
    }

}

// uint8_t Y[8][16] = {
//         {0x70, 0x74, 0x6d, 0x99, 0xc8, 0xb7, 0xbd, 0xc3, 0xbd, 0xbc, 0x7c, 0x74, 0x74, 0x74, 0x77, 0x72},
//         {0x73, 0x74, 0x8b, 0xc9, 0xb9, 0xd6, 0x8f, 0x83, 0xcc, 0xcb, 0xbf, 0x77, 0x74, 0x77, 0x6d, 0x76},
//         {0x6c, 0x9f, 0xcf, 0xc3, 0x63, 0x8f, 0xbc, 0x99, 0xc7, 0xd0, 0xc6, 0xce, 0xc7, 0x9c, 0x7b, 0x73},
//         {0x6f, 0x62, 0xc0, 0xab, 0x6b, 0xc3, 0xcb, 0xc9, 0xcc, 0x98, 0xd6, 0xd3, 0xd1, 0xd2, 0x90, 0x6f},
//         {0x72, 0x7c, 0xce, 0xb8, 0xbc, 0xc4, 0xc1, 0xc4, 0xc9, 0xc5, 0xc6, 0xb6, 0xb9, 0xc5, 0x82, 0x7e},
//         {0x73, 0x4e, 0x72, 0xa1, 0xbd, 0xc6, 0xc6, 0xc2, 0xc0, 0xcb, 0xc7, 0x8d, 0xc0, 0x8b, 0x4b, 0x79},
//         {0x71, 0x90, 0x69, 0x88, 0xbe, 0xc4, 0xc3, 0xc3, 0xc6, 0xc5, 0xc3, 0x9a, 0xb4, 0x9c, 0x66, 0x8a},
//         {0x89, 0xa2, 0x21, 0x91, 0x8d, 0xbe, 0xc6, 0xab, 0x94, 0xa2, 0x84, 0x6d, 0xa5, 0x88, 0x5b, 0x8e}
//     };
    
// uint8_t Cb[8][8] = {
//         {0x80, 0x78, 0x46, 0x45, 0x46, 0x88, 0x80, 0x82},
//         {0x83, 0x52, 0x3c, 0x55, 0x33, 0x6b, 0x84, 0x84},
//         {0x74, 0x3e, 0x59, 0x46, 0x45, 0x38, 0x54, 0x84},
//         {0x7d, 0x42, 0x5c, 0x48, 0x4d, 0x3a, 0x41, 0x7d},
//         {0x73, 0x41, 0x76, 0x6d, 0x76, 0x5d, 0x43, 0x75},
//         {0x7b, 0x66, 0x72, 0x75, 0x7b, 0x66, 0x51, 0x7f},
//         {0x75, 0x72, 0x77, 0x79, 0x7c, 0x6b, 0x52, 0x72},
//         {0x63, 0x7c, 0x78, 0x7b, 0x78, 0x76, 0x57, 0x73}
//     };
    
// uint8_t Cr[8][8] = {
//         {0xc8, 0xb4, 0xa2, 0x9b, 0xa2, 0xbd, 0xc3, 0xc7},
//         {0xbf, 0xac, 0x98, 0x97, 0x98, 0xb4, 0xbd, 0xc1},
//         {0xb4, 0xa0, 0x8f, 0x95, 0x99, 0x9e, 0xa7, 0xc4},
//         {0xb0, 0x99, 0x90, 0x97, 0x97, 0x98, 0x9d, 0xba},
//         {0xb2, 0x95, 0x98, 0x9c, 0x9d, 0x96, 0x96, 0xb3},
//         {0xb1, 0x93, 0x9f, 0x9f, 0xa4, 0x93, 0x8f, 0xaf},
//         {0xac, 0x91, 0x9d, 0x9e, 0x9a, 0x98, 0x94, 0xa2},
//         {0xa5, 0x8f, 0x98, 0x9b, 0x97, 0x92, 0x8f, 0xa2}
//     };
    
// // int main() {
//     // Allocation du tableau com
//     uint8_t ***com = malloc(3 * sizeof(uint8_t**));
//     for (int i = 0; i < 3; i++) {
//         int width = (i == 0) ? 16 : 8;
//         com[i] = malloc(8 * sizeof(uint8_t*));
//         for (int j = 0; j < 8; j++) {
//             com[i][j] = malloc(width * sizeof(uint8_t));
//         }
//     }


//     for (int i = 0; i < 8; i++) {
//         for (int j = 0; j < 16; j++) com[0][i][j] = Y[i][j];
//         for (int j = 0; j < 8; j++) {
//             com[1][i][j] = Cb[i][j];
//             com[2][i][j] = Cr[i][j];
//         }
//     }


//     infos_comp y_info  = {0, 16, 8, 0};
//     infos_comp cb_info = {1, 8, 8, 1};
//     infos_comp cr_info = {2, 8, 8, 1};
//     infos_comp *infos_img[3] = {&y_info, &cb_info, &cr_info};

    
//     com = sur_ech_horiz(com, infos_img);


//     printf("Cb après suréchantillonnage horizontal :\n");
//     for (int i = 0; i < 8; i++) {
//         for (int j = 0; j < 16; j++) {
//             printf("%02x ", com[1][i][j]);
//         }
//         printf("\n");
//     }

//     // Affichage Cr
//     printf("\nCr après suréchantillonnage horizontal :\n");
//     for (int i = 0; i < 8; i++) {
//         for (int j = 0; j < 16; j++) {
//             printf("%02x ", com[2][i][j]);
//         }
//         printf("\n");
//     }

//     return 0;
// }