#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "structs.h"

extern uint16_t hauteur;
extern uint16_t largeur;

umatrice* sur_ech_horiz(umatrice*com, infos_comp **infos_img);
umatrice* sur_ech_ver(umatrice* com, infos_comp **infos_img);
umatrice* sur_ech(umatrice* com, infos_comp **infos_img);