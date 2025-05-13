#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "structs.h"

extern uint16_t hauteur;
extern uint16_t largeur;

Mcu* sur_ech_horiz(Mcu* MCU, infos_comp **infos_img){
    
    // recuperation des donnees

    YCbCrstr* YCbCr = MCU->mcu;
    block8** Y_blocks = YCbCr->Y;
    block8** Cb_block = YCbCr->Cb1;
    block8** Cr_block = YCbCr->Cr1;
    uint8_t hy = infos_img[0]->h_i;
    uint8_t vy = infos_img[0]->v_i;
    uint8_t hcb = infos_img[1]->h_i;
    uint8_t vcb = infos_img[1]->v_i;
    uint8_t hcr = infos_img[2]->h_i;
    uint8_t vcr = infos_img[2]->v_i; 

    //-----------------------------//

    block8** new_Cb_block = malloc(vy*sizeof(block8*));
    block8** new_Cr_block = malloc(vy*sizeof(block8*));
    for (uint8_t i = 0; i < vy; i++) {
        new_Cb_block[i] = malloc(hy * sizeof(block8));
        new_Cr_block[i] = malloc(hy * sizeof(block8));
        for (uint8_t j = 0; j < hy; j++) {
            for (uint8_t k = 0; k < 8; k++) {
                
            }
        }
    }

    return MCU
}