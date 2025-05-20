#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "../include/structs.h"
#include "../include/decodage_huffman.h"

void quant_inverse(Bloc *bloc, quantification_table *qt) {
    uint8_t *quant_table = (uint8_t *)qt->data;
    for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                bloc->data[i*8+j] = (bloc->data[i*8+j] * quant_table[i * 8 + j]);
            }
            
        }
   
}