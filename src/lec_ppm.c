#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void transf_ppm(uint32_t tab[8][8], const char* nom){
    

    FILE *fichier = fopen(nom, "w");
    fprintf(fichier, "P6\n");
    fprintf(fichier, "8 8\n");
    fprintf(fichier, "255\n");
    
    for (uint32_t i =0; i < 8; i++){
        for (uint32_t j =0; j < 8; j++){
            uint8_t  b = (tab[i][j]);
            uint8_t  g = (tab[i][j] >>8);
            uint8_t  r = (tab[i][j] >>16);
            fwrite(&r, sizeof(uint8_t), 1, fichier);
            fwrite(&g, sizeof(uint8_t), 1, fichier);
            fwrite(&b, sizeof(uint8_t), 1, fichier);
        } 
        fprintf(fichier,"\n");
    }

    fclose(fichier);
}





void transf_pgm(uint32_t tab[8][8], const char* nom){
    

    FILE *fichier = fopen(nom, "w");
    fprintf(fichier, "P6\n");
    fprintf(fichier, "8 8\n");
    fprintf(fichier, "255\n");
    
    for (uint32_t i =0; i < 8; i++){
        for (uint32_t j =0; j < 8; j++){
            uint8_t  b = (tab[i][j]);
            fwrite(&b, sizeof(uint8_t), 1, fichier);
        } 
        fprintf(fichier,"\n");
    }

    fclose(fichier);
}