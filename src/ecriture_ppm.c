#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void transf_ppm (uint8_t** t, const char* nom, uint16_t nb_mcux, uint16_t nb_mcuy){
    FILE *fichier = fopen(nom, "wb");
    uint16_t largeur = nb_mcux * 8;
    uint16_t hauteur = nb_mcuy * 8;
    fprintf(fichier, "P6\n");
    fprintf(fichier, "%d %d\n", largeur, hauteur);
    fprintf(fichier, "255\n");
    uint8_t b, g, r;
    for (uint16_t mcu_y = 0; mcu_y < nb_mcuy; mcu_y++) {
        for (uint8_t i = 0; i < 8; i++) {
            for (uint16_t mcu_x = 0; mcu_x < nb_mcux; mcu_x++) {
                uint16_t k = mcu_y * nb_mcux + mcu_x; 
                for (uint8_t j = 0; j < 8; j++) {   
                    b = (t[k][i*8+j])  & 0xFF;
                    g = (t[k][i*8+j] >>8) & 0xFF;
                    r = (t[k][i*8+j] >>16) & 0xFF;
                    fwrite(&r, sizeof(uint8_t), 1, fichier);
                    fwrite(&g, sizeof(uint8_t), 1, fichier);
                    fwrite(&b, sizeof(uint8_t), 1, fichier);
               }
             } 
        }
    }
    fclose(fichier);
}


void transf_pgm(uint8_t** t, const char* nom, uint16_t nb_mcux, uint16_t nb_mcuy) {
    
    FILE *fichier = fopen(nom, "wb");
    uint16_t largeur = nb_mcux * 8;
    uint16_t hauteur = nb_mcuy * 8;
    fprintf(fichier, "P5\n");
    fprintf(fichier, "%d %d\n", largeur, hauteur);
    fprintf(fichier, "255\n");
    uint8_t b;
    for (uint16_t mcu_y = 0; mcu_y < nb_mcuy; mcu_y++) {
        for (uint8_t i = 0; i < 8; i++) {
            for (uint16_t mcu_x = 0; mcu_x < nb_mcux; mcu_x++) {
                uint16_t k = mcu_y * nb_mcux + mcu_x; 
                for (uint8_t j = 0; j < 8; j++) {   
                    if (t[k][i * 8 + j] > 255){
                        b = 255;
                    }
                    else {
                        b= t[k][i * 8 + j];
                    }
                    fwrite(&b, sizeof(uint8_t), 1, fichier);
                }
            }
        }
    }
    fclose(fichier);
}
