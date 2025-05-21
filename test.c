#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "./include/izigzag.h"
#include "./include/idct.h"
#include "./include/decodage_huffman.h"

#include <string.h>
#include "./include/quant_inverse.h"
#include "./include/YCbCr2RGB.h"
#include "./include/ecriture_ppm.h"
#include "./include/sur_ech_tot.h"
#include "./include/get_header.h"

int main(){
    const int16_t expectee[8][8] = {
        {0, 1, 5, 6, 14, 15, 27, 28},
        {2, 4, 7, 13, 16, 26, 29, 42},
        {3, 8, 12, 17, 25, 30, 41, 43},
        {9, 11, 18, 24, 31, 40, 44, 53},
        {10, 19, 23, 32, 39, 45, 52, 54},
        {20, 22, 33, 38, 46, 51, 55, 60},
        {21, 34, 37, 47, 50, 56, 59, 61},
        {35, 36, 48, 49, 57, 58, 62, 63}
    };

    int16_t *vect_in = malloc(64*sizeof(int16_t));
    for (int i=0;i<64;i++){
        vect_in[i] = i;

    }
    int16_t** mat = zigzag_inv(vect_in);
   
    for (int i=0;i<64;i++){
        if ( mat[i/8][i%8] != expectee[i/8][i%8]){
            printf("valeur fausse");
            free(vect_in);
           
         
            return 1;
        };
       
    }
    printf("passé\n");
    return 0;
}