#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "izigzag.h"
#include "idct.h"
#include "structs.h"
#include "bitstream.h"
#include "decde_Huff.h"
#include <string.h>


int main(int argc, char **argv){
    if( argc != 2){
        return 1;
        //nombre de paramètres non convenable
    }
    FILE *fptr = fopen(argv[1],"rb");
    if(fptr == NULL){
        perror("ouverture pas établie");
        return 2;
    }
    int i = 0;
    //SOI
    fgetc(fptr);
    fgetc(fptr); //avancer de deux octets 
    // tous les fichiers jpeg finissent par SOI et EOI
    unsigned char byte = fgetc(fptr);

    uint8_t tab_q_traite =0; /*
    structure pour stocker les tableaux de différents tailles*/

    
    
    quantification_table **tables = malloc( 4* sizeof(quantification_table *)); //4 tableaux au maximum
    

    huff_tbl **huff_ac= malloc(4*sizeof(huff_tbl *));
    huff_tbl **huff_dc= malloc(4*sizeof(huff_tbl *));
    uint8_t ac = 0;//nombre de tables ac

    uint8_t dc = 0;//nombre de tables dc

    infos_comp **infos_img =NULL;
    int N_comp = 0;
    int N_comp_sos = 0;
    SOS_val **sos_table = NULL;
    uint16_t hauteur =0 ;
    uint16_t largeur= 0;
    int16_t **brutes_Y = NULL;
    int16_t **brutes_Cb = NULL;
    int16_t **brutes_Cr= NULL;
    while((byte == 0xff)){//while pas de données brutes
        
        unsigned char flag = fgetc(fptr);

        if(flag == 0xe0){ //APP0 
            //taille de la section
            int o_fort = fgetc(fptr);
            int o_faible = fgetc(fptr);
            size_t taille_app= o_fort*256 + o_faible;

            //lecture de type
            unsigned char l1 = fgetc(fptr);
            unsigned char l2 = fgetc(fptr);
            unsigned char l3 = fgetc(fptr);
            unsigned char l4 = fgetc(fptr);
            unsigned char l5 = fgetc(fptr);
            unsigned char typ[5] = {l1,l2,l3,l4,l5};
   
            for(int j=7;j< taille_app;j++){
                fgetc(fptr);
            }
        }
        else if(flag == 0xfe){
            int len_com_b = fgetc(fptr); // commentaires
            int len_com_s = fgetc(fptr);
            int taille_comm = 256*len_com_b + len_com_s;
            for (int j=2;j<16;j++){
                fgetc(fptr);
            } 
        }
        else if(flag == 0xdb ){// Define Quantification Table
            int dqt_b = fgetc(fptr);
            int dqt_s = fgetc(fptr);
            int taille_dqt = 256*dqt_b +dqt_s;
        
        
            
            int j = 0;
            while(j<taille_dqt - 2){//lorsqu'il y ancore des tables à traiter
                //extraire la précision et l'indice
                uint8_t inter = fgetc(fptr);
                uint8_t prec = (inter & 0xF0)>>4; //précision
                uint8_t indi = inter & 0x0F;  //indice
                
                quantification_table *quan_ptr = malloc(sizeof(quantification_table));
                quan_ptr->prec =prec;
                quan_ptr->i_q = indi;

                if(prec == 0){//donc tableaux à 8 bits 
                    
                    uint8_t *quan_table = malloc(64*sizeof(uint8_t));
                    for(int k =0;k<64;k ++){
                        quan_table[k] = fgetc(fptr);
   
                    }
                    quan_ptr->data = quan_table;
                    j+= 65;
                    tables[tab_q_traite] = quan_ptr;
                    tab_q_traite++;
                }
            
                else{
                
                uint16_t n_tab_q = (taille_dqt -2)/129; 
                
                
                    uint16_t *quan_table = malloc(64*sizeof(uint16_t));
                    for(int k =0;k<64;k ++){
                        uint16_t quan_fort= fgetc(fptr);
                        uint16_t quan_faible = fgetc(fptr);
                        uint16_t quan_val = (quan_fort<<8) + quan_faible;
                        quan_table[k] = quan_val;
                    }
                    quan_ptr->data = quan_table;
                    j+=129;
                    tables[tab_q_traite] = quan_ptr;
                    tab_q_traite++;
                    

                }
            }

            }
        else if(flag == 0xc4){//Define Huffman table
            uint16_t len1 = fgetc(fptr);
            uint16_t len2 = fgetc(fptr);
            uint16_t len_huff = (len1<<8) + len2;

            
            int j=0;
            while( j <len_huff -2){
                //trouver les informatons supplémentaires
                uint16_t info = fgetc(fptr);
                uint8_t type_huff = (info & 0x10)>>4;
                uint8_t index_huff = info & 0x0F;

                if (type_huff == 0){ //DC
                    uint8_t *table_longuer = malloc(16*sizeof(uint8_t));//16 longouer et 16 c   ractères au maximum
                    int n_symb = 0;//nombre des symboles
                    for (int k=0;k<16;k++){
                        table_longuer[k] = fgetc(fptr);
                        printf("long %d : %d\n",k,table_longuer[k]);
                        n_symb += table_longuer[k];
                    }
                    uint8_t *symbols = malloc(n_symb*sizeof(uint8_t));
                    for(int k=0;k<n_symb;k++ ){
                        symbols[k] = fgetc(fptr);
           
                    }
                    huff_tbl *coll = malloc(sizeof(huff_tbl));
                    coll->lengths = table_longuer;
                    
                    coll->symboles = symbols;
                    huff_dc[dc] = coll;
                    for(int i=0;i<16;i++){
                    printf("valeur apres stockage %d \n",huff_dc[dc]->lengths[i]);};
                    dc++;//dc = indice de prochain table s'il existe
                    j +=  1 + 16 + n_symb;
                    
                }
                else { //AC
                    uint8_t *table_longuer = malloc(16*sizeof(uint8_t));//16 longouer et 16 c   ractères au maximum
                    int n_symb = 0;//nombre des symboles
                    for (int k=0;k<16;k++){
                        table_longuer[k] = fgetc(fptr);
                        n_symb += table_longuer[k];
                    }
                    uint8_t *symbols = malloc(n_symb*sizeof(uint8_t));
                    for(int k=0;k<n_symb;k++ ){
                        symbols[k] = fgetc(fptr);
         
                    }
                    
                    huff_tbl *coll = malloc(sizeof(huff_tbl));
                    coll->lengths = table_longuer;
                    coll->symboles = symbols;
                    huff_ac[ac] = coll;
                    ac++;//ac = indice de prochain table s'il existe
                    j += 1 + 16 + n_symb;
                }
            }
            
        }
        else if(flag == 0xc0){//SOF0
            //longuer
            uint16_t len_sofb = fgetc(fptr);//octet de poids fort
            uint16_t len_sofs = fgetc(fptr);//octet de poids faible
            uint16_t taille_sofo = (len_sofb<<8) + len_sofs;

            //précision
            uint8_t prec_sof = fgetc(fptr);

            //hauteur et largeur
            uint16_t haut_h = fgetc(fptr);
            uint16_t haut_b = fgetc(fptr);
            hauteur = (haut_h<<8) + haut_b;

            uint16_t lar_h = fgetc(fptr);
            uint16_t lar_b = fgetc(fptr);
            largeur = (lar_h<<8) + lar_b;

            //Nombre de composantes N
            uint8_t N_comp = fgetc(fptr);

            infos_comp **infos_img = malloc(N_comp*sizeof(infos_comp));
            for(int k=0;k<N_comp;k++){
                infos_comp* case_k=malloc(sizeof(infos_comp));
                uint8_t i_c = fgetc(fptr);
                case_k->i_c = i_c;
                uint8_t ech_fact =  fgetc(fptr);//facteur d'échantillonage
                case_k->h_i = (ech_fact>>4);
                case_k->v_i = (ech_fact & 0x0F);
                case_k->i_q = fgetc(fptr);//tableau de quantification

                infos_img[k] = case_k;
                

            }

        }
        else if(flag == 0xda){//SOS
            //longeur de section non brute
            uint16_t len_sos_b = fgetc(fptr);
            uint16_t len_sos_f = fgetc(fptr);
            uint16_t len_sos = (len_sos_b<<8) + len_sos_f;
            //nombre de composante sos
            uint8_t N_comp_sos = fgetc(fptr);
            
            SOS_val **sos_table = malloc(N_comp_sos*sizeof(SOS_val *));
            for(int k=0;k<N_comp_sos;k++){
                uint8_t i_c = fgetc(fptr);
                uint8_t ac_dc = fgetc(fptr);
                SOS_val *SH = malloc(sizeof(SOS_val));
                SH->i_c = i_c;
                SH->i_dc = (ac_dc >> 4);
                SH->i_ac = (ac_dc & 0x0F);
                sos_table[k] = SH;
                

            }
            fgetc(fptr);
            fgetc(fptr);
            fgetc(fptr);
            //extraction des data brutes------------
            //cas de non-sous echantionage
            
            uint8_t h_max = 1;//pas de sous ehantillonage,tous val 1
            uint8_t v_max = 1;
            uint8_t pix_lar = 8*h_max;
            uint8_t pix_haut = 8*v_max;
            printf("larg %d\n",largeur);
            uint16_t nb_mcu_x = ceil(largeur/pix_lar);
            printf("nbx %d\n",nb_mcu_x);
            uint16_t nb_mcu_y = ceil((hauteur/pix_haut));
            printf("nby %d\n",nb_mcu_y);
            brutes_Y = malloc(nb_mcu_x*nb_mcu_y*sizeof(int16_t *));
            brutes_Cb = malloc(nb_mcu_x*nb_mcu_y*sizeof(int16_t *));
            brutes_Cr = malloc(nb_mcu_x*nb_mcu_y*sizeof(int16_t *));
            for(int y=0;y<nb_mcu_y;y++){
                for(int x=0;x<nb_mcu_x;x++){
                    //pas besoin d'avoir h et v dans le cas invaders
                    brutes_Y[x+ nb_mcu_x*y] = malloc(64*sizeof(int16_t));
                    
                    for(int c=0;c<64;c++){
                        int16_t h_byte = fgetc(fptr);
                        
                        brutes_Y[x+ nb_mcu_x*y ][c] = h_byte ; 
                        printf("Y machin %x\n",brutes_Y[x+ nb_mcu_x*y ][c]);
                    }
                    brutes_Cb[x+ nb_mcu_x*y] = malloc(64*sizeof(int16_t));
                    for(int c=0;c<64;c++){
                        int16_t h_byte = fgetc(fptr);
                     
                        brutes_Cb[x+ nb_mcu_x*y ][c] = h_byte ; 
                    }
                    brutes_Cr[x+ nb_mcu_x*y] = malloc(64*sizeof(int16_t));
                    for(int c=0;c<64;c++){
                        int16_t h_byte = fgetc(fptr);
                        
                        brutes_Cr[x+ nb_mcu_x*y ][c] = h_byte ; 
                    }
                }
            }
            break;
        }
        else if(flag == 0xd9){//EOI
            break;//fin de ecture
        }
        byte = fgetc(fptr); //avancer vers le ff
        
        }
//--------------------------------------------------------------------decodage----------------------------------------------------------------------------------------------------------------------
    Huff_arb *arbre_dc = create_node();
    uint16_t nb_symbols = strlen((char *)huff_dc[0]->symboles); 
    uint8_t *data = huff_dc[0]->lengths;
    for (int i=0;i<16;i++){
        printf("data[%d] : %d hello\n",i,data[i]);
    }
  
    int *symbols = (int *)huff_dc[0]->symboles; 
    for (int i=0;i<nb_symbols;i++){
        printf("symboles[%d] : %d\n",i,symbols[i]);
    }
    uint16_t code = 0;
    int k = 0;
    for (int i = 0; i < 16; i++) {
        int len = i + 1;
        for (int j = 0; j < data[i]; j++) {
            insert_code(arbre_dc, code, symbols[k], len);
            code++;
            k++;
            }
        code <<= 1;
        }
    printf("Arbre Huffman construit avec succès.\n");
    int16_t bitstream = brutes_Y[0][0];
    printf("bitstream :%d",bitstream);
    /*uint8_t data[] = { 0b01111100 };
    BitStream bs;
    create_bitstream(&bs, data, 1);
    int dc_init = 0;
    int DC = decode_dc(arbre_dc, dc_init, &bs);
    printf("Valeur DC décodée : %d\n", DC);
    free_arbre(arbre_dc);  */
    




//------------------------------------------------------------------------ FIN------------------------------------------------------------------------------------------------------------
    
    fclose(fptr);
   
    for (int u=0;u< tab_q_traite; u++){
        if (tables[u]){
            free(tables[u]->data);
            free(tables[u]);
        }
        else{
            break;
        }
    }
    free(tables);
    for(i=0;i<4;i++){
        if (huff_ac[i] ){
            free(huff_ac[i]->lengths);
            free(huff_ac[i]->symboles);
            free(huff_ac[i]);
        }
        if (huff_dc[i]){
            free(huff_dc[i]->lengths);
            free(huff_dc[i]->symboles);
            free(huff_dc[i]);
        }
    }
    free(huff_ac);
    free(huff_dc);
    for (int k=0;k<N_comp;k++){
        free(infos_img[k]);
    }
    free(infos_img);
    for (int k=0;k<N_comp_sos;k++){
        free(sos_table[k]);
    }
    free(sos_table);
    
    return 0;}
