#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../include/structs.h"


void transf_ppm(uint32_t** t,const char* nom, uint16_t largeur, uint16_t hauteur){
    FILE *fichier = fopen(nom, "wb");
    fprintf(fichier, "P6\n");
    fprintf(fichier, "%d %d\n", largeur, hauteur);
    fprintf(fichier, "255\n");
    for (uint32_t i =0;i<hauteur;i++){
        for (uint32_t j = 0; j < largeur; j++) {
            uint32_t pix = t[i][j];
            uint8_t rouge = (pix >>16)& 0xff;
            uint8_t vert = (pix >>8)& 0xff;
            uint8_t bleu = pix & 0xff;
            fwrite(&rouge, sizeof(uint8_t), 1, fichier);
            fwrite(&vert, sizeof(uint8_t), 1, fichier);
            fwrite(&bleu, sizeof(uint8_t), 1, fichier);
        }
    }
    fclose(fichier);
}






void transf_pgm(umatrice*** t, const char* nom, uint16_t largeur, uint16_t hauteur) {
       uint16_t nb_mcux = (largeur+7)/8;
    uint16_t nb_mcuy = (hauteur+7)/8;

    FILE *fichier = fopen(nom, "wb");
 
    fprintf(fichier, "P5\n");
    fprintf(fichier, "%d %d\n", largeur, hauteur);
    fprintf(fichier, "255\n");
    uint8_t b;
    for (uint16_t mcu_y = 0; mcu_y < nb_mcuy; mcu_y++) {
        for (uint8_t i = 0; i < 8; i++) {
            if( mcu_y*8 +i >= hauteur){break;}
            for (uint16_t mcu_x = 0; mcu_x < nb_mcux; mcu_x++) {
                for (uint8_t j = 0; j < 8; j++) {  
                    if( mcu_x*8 +j >= largeur){break;}
                    b= t[mcu_y][mcu_x]->data[i][j];
                    fwrite(&b, sizeof(uint8_t), 1, fichier);
                }
            }
        }
    }
    fclose(fichier);
}

