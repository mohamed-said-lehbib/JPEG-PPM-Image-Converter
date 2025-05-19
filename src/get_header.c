#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "../include/structs.h"

void get_app0(FILE *fptr){
            // APP0
            // taille de la section
            int o_fort = fgetc(fptr);
            int o_faible = fgetc(fptr);
            size_t taille_app = o_fort * 256 + o_faible;

            // lecture de type
            //unsigned char l1 = 
            fgetc(fptr);              // J
            //nsigned char l2 = 
            fgetc(fptr);              // F
            //unsigned char l3 =
             fgetc(fptr);              // I
            //unsigned char l4 =
             fgetc(fptr);              // F
            //unsigned char l5 =
             fgetc(fptr);              //'\0'
            // unsigned char typ[5] = {l1, l2, l3, l4, l5}; // JFIF

            for (size_t j = 7; j < taille_app; j++)
            {
                fgetc(fptr); // ignorer le reste de l'APP0
            }
}

void get_comment(FILE *fptr){//ignorer

            int len_com_b = fgetc(fptr); // commentaires (section commentaires ignorable)
            int len_com_s = fgetc(fptr);
            int taille_comm = 256 * len_com_b + len_com_s; // recupere la taill de commentaire
            for (int j = 2; j < taille_comm; j++)
            {
                fgetc(fptr);
            }
}

void get_tables_q(FILE *fptr,quantification_table ***tables_ptr,uint8_t *tqt_ptr ){
    // Define Quantification Table
            quantification_table **tables = *tables_ptr;
            int dqt_b = fgetc(fptr);
            int dqt_s = fgetc(fptr);
            int taille_dqt = 256 * dqt_b + dqt_s; // taille de section DQT
            
            int j = 0;
            while (j < taille_dqt - 2)
            { // lorsqu'il y ancore des tables Ã  traiter
                // extraire la précision et l'indice
                uint8_t inter = fgetc(fptr);
                uint8_t prec = (inter & 0xF0) >> 4; // précision
                uint8_t indi = inter & 0x0F;        // indice
                // ALLOUER LA MÃ‰MOIRE POUR LE TABLEAU TEMPORAIRE
                quantification_table *quan_ptr = malloc(sizeof(quantification_table));

                quan_ptr->prec = prec;
                quan_ptr->i_q = indi;
               
                if (prec == 0)
                { // donc tableaux Ã  8 bits

                    uint8_t *quan_table = malloc(64 * sizeof(uint8_t));
                    // lire le tableau de quantification
                    for (int k = 0; k < 64; k++)
                    {
                        // lire chaque élÃ¨ment
                        quan_table[k] = fgetc(fptr);
                    }
                    quan_ptr->data = quan_table;
                    // remplir la section data, a définir le type de ce data
                    j += 65;                         // ajouter 64 + 1  (nombre des élements de la table + indice i_q)
                    tables[*tqt_ptr] = quan_ptr; // stocker la table
                    (*tqt_ptr)++;  
                                  // incrémenter le nombre des tableaux traitées
                }
                
                else
                { // donc tableau d'élÃ¨ments uint16_t

                    uint16_t *quan_table = malloc(64 * sizeof(uint16_t));
                    for (int k = 0; k < 64; k++)
                    {
                        uint16_t quan_fort = fgetc(fptr);
                        uint16_t quan_faible = fgetc(fptr);
                        uint16_t quan_val = (quan_fort << 8) + quan_faible;
                        quan_table[k] = quan_val;
                    }
                    quan_ptr->data = quan_table;
                    j += 129; // ici c'est nombre des élements*2 + 1 (octet d'indice)
                    tables[*tqt_ptr] = quan_ptr;
                    (*tqt_ptr)++;
                }
            }
}

void get_huff(FILE *fptr, huff_tbl ***huff_dc_ptr,
    huff_tbl ***huff_ac_ptr,uint8_t* dc_ptr,uint8_t* ac_ptr){
        // Define Huffman table
            uint16_t len1 = fgetc(fptr);
            uint16_t len2 = fgetc(fptr);
            uint16_t len_huff = (len1 << 8) + len2; // taille de section y inclus les octets de taille

            int j = 0;
            while (j < len_huff - 2)
            { // s'il y a mÃªme une autre octet non exploité donc il y a un tableau tt entier
                // trouver les informatons supplémentaires
                uint16_t info = fgetc(fptr);
                uint8_t type_huff = (info & 0x10) >> 4; // ac ou dc
                // uint8_t index_huff = info & 0x0F;       // indice de ce tableau

                if (type_huff == 0)
                { // DC
                    // initialization des tableaux des longuers
                    uint8_t *table_longuer = malloc(16 * sizeof(uint8_t)); // 16 longouer et 16 c   ractÃ¨res au maximum
                    int n_symb = 0;                                        // nombre des symboles
                    for (int k = 0; k < 16; k++)
                    {
                        table_longuer[k] = fgetc(fptr);
                        n_symb += table_longuer[k];
                    }
                    // initialization des tableau des symols,maintenant qu'on connait la taile
                    uint8_t *symbols = malloc(n_symb * sizeof(uint8_t));
                    for (int k = 0; k < n_symb; k++)
                    {
                        symbols[k] = fgetc(fptr);
                    }
                    // utiliser la structure définis en hauut
                    huff_tbl *coll = malloc(sizeof(huff_tbl));
                    coll->lengths = table_longuer;
                    coll->symboles = symbols;
                    coll->nb_symb = n_symb;
                    // stocker le tableux avec les types correspondants
                    (*huff_dc_ptr)[*dc_ptr] = coll;
                    (*dc_ptr)++;                 // dc = indice de prochain table s'il existe
                    j += 1 + 16 + n_symb; // avancer avec le nombre des octets lus
                }
                else
                {                                                          // AC
                    uint8_t *table_longuer = malloc(16 * sizeof(uint8_t)); // 16 longouer et 16 c   ractÃ¨res au maximum
                    int n_symb = 0;                                        // nombre des symboles
                    for (int k = 0; k < 16; k++)
                    {
                        table_longuer[k] = fgetc(fptr);
                        n_symb += table_longuer[k];
                    }

                    uint8_t *symbols = malloc(n_symb * sizeof(uint8_t));
                    for (int k = 0; k < n_symb; k++)
                    {
                        symbols[k] = fgetc(fptr);
                    }

                    huff_tbl *coll = malloc(sizeof(huff_tbl));

                    coll->lengths = table_longuer;
                    coll->symboles = symbols;
                    coll->nb_symb = n_symb;
                    (*huff_ac_ptr)[*ac_ptr] = coll;

                    (*ac_ptr)++; // ac = indice de prochain table s'il existe
                    j += 1 + 16 + n_symb;
                }
            }
    }

