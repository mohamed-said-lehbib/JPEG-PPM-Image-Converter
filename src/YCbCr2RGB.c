#include <stdio.h>
#include <string.h>
#include <stdint.h>


uint32_t f(uint32_t Y[8][8], uint32_t Cb[8][8], uint32_t Cr[8][8]){
    uint32_t pix [8][8];
    for (uint16_t i  = 0; i <= sizeof(Y[0]); i++){ 
        for (uint16_t j  = 0; j <= sizeof(Y[0]); j++){   
            uint8_t R = Y[i][j] -0.0009267*(Cb[i][j]-128)+1.4016868*(Cr[i][j]-128);
            uint8_t G = Y[i][j] -0.3436954*(Cb[i][j]-128)-0.7141690*(Cr[i][j]-128);
            uint8_t B = Y[i][j] +1.7721604*(Cb[i][j]-128)+0.0009902*(Cr[i][j]-128);
            pix[i][j] = (R << 16) | (G << 8) | B;
        }
    }
    return pix;
};