#ifndef STRUCTS_H
#define STRUCTS_H


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

//valeurs de la section sos
typedef struct{
    uint8_t i_c;
    uint8_t i_ac;
    uint8_t i_dc;

} SOS_val;
//tables de quantification
typedef struct {
    uint8_t prec;//pour se souvenir de typpe de table
    uint8_t i_q;
    void *data;
} quantification_table;
//longuers et symboles de huffmann
typedef struct{
    uint8_t nb_symb;
    uint8_t *lengths;
    uint8_t* symboles;
} huff_tbl;
//valeurs de l'entête SOF,informations des composantes
typedef struct {
    uint8_t i_c;
    uint8_t h_i;
    uint8_t v_i;
    uint8_t i_q;//table de quantification
} infos_comp;



typedef struct {
    int16_t** data;
} matrice ;


typedef struct {
    matrice ycbcr[3];
} Mcu;


// Composantes Y Cb Cr
// typedef struct {
//     block8** Y;  
//     block8** Cb1;
//     block8** Cr1;
// } YCbCrstr;
//typedef struct {
//     uint8_t** mat;
// } block8x8;
// // MCUs
// typedef struct {
//     YCbCrstr* mcu;
// } Mcu;



#endif