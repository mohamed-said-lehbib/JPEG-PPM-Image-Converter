#ifndef LEC_PPM_H
#define LEC_PPM_H

#include <stdint.h>

void transf_ppm(uint8_t tab[8][8], const char* nom);
void transf_pgm(uint8_t **t, const char* nom,uint16_t nb_mcux,uint16_t nb_mcuy);
#endif 