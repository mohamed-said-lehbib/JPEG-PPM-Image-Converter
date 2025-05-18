#ifndef GET_HEADER_H
#define GET_HEADER_H


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "structs.h"

void get_app0(FILE *fptr);

void get_comment(FILE *fptr);

void get_tables_q(FILE *fptr,
    quantification_table ***tables,uint8_t *tqt_ptr );

void get_huff(FILE *fptr, huff_tbl ***huff_dc,
    huff_tbl ***huff_ac,uint8_t* dc_ptr,uint8_t* ac_ptr);

void get_sof(FILE* fptr,infos_comp ***infos_img,
    uint16_t* ptr_hauteur,uint16_t* ptr_largeur,
    uint8_t* N_comp_ptr);

void get_sos(FILE* fptr,SOS_val ***sos_table,
    size_t* cap_ptr,uint8_t **brutes,
    uint16_t* ptr_N_brute,uint8_t* N_comp_ss_ptr);

#endif