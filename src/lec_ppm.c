#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// void transf_ppm (uint8_t ** tab, const char* nom){
//     FILE *fichier = fopen(nom, "wb");
//     fprintf(fichier, "P6\n");
//     fprintf(fichier, "8 8\n");
//     fprintf(fichier, "255\n");
//     uint8_t taille = 2;
//     for ( int i=0; i< taille;i++){
//         uint8_t  b, g, r;
//         for (uint32_t i =0; i < 8; i++){
//             for (uint32_t j =0; j < 8; j++){
//                 b = (tab[i][j])  & 0xFF;
//                 g = (tab[i][j] >>8) & 0xFF;
//                 r = (tab[i][j] >>16) & 0xFF;
//                 fwrite(&r, sizeof(uint8_t), 1, fichier);
//                 fwrite(&g, sizeof(uint8_t), 1, fichier);
//                 fwrite(&b, sizeof(uint8_t), 1, fichier);
//             }
//         } 
//         for (uint32_t i =0; i < 8; i++){
//             for (uint32_t j =0; j < 8; j++){
//                 b = (tab[i][j])  & 0xFF;
//                 g = (tab[i][j] >>8) & 0xFF;
//                 r = (tab[i][j] >>16) & 0xFF;
//                 fwrite(&r, sizeof(uint8_t), 1, fichier);
//                 fwrite(&g, sizeof(uint8_t), 1, fichier);
//                 fwrite(&b, sizeof(uint8_t), 1, fichier);
//             }
//         } 
//     }
//     fclose(fichier);
// }


void transf_pgm(uint8_t** t, const char* nom, uint16_t largeur, uint16_t hauteur) {
    
    FILE *fichier = fopen(nom, "wb");
    if (!fichier) {
        perror("Erreur d'ouverture du fichier");
        return;
    }

  
    uint16_t nb_mcux = (largeur+7)/8;
    uint16_t nb_mcuy = (hauteur+7)/8;

    
    fprintf(fichier, "P5\n");
    fprintf(fichier, "%d %d\n", largeur, hauteur);
    fprintf(fichier, "255\n");

    uint8_t b;

    
    for (uint16_t mcu_y = 0; mcu_y < nb_mcuy; mcu_y++) {
        for (uint8_t i = 0; i < 8; i++) {
            if( mcu_y*8 +i >= hauteur){break;}
            for (uint16_t mcu_x = 0; mcu_x < nb_mcux; mcu_x++) {
                
                uint16_t k = mcu_y * nb_mcux + mcu_x; 
                
                for (uint8_t j = 0; j < 8; j++) {
                    if( mcu_x*8 +j >= largeur){break;}
                    b = (t[k][i * 8 + j] > 255) ? 255 : t[k][i * 8 + j];
                    fwrite(&b, sizeof(uint8_t), 1, fichier);
                }
            }
        }
    }

    fclose(fichier);
}
