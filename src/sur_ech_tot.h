#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "structs.h"

extern uint16_t hauteur;
extern uint16_t largeur;

uint16_t ***sur_ech_horiz(uint16_t ***com, infos_comp **infos_img);
uint16_t ***sur_ech_ver(uint16_t ***com, infos_comp **infos_img);
uint16_t ***sur_ech(uint16_t ***com, infos_comp **infos_img);