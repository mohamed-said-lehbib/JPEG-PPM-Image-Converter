#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "izigzag.h"
#include "idct.h"
#include "decde_Huff.h"
#include "structs.h"
#include <string.h>
#include "quant_inverse.h"
#include "lec_ppm.h"

int main(int argc, char **argv){
    if( argc != 2){//s'assure du nombre du paramètres
        return 1;
        //nombre de paramètres non convenable
    }
    FILE *fptr = fopen(argv[1],"rb");
    if(fptr == NULL){ //s'assurer qu'on trouve le fichier
        perror("ouverture pas établie");
        return 2;
    }
    
    //SOI
    fgetc(fptr);
    fgetc(fptr); //avancer de deux octets 
    // tous les fichiers jpeg finissent par SOI et EOI
    unsigned char byte = fgetc(fptr); //recpuperer le premier ff

    uint8_t tab_q_traite =0; /*tableaux de quantifications traités
    structure pour stocker les tableaux de différents tailles*/

    
    
    quantification_table **tables = malloc( 4* sizeof(quantification_table *)); //4 tableaux au maximum
    if (tables == NULL){//s'assurer que malloc marche
        fprintf(stderr,"allocation de mémoire ");
        exit(EXIT_FAILURE);
    }

    //initialiser les données de HUffmann
    huff_tbl **huff_ac= malloc(4*sizeof(huff_tbl *));
    
    if (huff_ac == NULL){//s'assurer que malloc marche
        fprintf(stderr,"allocation de mémoire ");
        exit(EXIT_FAILURE);
    }
    huff_tbl **huff_dc= malloc(4*sizeof(huff_tbl *));
    if (huff_dc == NULL ){
        fprintf(stderr,"allocation de mémoire ");
        exit(EXIT_FAILURE);
    }
    uint8_t ac = 0;//nombre de tables ac

    uint8_t dc = 0;//nombre de tables dc
    //structure pour souvegarder les données de section SOF
    infos_comp **infos_img =NULL;
    //souvegarde du nombre des composantes de SOF
    int N_comp = 0;
    //souvegarde du nombre des copmposantes de SOS
    int N_comp_sos = 0;
    //initialiser la section SOS
    SOS_val **sos_table = NULL;
    //souvegarder globalement la hauteur
    uint16_t hauteur =0 ;
    //souvegarder globalement la largeur
    uint16_t largeur= 0;
    //initialiser les données brutes
    uint8_t *brutes = NULL;
    //on ne sait pas leur taille,donc on met une taille variable cap
    size_t cap = 0;//taille variable
    //souvegarder en fin le nombre des données brutes
    uint16_t N_brute =0;
    while((byte == 0xff)){//while pas de données brutes
        
        unsigned char flag = fgetc(fptr);
       
        if(flag == 0xe0){ //APP0 
            //taille de la section
            int o_fort = fgetc(fptr);
            int o_faible = fgetc(fptr);
            size_t taille_app= o_fort*256 + o_faible;

            //lecture de type
            unsigned char l1 = fgetc(fptr);//J
            unsigned char l2 = fgetc(fptr);//F
            unsigned char l3 = fgetc(fptr);//I
            unsigned char l4 = fgetc(fptr);//F
            unsigned char l5 = fgetc(fptr);//'\0'
            unsigned char typ[5] = {l1,l2,l3,l4,l5};//JFIF
           
            for(int j=7;j< taille_app;j++){
                fgetc(fptr);//ignorer le reste de l'APP0
            }
        }
        else if(flag == 0xfe){
            int len_com_b = fgetc(fptr); // commentaires (section commentaires ignorable)
            int len_com_s = fgetc(fptr);
            int taille_comm = 256*len_com_b + len_com_s;//recupere la taill de commentaire
            for (int j=2;j<16;j++){
                fgetc(fptr);
            } 
        }
        else if(flag == 0xdb ){// Define Quantification Table
            int dqt_b = fgetc(fptr);
            int dqt_s = fgetc(fptr);
            int taille_dqt = 256*dqt_b +dqt_s;//taille de section DQT
        
        
            
            int j = 0;
            while(j<taille_dqt - 2){//lorsqu'il y ancore des tables à traiter
                //extraire la précision et l'indice
                uint8_t inter = fgetc(fptr);
                uint8_t prec = (inter & 0xF0)>>4; //précision
                uint8_t indi = inter & 0x0F;  //indice
                //ALLOUER LA MÉMOIRE POUR LE TABLEAU TEMPORAIRE
                quantification_table *quan_ptr = malloc(sizeof(quantification_table));
                
                quan_ptr->prec =prec;
                quan_ptr->i_q = indi;

                if(prec == 0){//donc tableaux à 8 bits 
                    
                    uint8_t *quan_table = malloc(64*sizeof(uint8_t));
                    //lire le tableau de quantification
                    for(int k =0;k<64;k ++){
                        //lire chaque élèment
                        quan_table[k] = fgetc(fptr);
                     
                    }
                    quan_ptr->data = quan_table;
                    //remplir la section data, a définir le type de ce data
                    j+= 65;//ajouter 64 + 1  (nombre des élements de la table + indice i_q)
                    tables[tab_q_traite] = quan_ptr;//stocker la table
                    tab_q_traite++;//incrémenter le nombre des tableaux traitées
                }
            
                else{//donc tableau d'élèments uint16_t
                

                
                
                    uint16_t *quan_table = malloc(64*sizeof(uint16_t));
                    for(int k =0;k<64;k ++){
                        uint16_t quan_fort= fgetc(fptr);
                        uint16_t quan_faible = fgetc(fptr);
                        uint16_t quan_val = (quan_fort<<8) + quan_faible;
                        quan_table[k] = quan_val;
                    }
                    quan_ptr->data = quan_table;
                    j+=129; // ici c'est nombre des élements*2 + 1 (octet d'indice)
                    tables[tab_q_traite] = quan_ptr;
                    tab_q_traite++;
                    

                }
            }

            }
        else if(flag == 0xc4){//Define Huffman table
            uint16_t len1 = fgetc(fptr);
            uint16_t len2 = fgetc(fptr);
            uint16_t len_huff = (len1<<8) + len2;//taille de section y inclus les octets de taille

            
            int j=0;
            while( j <len_huff -2){//s'il y a même une autre octet non exploité donc il y a un tableau tt entier
                //trouver les informatons supplémentaires
                uint16_t info = fgetc(fptr);
                uint8_t type_huff = (info & 0x10)>>4;//ac ou dc
                uint8_t index_huff = info & 0x0F;//indice de ce tableau

                if (type_huff == 0){ //DC
                    //initialization des tableaux des longuers
                    uint8_t *table_longuer = malloc(16*sizeof(uint8_t));//16 longouer et 16 c   ractères au maximum
                    int n_symb = 0;//nombre des symboles
                    for (int k=0;k<16;k++){
                        table_longuer[k] = fgetc(fptr);
                        n_symb += table_longuer[k];
                    }
                    //initialization des tableau des symols,maintenant qu'on connait la taile
                    uint8_t *symbols = malloc(n_symb*sizeof(uint8_t));
                    for(int k=0;k<n_symb;k++ ){
                        symbols[k] = fgetc(fptr);
                       
                    }
                    //utiliser la structure définis en hauut
                    huff_tbl *coll = malloc(sizeof(huff_tbl));
                    coll->lengths = table_longuer;
                    coll->symboles = symbols;
                    coll->nb_symb = n_symb;
                    //stocker le tableux avec les types correspondants
                    huff_dc[dc] = coll;
                    dc++;//dc = indice de prochain table s'il existe
                    j +=  1 + 16 + n_symb;//avancer avec le nombre des octets lus
                    
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
                    coll->nb_symb = n_symb;
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
            N_comp = fgetc(fptr);

            infos_img = malloc(N_comp*sizeof(infos_comp));
            for(int k=0;k<N_comp;k++){
                //allouer de la mémoire 
                infos_comp* case_k=malloc(sizeof(infos_comp));
                //type de composante,Y,Cb ou Cr
                uint8_t i_c = fgetc(fptr);
                case_k->i_c = i_c;
                //les facters d'échantillonage,4:2:0 ,...
                uint8_t ech_fact =  fgetc(fptr);//facteur d'échantillonage
                case_k->h_i = (ech_fact>>4);
                case_k->v_i = (ech_fact & 0x0F);
                case_k->i_q = fgetc(fptr);//indice tableau de quantification
                //stocker dans la case correspondante
                infos_img[k] = case_k;
                printf("infos_img[%d]->i_q = %d\n",k,infos_img[k]->i_q);

            }

        }
        else if(flag == 0xda){//SOS
            //longeur de section non brute
            uint16_t len_sos_b = fgetc(fptr);
            uint16_t len_sos_f = fgetc(fptr);
            uint16_t len_sos = (len_sos_b<<8) + len_sos_f;
            //nombre de composante sos
            N_comp_sos = fgetc(fptr);
            
            sos_table = malloc(N_comp_sos*sizeof(SOS_val *));
            for(int k=0;k<N_comp_sos;k++){
                //type de composante
                uint8_t i_c = fgetc(fptr);
                //indice ac/dc ,type et indice de table de huffman correspondant
                uint8_t ac_dc = fgetc(fptr);
                SOS_val *SH = malloc(sizeof(SOS_val));
                SH->i_c = i_c;
                SH->i_dc = (ac_dc >> 4);
                SH->i_ac = (ac_dc & 0x0F);
                //stocker ces valeurs
                sos_table[k] = SH;
                

            }
            fgetc(fptr);
            fgetc(fptr);
            fgetc(fptr);//bits à ignorer
            //extraction des data brutes----
            cap = 256;//initialisation de cap
            byte = fgetc(fptr);
            uint32_t pointer_vr = 0;
            brutes = malloc(cap*sizeof(uint8_t));
            uint8_t next = fgetc(fptr);
            N_brute = 0;
            //lire deuc bytes pour vérifier si on est dans le EOF ou pas
            while((next != 0xd9) | (byte != 0xff)){
               
                

                        
                    
             
                
                
                brutes[pointer_vr++] = byte;//le pointeur d'incrémente à chaque stockage
                byte = next;
                next = fgetc(fptr);//avancer  toujours d'un pas
                
                
                
                //valeur de pointeur s'incrémente à chaque fois
                    
                
                
              
                if (pointer_vr >= cap){
                    //si la taille ne semble pas suffisante,on double la taille
                    cap <<=1;
                    brutes = realloc(brutes,cap);
                }
                
            }
        N_brute = pointer_vr ;   //recupérer le nombre des données brutes 
            break;
        }
        else if(flag == 0xd9){//EOI
            break;//fin de ecture
        }
        byte = fgetc(fptr); //avancer vers le ff
        printf("end : %x\n",byte);
        }
    //extraction des données brutes
    //--------------------------------------------------------------------decodage----------------------------------------------------------------------------------------------------------------------

    Huff_arb *arbre_dc = create_node();
    Huff_arb *arbre_ac = create_node();
    uint16_t nb_symbols_dc =huff_dc[0]->nb_symb ; 
    uint8_t *table_dc = huff_dc[0]->lengths;
    uint16_t nb_symbols_ac = huff_ac[0]->nb_symb; 
    uint8_t *table_ac = huff_ac[0]->lengths;
    uint8_t *symbols_dc= huff_dc[0]->symboles;
    uint8_t *symbols_ac= huff_ac[0]->symboles;

// remplir l'arbre dc
    uint16_t nb_mcux = largeur / 8;
    uint16_t nb_mcuy = hauteur /8;
    printf("\n%d ,%d\n ", nb_mcux,nb_mcuy);
    uint16_t code = 0;
    int k = 0;
    for (int i = 0; i < 16; i++) {
        int len = i + 1;
        for (int j = 0; j < table_dc[i]; j++) {
            insert_code(arbre_dc, code, symbols_dc[k], len);
            code++;
            k++;
        }
        code <<= 1;
    }
// remplir l'arbre AC
    code = 0; k = 0;
    for (int i = 0; i < 16; i++) {
        int len = i + 1;
        for (int j = 0; j < table_ac[i]; j++) {
            insert_code(arbre_ac, code, symbols_ac[k], len);
            code++;
            k++;
        }
        code <<= 1;
    }


    BitStream bs;
    create_bitstream(&bs, brutes, N_brute);
    int **blocs = decode_bloc(arbre_dc, arbre_ac, &bs ,nb_mcux,nb_mcuy);

    /*for(int i=0 ; i<1 ; i++){
        printf("\nBLOC\n");
        for (int j =0; j<64;j++){
            printf( "%x ", blocs[i][j]);
         }
    }*/
    

   


 
//   ------------------------------------Quantification inverse ----------------------------------------
    for (int i=0 ; i<nb_mcux*nb_mcuy ; i++){
        quant_inverse(blocs[i],tables[infos_img[0]->i_q]);
        
    }

 

    

    

 
//------------------------------------Zigzg inverse ----------------------------------------
    printf("je sui la \n ");
    int16_t **izz = malloc(nb_mcux*nb_mcuy*sizeof(int16_t *));
    for (int i=0 ; i<nb_mcux*nb_mcuy ; i++){
        izz[i] = zigzag_inv(blocs[i]);
        
    }
  
    

//------------------------------------IDCT---------------------------------------------------------
    uint8_t **idct = malloc(nb_mcux*nb_mcuy*sizeof(uint8_t));
    for (int k=0 ; k<nb_mcux*nb_mcuy ; k++){
        idct[k] = iDCT(izz[k]);
    }
    
    

//------------------------------------Ecriture dans le fichier PPM -------------------------------------------
    
    
    transf_pgm(idct, "gris.pgm",nb_mcux,nb_mcuy);
    
    //---------------------------FIN--------------------------------------------------------------

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
    for(int i=0;i<4;i++){
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
