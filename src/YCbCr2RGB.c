#include <stdio.h>
#include <string.h>
#include <stdint.h>

uint32_t f(Y, Cb, Cr){
    uint8_t R = Y -0.0009267*(Cb-128)+1.4016868*(Cr-128);
    uint8_t G = Y -0.3436954*(Cb-128)-0.7141690*(Cr-128);
    uint8_t B = Y +1.7721604*(Cb-128)+0.0009902*(Cr-128);
    uint32_t pix = (R << 16) | (G << 8) | B;
    return pix;
};