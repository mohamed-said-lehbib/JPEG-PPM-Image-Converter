#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../include/structs.h"

extern uint16_t hauteur;
extern uint16_t largeur;


    //   --------------------------------------**********************-------------------------------------------------//
    //   ---------------------------------Sur echantillonnage horizontale---------------------------------------------//
    //   --------------------------------------**********************-------------------------------------------------//


umatrice *sur_ech_horiz(umatrice*com, infos_comp **infos_img){
    
    //-----------recuperation des donnees------------------//

          //  recuperation des composantes
    uint8_t **cb = com[1].data;
    uint8_t **cr = com[2].data;

    uint8_t hy = 8*infos_img[0]->h_i; // recuperation des dimensions des composantes         
    uint8_t hcb = 8*infos_img[1]->h_i;
    uint8_t vcb = 8*infos_img[1]->v_i;    
    uint8_t hcr = 8*infos_img[2]->h_i;
    uint8_t vcr = 8*infos_img[2]->v_i; 

    uint8_t facteur_hcb = hy/hcb;   // les facteurs d'echantillonnage
    uint8_t facteur_hcr = hy/hcr;   // On divese les dimension de y sur celle de cb et cr pour savoir 
                                    // combien de fois on va dupliquer la valeur de l'echantillonnage

    //--------------Allocation de l'espace ---------------//
    
   /* On alloue des nouveaux blocks pour dupliquer les valeurs des composantes Cb et Cr suivant les facteurs d'echantionnage
      et en suite on pointe les pointeurs de com vers ces nouveaux block */

    umatrice new_cb_block ;                 
    new_cb_block.data= malloc(vcb*sizeof(uint8_t*));        // les hauteurs des composantes ne changes pas dans 
    umatrice new_cr_block ;                                 // le sur echantillonnage horizontal 
    new_cr_block.data= malloc(vcr*sizeof(uint8_t*));
    for (uint8_t i = 0; i < vcb; i++) {
        new_cb_block.data[i] = malloc(hy * sizeof(uint8_t));        // Les largeurs prennent la valeur de celle Y
        new_cr_block.data[i] = malloc(hy * sizeof(uint8_t));
    }

    //--------------remplissage de la mcu---------------//

    /* On remplie les nouveaux blocks par les echantillons dupliquer. 
       Chaque echantillon sera ecrit facteur_hcb fois avec un marqueur 
       qui indique là oû on va commancer pour la prochaine iteration */

                //----Pour Cb------//

    for (int i =0;i< vcb;i++){
        uint8_t decale=0;
        for (int j = 0 ; j<hcb; j++){
            decale = facteur_hcb*j;                         // pour commencer là oû on est arreter dans l'iteration precedente
            for (int k=0 ; k < facteur_hcb; k++){
                new_cb_block.data[i][k+decale] = cb[i][j];  // On remplit le nouveu block par les valeur dupliqué facteur_hcb fois
            }
        }
    }
                 //----Pour Cr------//

    /* On remplie les nouveaux blocks par les echantillons dupliquer. 
       Chaque echantillon sera ecrit facteur_hcr fois avec un marqueur 
       qui indique là oû on va commancer pour la prochaine iteration */

    for (int i =0;i< vcr;i++){
        uint8_t decale=0;
        for (int j = 0 ; j<hcr; j++){
            decale = facteur_hcr*j;                         // pour commencer là oû on est arreter l'iteration precedente
            for (int k=0 ; k < facteur_hcb; k++){
                new_cr_block.data[i][k+decale] = cr[i][j];  // On remplit le nouveu block par les valeur dupliqué facteur_hrb fois
            }
        }
    }
    com[1] = new_cb_block;                                  // On point les pointeurs des composantes vers les nouveux block
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

    //   ------------------------------------**********************-------------------------------------------------//
    //   ---------------------------------Sur echantillonnage verticale---------------------------------------------//
    //   ------------------------------------**********************-------------------------------------------------//

umatrice*sur_ech_ver(umatrice*com, infos_comp **infos_img){
    
    //-----------recuperation des donnees------------------//
                              
    uint8_t **cb = com[1].data;                //  recuperation des composantes
    uint8_t **cr = com[2].data;

    uint8_t vy = 8*infos_img[0]->v_i;          // recuperation des dimensions des composantes
    uint8_t hcb = 8*infos_img[1]->h_i;
    uint8_t vcb = 8*infos_img[1]->v_i;    
    uint8_t hcr = 8*infos_img[2]->h_i;
    uint8_t vcr = 8*infos_img[2]->v_i;

    uint8_t facteur_vcb = vy/vcb;              // les facteurs d'echantillonnage
    uint8_t facteur_vcr = vy/vcr;   

    //--------------Allocation de l'espace ---------------//

    /* On alloue des nouveaux blocks pour dupliquer les valeurs des composantes Cb et Cr suivant les facteurs d'echantionnage
      et en suite on pointe les pointeurs de com vers ces nouveaux block */

    umatrice new_cb_block ;
    new_cb_block.data= malloc(vy*sizeof(uint8_t*));                 // Les hauteurs prennent la valeur de celle Y
    umatrice new_cr_block ;                              
    new_cr_block.data= malloc(vy*sizeof(uint8_t*));
    for (uint8_t i = 0; i < vy; i++) {
        new_cb_block.data[i] = malloc(hcb * sizeof(uint8_t));       // les largeurs des composantes ne changes pas dans
        new_cr_block.data[i] = malloc(hcr * sizeof(uint8_t));       // le sur echantillonnage verticale 
    }

    //--------------remplissage de la mcu---------------//


                //----Pour Cb------//

    /* On remplie les nouveaux blocks par les echantillons dupliquer. 
       Chaque echantillon sera ecrit facteur_vcb fois avec un marqueur 
       qui indique là oû on va commancer pour la prochaine iteration */

    for (int i =0;i< vcb;i++){
        uint8_t decale=0;
        for (int j = 0 ; j<hcb; j++){
            decale = facteur_vcb*i;                                 // pour commencer là oû on est arreter dans l'iteration precedente
            for (int k=0 ; k < facteur_vcb; k++){
                new_cb_block.data[k+decale][j] = cb[i][j];          // On remplit le nouveu block par les valeur dupliqué facteur_vcb fois
            }
        }
    }
                 //----Pour Cr------//

    /* On remplie les nouveaux blocks par les echantillons dupliquer. 
       Chaque echantillon sera ecrit facteur_vcr fois avec un marqueur 
       qui indique là oû on va commancer pour la prochaine iteration */

    for (int i =0;i< vcb;i++){
        uint8_t decale=0;
        for (int j = 0 ; j<hcr; j++){
            decale = facteur_vcr*i;                                // pour commencer là oû on est arreter dans l'iteration precedente
            for (int k=0 ; k < facteur_vcr; k++){
                new_cr_block.data[k+decale][j] = cr[i][j];}        // On remplit le nouveu block par les valeur dupliqué facteur_vcr fois
        }
    }
    com[1] = new_cb_block;                                         // On point les pointeurs des composantes vers les nouveux block
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


    //   ------------------------------------****************************************-------------------------------------------------//
    //   ---------------------------------Sur echantillonnage horizontale et/ou verticale---------------------------------------------//
    //   ------------------------------------****************************************-------------------------------------------------//


umatrice *sur_ech(umatrice*com, infos_comp **infos_img){

    uint8_t hy = infos_img[0]->h_i; // recuperation des dimensions des composantes
    uint8_t vy = infos_img[0]->v_i;         
    uint8_t hcb = infos_img[1]->h_i;
    uint8_t vcb = infos_img[1]->v_i;    
    uint8_t hcr = infos_img[2]->h_i;
    uint8_t vcr = infos_img[2]->v_i; 

    //--------Cas de sous echantollage verticale----------------//
    if ((hy==hcb || hy ==hcr)&&(vy!=vcb || vy !=vcr)){                   // Si les hauteurs sont differentes alors on fait 
        return sur_ech_ver(com, infos_img);                             // un echantillonnage verticale
    }

    //--------Cas de sous echantollage horizontalle-------------//
    else if ((vy==vcb || vy ==vcr)&&(hy!=hcb || hy !=hcr)){              // Si les largeurs sont differentes alors on fait 
        return sur_ech_horiz(com, infos_img);                           // un echantillonnage horizontale
    }

    //--------Cas de sous echantollage horizontalle et verticale-------------//
    else if ((vy!=vcb || vy !=vcr)&&( hy!=hcb || hy !=hcr)){              // Si les largeurs et les hauteurs sont differentes alors on fait                                           
        umatrice* new_com = sur_ech_horiz(com, infos_img);                // un echantillonnage horizontale et verticale
        infos_comp *temp[3];
        for (int i = 0; i < 3; i++) {
            temp[i] = malloc(sizeof(infos_comp));
            *temp[i] = *infos_img[i];  
        }
        temp[1]->h_i=hy;
        temp[2]->h_i=hy;
        umatrice *res=sur_ech_ver(new_com,temp);
        for (int i = 0; i < 3; i++) {
            free(temp[i]);
        }
        return res;
    }
    return com;
}
