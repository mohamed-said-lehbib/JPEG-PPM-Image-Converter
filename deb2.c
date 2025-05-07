#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


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
    typedef struct {
        uint8_t prec;//pour se souvenir de typpe de table
        uint8_t i_q;
        void *data;
    } quantification_table;
    uint8_t tab_q_traite =0; /*
    structure pour stocker les tableaux de différents tailles*/


    
    quantification_table **tables = malloc( 4* sizeof(quantification_table *)); //4 tableaux au maximum
    
    typedef struct{
        uint8_t *lengths;
        uint8_t* symboles;
    } huff_tbl;
    huff_tbl **huff_ac= malloc(4*sizeof(huff_tbl *));
    huff_tbl **huff_dc= malloc(4*sizeof(huff_tbl *));
    uint8_t ac = 0;//nombre de tables ac

    uint8_t dc = 0;//nombre de tables dc
    while((byte == 0xff)){//while True
        
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
            for(int j=8;j< taille_app;j++){
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
                uint8_t type_huff = info & 0x10;
                uint8_t index_huff = info & 0x0F;

                if (type_huff == 0){ //DC
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
                    huff_dc[dc] = coll;
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
            uint16_t len_sofb = fgetc(fptr);//octet de poids fort
            uint16_t len_sofs = fgetc(fptr);//octet de poids faible
            uint16_t taille_sofo = (len_sofb<<8) + len_sofs;
        }
        byte = fgetc(fptr); //avancer vers le ff
        }
    fclose(fptr);
    for (int u=0;u< tab_q_traite; u++){
        if (tables[i]){
            free(tables[i]->data);
            free(tables[i]);
        }
    }
    free(tables);
    free(huff_ac);
    free(huff_dc);
    //à compléter
    return 0;}
