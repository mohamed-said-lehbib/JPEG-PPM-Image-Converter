#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void transf_ppm(uint32_t tab[8][8], const char* nom){
    

    FILE *fichier = fopen(nom, "wb");
    fprintf(fichier, "P6\n");
    fprintf(fichier, "8 8\n");
    fprintf(fichier, "255\n");
    uint8_t  b, g, r;
    for (uint32_t i =0; i < 8; i++){
        for (uint32_t j =0; j < 8; j++){
            b = (tab[i][j])  & 0xFF;
            g = (tab[i][j] >>8) & 0xFF;
            r = (tab[i][j] >>16) & 0xFF;
            fwrite(&r, sizeof(uint8_t), 1, fichier);
            fwrite(&g, sizeof(uint8_t), 1, fichier);
            fwrite(&b, sizeof(uint8_t), 1, fichier);
        }
    }
    fclose(fichier);
}


void transf_pgm(uint32_t tab[8][8], const char* nom){
    

    FILE *fichier = fopen(nom, "wb");
    fprintf(fichier, "P5\n");
    fprintf(fichier, "8 8\n");
    fprintf(fichier, "255\n");
    uint8_t b;
    for (uint32_t i =0; i < 8; i++){
        for (uint32_t j =0; j < 8; j++){
            if (tab[i][j] > 255){
                b = 255;
            }
            else
                b = (tab[i][j]);
            fwrite(&b, sizeof(uint8_t), 1, fichier);
        }
    }

    fclose(fichier);
}

// teste //


// int main() {
//     uint32_t image[8][8];

//     for (int i = 0; i < 8; i++) {
//         for (int j = 0; j < 8; j++) {
//             uint8_t r = i * 32;   
//             uint8_t g = j * 32;   
//             uint8_t b = 0;        
//             image[i][j] = (r << 16) | (g << 8) | b;
//         }
//     }

//     transf_ppm(image, "test.ppm");
//     printf("Fichier test.ppm généré.\n");
//     return 0;
// }
