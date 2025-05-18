#ifndef IDCT_H
#define IDCT_H
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

float C(uint16_t eps);//calcule de la fonction C 

uint8_t **iDCT(int16_t **matrice);//nécessite une converion si 8 bits

uint8_t **iDCT_rap(int16_t **matrice);

#endif