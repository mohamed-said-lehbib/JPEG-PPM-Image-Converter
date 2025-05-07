#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decde_Huff.h"

int decode_dc(Huff_arb *arbre , int *dc_init) {
    int code = 0;
    int i = 0;
    Huff_arb *courant = arbre;
    while (courant->est_mot_de_code == 0) {
        int bit = read_bit();
        if (bit == 0) {
            courant = courant->fg;
        } else {
            courant = courant->fd;
        }
}