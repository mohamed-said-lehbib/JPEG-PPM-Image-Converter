#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "structs.h"

Mcu* duplique_horiz(Mcu *b, uint8_t h, uint8_t v){
    Mcu *b1 = malloc(sizeof(Mcu));;
    b1->mcu = malloc(v * sizeof(uint16_t***));
    for (uint8_t i = 0; i < v; i++) {
        b1->mcu[i] = malloc(h * sizeof(uint16_t**));
        for (uint8_t j = 0; j < h; j++) {
            b1->mcu[i][j] = malloc(8 * sizeof(uint16_t*));
            for (uint8_t k = 0; k < 8; k++) {
                b1->mcu[i][j][k] = malloc(8 * sizeof(uint16_t));
            }
        }
    }
    for (uint8_t i=0;i< v; i++){
        uint8_t p=0;
        for (uint8_t j =0; j < h;j++){
            uint16_t** block = b1->mcu[i][j];
            uint16_t** blk = b->mcu[i][j];
            for (uint8_t l=0;l< 8; l++){
                uint8_t k =0;
                for (uint8_t m =0; m < 4;m++){
                    if(p%2==0){
                        block[l][k] = blk[l][m];
                        block[l][k+1] = blk[l][m];
                        k=2*m;
                    }
                    else{
                        block[l][k] = blk[l][4+m];
                        block[l][k+1] = blk[l][m+4];
                        k=2*m;
                    }
                }
            }
            p +=1;
        }       
    }
    return b1;
}




YCbCrstr sur_ech_hori(YCbCrstr YCbCr,infos_comp **infos_img){
    uint8_t h = infos_img[0]->h_i;
    uint8_t v = infos_img[0]->v_i; 

    YCbCrstr YCbCr1;
    YCbCr1.Cb1 = malloc(v * sizeof(uint16_t**));
    YCbCr1.Cr1 = malloc(v * sizeof(uint16_t**));
}