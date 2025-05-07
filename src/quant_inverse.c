#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <quantification.h> 
void quant_inverse(uint8_t input[8][8], quantification_table *qt) {
    if (qt->prec == 0) {
        uint8_t *quant_table = (uint8_t *)qt->data;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                input[i][j] = (input[i][j] * quant_table[i * 8 + j]);
            }
            
        }
    } else {
        uint16_t *quant_table = (uint16_t *)qt->data;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                input[i][j] = (input[i][j] * quant_table[i * 16 + j]);
            }
            
        }
    }
}