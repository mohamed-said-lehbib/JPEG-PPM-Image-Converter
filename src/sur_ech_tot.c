#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "structs.h"

extern uint16_t hauteur;
extern uint16_t largeur;

umatrice *sur_ech_horiz(umatrice*com, infos_comp **infos_img){
    
    //-----------recuperation des donnees------------------//

    uint8_t **y = com[0].data;           //  recuperation des composantes
    uint8_t **cb = com[1].data;
    uint8_t **cr = com[2].data;

    uint8_t hy = 8*infos_img[0]->h_i; // recuperation des dimensions des composantes
    uint8_t vy = 8*infos_img[0]->v_i;         
    uint8_t hcb = 8*infos_img[1]->h_i;
    uint8_t vcb = 8*infos_img[1]->v_i;    
    uint8_t hcr = 8*infos_img[2]->h_i;
    uint8_t vcr = 8*infos_img[2]->v_i; 

    uint8_t facteur_hcb = hy/hcb;   // les facteurs d'echantillonnage
    uint8_t facteur_hcr = hy/hcr;  

    //--------------Allocation de l'espace ---------------//

    umatrice new_cb_block ;     
    new_cb_block.data= malloc(vcb*sizeof(uint8_t*));
    umatrice new_cr_block ;
    new_cr_block.data= malloc(vcr*sizeof(uint8_t*));
    for (uint8_t i = 0; i < vcb; i++) {
        new_cb_block.data[i] = malloc(hy * sizeof(uint8_t));
        new_cr_block.data[i] = malloc(hy * sizeof(uint8_t));
    }

    //--------------remplissage de la mcu---------------//

                //----Pour Cb------//

    for (int i =0;i< vcb;i++){
        uint16_t decale=0;
        for (int j = 0 ; j<hcb; j++){
            decale = facteur_hcb*j;       // pour commencer là oû on est arreter l'iteration precedente
            for (int k=0 ; k < facteur_hcb; k++){
                new_cb_block.data[i][k+decale] = cb[i][j];
            }
        }
    }
                 //----Pour Cr------//
    for (int i =0;i< vcr;i++){
        uint16_t decale=0;
        for (int j = 0 ; j<hcr; j++){
            /*uint16_t valeur = cr[i][j];*/
            decale = facteur_hcb*j;       // pour commencer là oû on est arreter l'iteration precedente
            for (int k=0 ; k < facteur_hcb; k++){
                new_cr_block.data[i][k+decale] = cr[i][j];
            }
        }
    }
    com[1] = new_cb_block;
    com[2] = new_cr_block;
    //------------LiBERTEEEEEEEE----------//
    for (uint8_t i = 0; i < vcb; i++) {
        free(cb[i]);
    }
    free(cb);

    for (uint8_t i = 0; i < vcr; i++) {
        free(cr[i]);
    }
    free(cr);

    return com;
}





//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//





umatrice*sur_ech_ver(umatrice*com, infos_comp **infos_img){
    
    //-----------recuperation des donnees------------------//

    uint8_t **y = com[0].data;           //  recuperation des composantes
    uint8_t **cb = com[1].data;
    uint8_t **cr = com[2].data;

    uint8_t hy = 8*infos_img[0]->h_i; // recuperation des dimensions des composantes
    uint8_t vy = 8*infos_img[0]->v_i;         
    uint8_t hcb = 8*infos_img[1]->h_i;
    uint8_t vcb = 8*infos_img[1]->v_i;    
    uint8_t hcr = 8*infos_img[2]->h_i;
    uint8_t vcr = 8*infos_img[2]->v_i;

    uint8_t facteur_vcb = vy/vcb;  // les facteurs d'echantillonnage
    uint8_t facteur_vcr = vy/vcr;   

    //--------------Allocation de l'espace ---------------//

    umatrice new_cb_block ;
    new_cb_block.data= malloc(vy*sizeof(uint8_t*));
    umatrice new_cr_block ;
    new_cr_block.data= malloc(vy*sizeof(uint8_t*));
    for (uint8_t i = 0; i < vy; i++) {
        new_cb_block.data[i] = malloc(hcb * sizeof(uint8_t));
        new_cr_block.data[i] = malloc(hcr * sizeof(uint8_t));
    }


    //--------------remplissage de la mcu---------------//


                //----Pour Cb------//



    for (int i =0;i< vcb;i++){
        uint16_t decale=0;
        for (int j = 0 ; j<hcb; j++){
            decale = facteur_vcb*i;       // pour commencer là oû on est arreter l'iteration precedente
            for (int k=0 ; k < facteur_vcb; k++){
                new_cb_block.data[k+decale][j] = cb[i][j];
            }
        }
    }

                 //----Pour Cr------//



    for (int i =0;i< vcb;i++){
        uint16_t decale=0;
        for (int j = 0 ; j<hcr; j++){
            /*uint16_t valeur = cr[i][j];*/
            decale = facteur_vcr*i;       // pour commencer là oû on est arreter l'iteration precedente
            for (int k=0 ; k < facteur_vcr; k++){
                new_cr_block.data[k+decale][j] = cr[i][j];
            }
        }
    }

    com[1] = new_cb_block;
    com[2] = new_cr_block;

    //------------LiBERTEEEEEEEE----------//

    for (uint8_t i = 0; i < vcb; i++) {
        free(cb[i]);
    }
    free(cb);

    for (uint8_t i = 0; i < vcr; i++) {
        free(cr[i]);
    }
    free(cr);

    return com;
}


umatrice *sur_ech(umatrice*com, infos_comp **infos_img){

    uint8_t hy = infos_img[0]->h_i; // recuperation des dimensions des composantes
    uint8_t vy = infos_img[0]->v_i;         
    uint8_t hcb = infos_img[1]->h_i;
    uint8_t vcb = infos_img[1]->v_i;    
    uint8_t hcr = infos_img[2]->h_i;
    uint8_t vcr = infos_img[2]->v_i; 



    //--------Cas de sous echantollage verticale----------------//
    if ( hy==hcb && hy == hcr){
        return sur_ech_ver(com, infos_img);
    }

    //--------Cas de sous echantollage horizontalle-------------//
    else if ( vy==vcb && vy == vcr){
        return sur_ech_horiz(com, infos_img);
    }


    //--------Cas de sous echantollage horizontalle et verticale-------------//
    else{
        umatrice* new_com = sur_ech_horiz(com, infos_img);
        infos_img[1]->h_i=hy;
        infos_img[2]->h_i=hy;
        return sur_ech_ver(new_com,infos_img);
    }

}