void get_sof(FILE* fptr,infos_comp ***infos_img_ptr,
    uint16_t* ptr_hauteur,uint16_t* ptr_largeur,
    uint8_t* N_comp_ptr){
    // SOF0
            // longuer
            // uint16_t len_sofb = 
            fgetc(fptr); // octet de poids fort
            // uint16_t len_sofs = 
            fgetc(fptr); // octet de poids faible
            // uint16_t taille_sofo = (len_sofb << 8) + len_sofs;
         
            // précision
            // uint8_t prec_sof = 
            fgetc(fptr);

            // hauteur et largeur
            uint16_t haut_h = fgetc(fptr);
            uint16_t haut_b = fgetc(fptr);
            *ptr_hauteur = (haut_h << 8) + haut_b;

            uint16_t lar_h = fgetc(fptr);
            uint16_t lar_b = fgetc(fptr);
            *ptr_largeur = (lar_h << 8) + lar_b;

            // Nombre de composantes N
            *N_comp_ptr = fgetc(fptr);

            *infos_img_ptr = malloc(*N_comp_ptr * sizeof(infos_comp *));
            for (int k = 0; k < *N_comp_ptr; k++)
            {
                // allouer de la mémoire
                infos_comp *case_k = malloc(sizeof(infos_comp));
                // type de composante,Y,Cb ou Cr
                uint8_t i_c = fgetc(fptr);
                case_k->i_c = i_c;
                // les facters d'échantillonage,4:2:0 ,...
                uint8_t ech_fact = fgetc(fptr); // facteur d'échantillonage
                case_k->h_i = (ech_fact >> 4);
                           
              
                case_k->v_i = (ech_fact & 0x0F);
                case_k->i_q = fgetc(fptr); // indice tableau de quantification
                // stocker dans la case correspondante
                (*infos_img_ptr)[k] = case_k;
            }
}

void get_sos(FILE* fptr,SOS_val ***sos_table_ptr,
    size_t* cap_ptr,uint8_t **brutes_ptr,
    uint16_t* ptr_N_brute,uint8_t* N_comp_sos_ptr){
        // SOS
            // longeur de section non brute
            // uint16_t len_sos_b = 
            fgetc(fptr);
            // uint16_t len_sos_f = 
            fgetc(fptr);
            //uint16_t len_sos = (len_sos_b << 8) + len_sos_f;
            // nombre de composante sos
            *N_comp_sos_ptr = fgetc(fptr);

            *sos_table_ptr = malloc(*N_comp_sos_ptr * sizeof(SOS_val *));
            for (int k = 0; k < *N_comp_sos_ptr; k++)
            {
                // type de composante
                uint8_t i_c = fgetc(fptr);
                // indice ac/dc ,type et indice de table de huffman correspondant
                uint8_t ac_dc = fgetc(fptr);
                SOS_val *SH = malloc(sizeof(SOS_val));
                SH->i_c = i_c;
                SH->i_dc = (ac_dc >> 4);
                SH->i_ac = (ac_dc & 0x0F);
                // stocker ces valeurs
                (*sos_table_ptr)[k] = SH;
            }
            fgetc(fptr);
            fgetc(fptr);
            fgetc(fptr); // bits Ã  ignorer
            // extraction des data brutes----
            *cap_ptr = 256; // initialisation de cap
            uint8_t byte= fgetc(fptr);
            uint32_t pointer_vr = 0;
            *brutes_ptr = malloc(*cap_ptr * sizeof(uint8_t));
            uint8_t next = fgetc(fptr);
            *ptr_N_brute = 0;
            // lire deuc bytes pour vérifier si on est dans le EOF ou pas
            while ((next != 0xd9) | (byte != 0xff))
            {

                (*brutes_ptr)[pointer_vr++] = byte; // le pointeur d'incrémente Ã  chaque stockage
                byte = next;
                next = fgetc(fptr); // avancer  toujours d'un pas

                // valeur de pointeur s'incrémente Ã  chaque fois

                if (pointer_vr >= *cap_ptr)
                {   
                    // si la taille ne semble pas suffisante,on double la taille
                    *cap_ptr <<= 1;
                    (*brutes_ptr) = realloc(*brutes_ptr, *cap_ptr);
                }
            }
            *ptr_N_brute = pointer_vr; // recupérer le nombre des données brutes
            
}