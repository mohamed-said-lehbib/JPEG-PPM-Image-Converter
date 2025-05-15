#ifndef LEC_PPM_H
#define LEC_PPM_H

#include <stdint.h>
void transf_ppm (uint32_t*** t, const char* nom, uint16_t largeur, uint16_t hauteur,infos_comp* infos_img);
void transf_pgm(uint8_t ****t, const char* nom,uint16_t largeur,uint16_t hauteur);
void transf_pgm_col(uint8_t ***t, const char* nom,uint16_t largeur,uint16_t hauteur);
#endif 