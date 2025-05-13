#include <stdio.h>
#include <string.h>
#include <stdint.h>


uint32_t** f(uint8_t *Y, uint8_t *Cb, uint8_t *Cr,uint8_t hauteur,uint8_t largeur){
    uint32_t **pix = malloc(largeur*sizeof(uint32_t*));
    for (int i =0; i < largeur; i++){
        pix[i]= malloc(hauteur*sizeof(uint32_t));
    }
    for (uint16_t i  = 0; i <= sizeof(Y)/8; i++){ 
        for (uint16_t j  = 0; j <= sizeof(Y[0]); j++){   
            uint8_t R = Y[i*8+j] -0.0009267 * (Cb[i*8+j]-128)+1.4016868*(Cr[i*8+j]-128);
            uint8_t G = Y[i*8+j] -0.3436954 * (Cb[i*8+j]-128)-0.7141690*(Cr[i*8+j]-128);
            uint8_t B = Y[i*8+j] +1.7721604 * (Cb[i*8+j]-128)+0.0009902*(Cr[i*8+j]-128);
            pix[i][j] = (R << 16) | (G << 8) | B;
        }
    }
    return pix;
};