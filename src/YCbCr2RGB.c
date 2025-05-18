#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "../include/structs.h"
uint32_t** YCbCr2RGB(umatrice *bloc,uint8_t h,uint8_t v){
    uint8_t **Y = bloc[0].data;
    uint8_t **Cb = bloc[1].data;
    uint8_t **Cr = bloc[2].data;
    uint32_t **pix= malloc(8*v*sizeof(uint32_t*));
    for (uint8_t i =0; i < v*8; i++){
        pix[i]= malloc(8*h*sizeof(uint32_t));
    }
    for (uint8_t i  = 0; i < v*8; i++){ 
        for (uint8_t j  = 0; j < h*8; j++){   
            float y = Y[i][j];
            float cb = Cb[i][j] - 128;
            float cr = Cr[i][j] - 128;
            float r = y -0.0009267 * (cb)+1.4016868*(cr);
            float g = y -0.3436954 * (cb)-0.7141690*(cr);
            float  b = y +1.7721604 * (cb)+0.0009902*(cr);
            uint8_t R= r<0?0 :(r>255?255:r);
            uint8_t G= g<0?0 :(g>255?255:g);
            uint8_t B= b<0?0 :(b>255?255:b);
            pix[i][j] = (R << 16) | (G << 8) | B;
        }
    }
    return pix;
};