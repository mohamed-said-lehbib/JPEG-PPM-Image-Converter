#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "izigzag.h"
#include "idct.h"
#include "decde_Huff.h"
#include "structs.h"
#include <string.h>
#include "quant_inverse.h"
#include "YCbCr2RGB.h"
#include "ecriture_ppm.h"
#include "sur_ech_tot.h"


uint8_t *get_indices(SOS_val **sos_table) {}
int main(int argc, char **argv)
{
    if (argc != 2)
    { // s'assure du nombre du paramÃ¨tres
        return 1;
        // nombre de paramÃ¨tres non convenable
    }
    FILE *fptr = fopen(argv[1], "rb");
    if (fptr == NULL)
    { // s'assurer qu'on trouve le fichier
        perror("ouverture pas Ã©tablie");
        return 2;
    }

    // SOI
    fgetc(fptr);
    fgetc(fptr); // avancer de deux octets
    // tous les fichiers jpeg finissent par SOI et EOI
    unsigned char byte = fgetc(fptr); // recpuperer le premier ff

    uint8_t tab_q_traite = 0; /*tableaux de quantifications traitÃ©s
     structure pour stocker les tableaux de diffÃ©rents tailles*/

    quantification_table **tables = malloc(4 * sizeof(quantification_table *)); // 4 tableaux au maximum
    if (tables == NULL)
    { // s'assurer que malloc marche
        fprintf(stderr, "allocation de mÃ©moire ");
        exit(EXIT_FAILURE);
    }

    // initialiser les donnÃ©es de HUffmann
    huff_tbl **huff_ac = malloc(4 * sizeof(huff_tbl *));

    if (huff_ac == NULL)
    { // s'assurer que malloc marche
        fprintf(stderr, "allocation de mÃ©moire ");
        exit(EXIT_FAILURE);
    }
    huff_tbl **huff_dc = malloc(4 * sizeof(huff_tbl *));
    if (huff_dc == NULL)
    {
        fprintf(stderr, "allocation de mÃ©moire ");
        exit(EXIT_FAILURE);
    }
    uint8_t ac = 0; // nombre de tables ac

    uint8_t dc = 0; // nombre de tables dc
    // structure pour souvegarder les donnÃ©es de section SOF
    infos_comp **infos_img = NULL;
    // souvegarde du nombre des composantes de SOF
    int N_comp = 0;
    // souvegarde du nombre des copmposantes de SOS
    int N_comp_sos = 0;
    // initialiser la section SOS
    SOS_val **sos_table = NULL;
    // souvegarder globalement la hauteur
    uint16_t hauteur = 0;
    // souvegarder globalement la largeur
    uint16_t largeur = 0;
    // initialiser les donnÃ©es brutes
    uint8_t *brutes = NULL;
    // on ne sait pas leur taille,donc on met une taille variable cap
    size_t cap = 0; // taille variable
    // souvegarder en fin le nombre des donnÃ©es brutes
    uint16_t N_brute = 0;
    while ((byte == 0xff))
    { // while pas de donnÃ©es brutes

        unsigned char flag = fgetc(fptr);

        if (flag == 0xe0)
        { // APP0
            // taille de la section
            int o_fort = fgetc(fptr);
            int o_faible = fgetc(fptr);
            size_t taille_app = o_fort * 256 + o_faible;

            // lecture de type
            unsigned char l1 = fgetc(fptr);              // J
            unsigned char l2 = fgetc(fptr);              // F
            unsigned char l3 = fgetc(fptr);              // I
            unsigned char l4 = fgetc(fptr);              // F
            unsigned char l5 = fgetc(fptr);              //'\0'
            // unsigned char typ[5] = {l1, l2, l3, l4, l5}; // JFIF

            for (size_t j = 7; j < taille_app; j++)
            {
                fgetc(fptr); // ignorer le reste de l'APP0
            }
        }
        else if (flag == 0xfe)
        {
            // int len_com_b = 
            fgetc(fptr); // commentaires (section commentaires ignorable)
            // int len_com_s = 
            fgetc(fptr);
            // int taille_comm = 256 * len_com_b + len_com_s; // recupere la taill de commentaire
            for (int j = 2; j < 16; j++)
            {
                fgetc(fptr);
            }
        }
        else if (flag == 0xdb)
        { // Define Quantification Table
            int dqt_b = fgetc(fptr);
            int dqt_s = fgetc(fptr);
            int taille_dqt = 256 * dqt_b + dqt_s; // taille de section DQT

            int j = 0;
            while (j < taille_dqt - 2)
            { // lorsqu'il y ancore des tables Ã  traiter
                // extraire la prÃ©cision et l'indice
                uint8_t inter = fgetc(fptr);
                uint8_t prec = (inter & 0xF0) >> 4; // prÃ©cision
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
                        // lire chaque Ã©lÃ¨ment
                        quan_table[k] = fgetc(fptr);
                    }
                    quan_ptr->data = quan_table;
                    // remplir la section data, a dÃ©finir le type de ce data
                    j += 65;                         // ajouter 64 + 1  (nombre des Ã©lements de la table + indice i_q)
                    tables[tab_q_traite] = quan_ptr; // stocker la table
                    tab_q_traite++;                  // incrÃ©menter le nombre des tableaux traitÃ©es
                }

                else
                { // donc tableau d'Ã©lÃ¨ments uint16_t

                    uint16_t *quan_table = malloc(64 * sizeof(uint16_t));
                    for (int k = 0; k < 64; k++)
                    {
                        uint16_t quan_fort = fgetc(fptr);
                        uint16_t quan_faible = fgetc(fptr);
                        uint16_t quan_val = (quan_fort << 8) + quan_faible;
                        quan_table[k] = quan_val;
                    }
                    quan_ptr->data = quan_table;
                    j += 129; // ici c'est nombre des Ã©lements*2 + 1 (octet d'indice)
                    tables[tab_q_traite] = quan_ptr;
                    tab_q_traite++;
                }
            }
        }
        else if (flag == 0xc4)
        { // Define Huffman table
            uint16_t len1 = fgetc(fptr);
            uint16_t len2 = fgetc(fptr);
            uint16_t len_huff = (len1 << 8) + len2; // taille de section y inclus les octets de taille

            int j = 0;
            while (j < len_huff - 2)
            { // s'il y a mÃªme une autre octet non exploitÃ© donc il y a un tableau tt entier
                // trouver les informatons supplÃ©mentaires
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
                    // utiliser la structure dÃ©finis en hauut
                    huff_tbl *coll = malloc(sizeof(huff_tbl));
                    coll->lengths = table_longuer;
                    coll->symboles = symbols;
                    coll->nb_symb = n_symb;
                    // stocker le tableux avec les types correspondants
                    huff_dc[dc] = coll;
                    dc++;                 // dc = indice de prochain table s'il existe
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
                    huff_ac[ac] = coll;

                    ac++; // ac = indice de prochain table s'il existe
                    j += 1 + 16 + n_symb;
                }
            }
        }
        else if (flag == 0xc0)
        { // SOF0
            // longuer
            // uint16_t len_sofb = 
            fgetc(fptr); // octet de poids fort
            // uint16_t len_sofs = 
            fgetc(fptr); // octet de poids faible
            // uint16_t taille_sofo = (len_sofb << 8) + len_sofs;

            // prÃ©cision
            // uint8_t prec_sof = 
            fgetc(fptr);

            // hauteur et largeur
            uint16_t haut_h = fgetc(fptr);
            uint16_t haut_b = fgetc(fptr);
            hauteur = (haut_h << 8) + haut_b;

            uint16_t lar_h = fgetc(fptr);
            uint16_t lar_b = fgetc(fptr);
            largeur = (lar_h << 8) + lar_b;

            // Nombre de composantes N
            N_comp = fgetc(fptr);

            infos_img = malloc(N_comp * sizeof(infos_comp));
            for (int k = 0; k < N_comp; k++)
            {
                // allouer de la mÃ©moire
                infos_comp *case_k = malloc(sizeof(infos_comp));
                // type de composante,Y,Cb ou Cr
                uint8_t i_c = fgetc(fptr);
                case_k->i_c = i_c;
                // les facters d'Ã©chantillonage,4:2:0 ,...
                uint8_t ech_fact = fgetc(fptr); // facteur d'Ã©chantillonage
                case_k->h_i = (ech_fact >> 4);
                case_k->v_i = (ech_fact & 0x0F);
                case_k->i_q = fgetc(fptr); // indice tableau de quantification
                // stocker dans la case correspondante
                infos_img[k] = case_k;
            }
        }
        else if (flag == 0xda)
        { // SOS
            // longeur de section non brute
            // uint16_t len_sos_b = 
            fgetc(fptr);
            // uint16_t len_sos_f = 
            fgetc(fptr);
            //uint16_t len_sos = (len_sos_b << 8) + len_sos_f;
            // nombre de composante sos
            N_comp_sos = fgetc(fptr);

            sos_table = malloc(N_comp_sos * sizeof(SOS_val *));
            for (int k = 0; k < N_comp_sos; k++)
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
                sos_table[k] = SH;
            }
            fgetc(fptr);
            fgetc(fptr);
            fgetc(fptr); // bits Ã  ignorer
            // extraction des data brutes----
            cap = 256; // initialisation de cap
            byte = fgetc(fptr);
            uint32_t pointer_vr = 0;
            brutes = malloc(cap * sizeof(uint8_t));
            uint8_t next = fgetc(fptr);
            N_brute = 0;
            // lire deuc bytes pour vÃ©rifier si on est dans le EOF ou pas
            while ((next != 0xd9) | (byte != 0xff))
            {

                brutes[pointer_vr++] = byte; // le pointeur d'incrÃ©mente Ã  chaque stockage
                byte = next;
                next = fgetc(fptr); // avancer  toujours d'un pas

                // valeur de pointeur s'incrÃ©mente Ã  chaque fois

                if (pointer_vr >= cap)
                {
                    // si la taille ne semble pas suffisante,on double la taille
                    cap <<= 1;
                    brutes = realloc(brutes, cap);
                }
            }
            N_brute = pointer_vr; // recupÃ©rer le nombre des donnÃ©es brutes
            break;
        }
        else if (flag == 0xd9)
        {          // EOI
            break; // fin de ecture
        }
        byte = fgetc(fptr); // avancer vers le ff
    
     
    }

    // extraction des donnÃ©es brutes
    //--------------------------------------------------------------------decodage----------------------------------------------------------------------------------------------------------------------

    uint8_t nb_y =(uint8_t) infos_img[0]->h_i * infos_img[0]->v_i;
    uint8_t nb_cb =(uint8_t) (N_comp!=1)?infos_img[1]->h_i * infos_img[1]->v_i:0;
    uint8_t nb_cr = (uint8_t)(N_comp!=1)?infos_img[2]->h_i * infos_img[2]->v_i:0;
    
    uint8_t hy = infos_img[0]->h_i; // recuperation des dimensions des composantes
    uint8_t vy = infos_img[0]->v_i;         
    // uint8_t hcb = (N_comp!=1)?infos_img[1]->h_i:0;
    // uint8_t vcb = (N_comp!=1)?infos_img[1]->v_i:0;    
    // uint8_t hcr = (N_comp!=1)?infos_img[2]->h_i:0;
    // uint8_t vcr = (N_comp!=1)?infos_img[2]->v_i:0; 

    uint16_t nb_mcux = (largeur + 7) / (8*hy);
    uint16_t nb_mcuy = (hauteur + 7) / (8*vy);
    Huff_arb **arbres_dc = malloc(dc * sizeof(huff_tbl));
    Huff_arb **arbres_ac = malloc(ac * sizeof(huff_tbl));
    // remplir les arbres db
    for (int i = 0; i < dc; i++)
    {
        Huff_arb *arbre_dc = create_node();

        uint8_t *table_dc = huff_dc[i]->lengths;

        uint8_t *symbols_dc = huff_dc[i]->symboles;
        uint16_t code = 0;
        int k = 0;
        for (int i = 0; i < 16; i++)
        {
            int len = i + 1;
            for (int j = 0; j < table_dc[i]; j++)
            {
                insert_code(arbre_dc, code, symbols_dc[k], len);
                code++;
                k++;
            }
            code <<= 1;
        }
        arbres_dc[i] = arbre_dc;
    }

    for (int i = 0; i < ac; i++)
    {
        Huff_arb *arbre_ac = create_node();

        uint8_t *table_ac = huff_ac[i]->lengths;

        uint8_t *symbols_ac = huff_ac[i]->symboles;
        uint16_t code = 0;
        int k = 0;
        for (int i = 0; i < 16; i++)
        {
            int len = i + 1;
            for (int j = 0; j < table_ac[i]; j++)
            {
                insert_code(arbre_ac, code, symbols_ac[k], len);
                code++;
                k++;
            }
            code <<= 1;
        }
        arbres_ac[i] = arbre_ac;
    }
    uint8_t *huff_corr_dc = malloc(N_comp * sizeof(uint8_t)); // table de huffman correspondant dc

    uint8_t *huff_corr_ac = malloc(N_comp * sizeof(uint8_t));
    for (int i = 0; i < N_comp; i++)
    {
        for (int j = 0; j < N_comp_sos; j++)
        {
            if (sos_table[j]->i_c == infos_img[i]->i_c)
            {
                huff_corr_dc[i] = sos_table[j]->i_dc;
                huff_corr_ac[i] = sos_table[j]->i_ac;
                break;
            }
        }
    }

    
    uint16_t nb[3] ={ nb_y,nb_cb,nb_cr} ;// nombre de composant par mcu
    printf(" hello %d , %d , %d ,% d, %d", nb[0],nb_cb,nb_cr, nb_mcux, nb_mcuy);
    printf(" %d, %d, %d\n", nb_mcux*nb_mcuy, hy, vy);
    printf( " \nhi\n");

    BitStream bs;
    create_bitstream(&bs, brutes, N_brute);
    MCU *blocs = decode_bloc(arbres_dc, arbres_ac, &bs, nb_mcux, nb_mcuy, N_comp, huff_corr_dc, huff_corr_ac, nb);

    // for( int i =0 ; i<8 ; i++){
    //     for (int j =0 ; j<8 ;j++){
    //         printf(" %x ", blocs[1].comps[1].blocs[0].data[i*8+j]);}
    //     printf( "\n");
    // }

    //   ------------------------------------Quantification inverse ----------------------------------------
    uint8_t *qt_corr = malloc(N_comp * sizeof(uint8_t));
    for (int i = 0; i < N_comp; i++)
    {
        qt_corr[i] = infos_img[i]->i_q;
    }
    for (int i = 0; i < nb_mcux * nb_mcuy; i++)
    {
        for (int j = 0; j < N_comp; j++)
        {  for (int k =0 ;k<nb[j];k++){
            quant_inverse(&blocs[i].comps[j].blocs[k], tables[infos_img[j]->i_q]);
        }
    }}
    

    

    //uint16_t nb_mcux = (largeur + 7) / (8*hy);
    //uint16_t nb_mcuy = (hauteur + 7) / (8*vy);



    //-----------------------Allocation de la memoire -----------------------------------//
    umatrice ***idct = malloc(nb_mcuy  * sizeof(umatrice **));
    for (uint32_t i = 0; i < nb_mcuy; i++){
        idct[i] = malloc(nb_mcux * sizeof(umatrice *));
        for (uint32_t j = 0; j < nb_mcux; j++){
            idct[i][j] = malloc(N_comp *sizeof(umatrice));
            for (int k = 0; k < N_comp; k++){
                idct[i][j][k].data=malloc(infos_img[k]->v_i*8*sizeof(uint8_t*));
                for (uint32_t l = 0; l < infos_img[k]->v_i*8; l++){
                    idct[i][j][k].data[l]=malloc(infos_img[k]->h_i*8*sizeof(uint8_t));
                }
            }
        }
    }

//------------------------------------------------------IDCT----------------------------------


    
    for (uint32_t i = 0; i < nb_mcux * nb_mcuy; i++){
        for (int j = 0; j < N_comp; j++){
            uint16_t decalex =0 ;
            uint16_t decaley =0 ;
            for( int k=0 ; k<nb[j] ; k++){
                umatrice idc;
                idc.data =iDCT(zigzag_inv(blocs[i].comps[j].blocs[k].data));
                if (k != 0 && k % infos_img[j]->h_i == 0) {
                    decalex = 0;
                    decaley += 8;
                }
                for (int l= 0; l<8;l++ ){
                    for (int m=0;m<8;m++){
                        idct[i/nb_mcux][i%nb_mcux][j].data[l+decaley][m+decalex] = idc.data[l][m];
                    }
                }
                decalex += 8;
            }
        }
    }

    // for( int i =0 ; i<8*vy ; i++){
    //     for (int j =0 ; j<8*hy ;j++){

    //    printf(" %x ", idct[0][0][0].data[i][j]);}
    //    printf( "\n");
    // }
    // printf("done hh \n");


    // for( int i =0 ; i<8 ; i++){
    //     for (int j =0 ; j<8 ;j++){

    //    printf(" %x ", idct[0][1][1].data[i][j]);}
    //    printf( "\n");
    //  }



    

// //    //------------------------------------Echantillonnage---------------------------------------------------------

    printf("avants \n");
    for (uint32_t i = 0; i < nb_mcuy; i++){
        for (uint32_t j = 0; j < nb_mcux; j++){
            idct[i][j]=sur_ech(idct[i][j],infos_img);
        }
    }
 
    // for( int i =0 ; i<8*vy ; i++){
    //     for (int j =0 ; j<8*hy ;j++){

    //    printf(" %x ", idct[2][0][1].data[i][j]);}
    //    printf( "\n");
    // }
    // printf("done hh \n");
    // for( int i =0 ; i<8 ; i++){
    //     for (int j =0 ; j<8 ;j++){

    //    printf(" %x ", idct[0][1][1].data[i][j]);}
    //    printf( "\n");
    //  }
    // printf("apres \n");
    
// //     // //------------------------------------Ecriture dans le fichier PPM -------------------------------------------

    uint32_t** image ;
    image= malloc(hauteur*sizeof(uint32_t*));
    for(uint32_t i =0;i<hauteur;i++){
        image[i]=malloc(largeur*sizeof(uint32_t));
    }
   if (N_comp == 1){
    for (uint32_t i = 0; i < nb_mcuy; i++){
        for (uint32_t j = 0; j < nb_mcux; j++){
            uint32_t*** new = idct[i][j];
            for (uint32_t k = 0; k < vy*8; k++){
                for (uint32_t l = 0; l < hy*8; l++){
                    if (i*vy*8+k < hauteur && j*hy*8+l < largeur) {
                    image[i*vy*8+k][j*hy*8+l]=new[0][k][l];}
                }
            }
        }
    }
    transf_pgm(image, "bisou.pgm",largeur,hauteur);
   }
    else{
        for (uint32_t i = 0; i < nb_mcuy; i++){
            for (uint32_t j = 0; j < nb_mcux; j++){
                uint32_t** new = YCbCr2RGB(idct[i][j],hy,vy);
                for (uint32_t k = 0; k < vy*8; k++){
                    for (uint32_t l = 0; l < hy*8; l++){
                        if (i*vy*8+k < hauteur && j*hy*8+l < largeur) {
                        image[i*vy*8+k][j*hy*8+l]=new[k][l];}
                    }
                }
            }
        }
    }
    // for( int i =0 ; i<hauteur ; i++){
    //     for (int j =0 ; j<largeur ;j++){

    //    printf(" %x ", image[i][j]);}
    //    printf( "\n");
    //     }
    transf_ppm(image, "yaaaay.ppm",largeur,hauteur);

    //---------------------------FIN--------------------------------------------------------------
    fclose(fptr);

    for (int u = 0; u < tab_q_traite; u++)
    {
        if (tables[u])
        {
            free(tables[u]->data);
            free(tables[u]);
        }
        else
        {
            break;
        }
    }
    free(tables);
    for (int i = 0; i < 4; i++)
    {
        if (huff_ac[i])
        {
            free(huff_ac[i]->lengths);
            free(huff_ac[i]->symboles);
            free(huff_ac[i]);
        }
        if (huff_dc[i])
        {
            free(huff_dc[i]->lengths);
            free(huff_dc[i]->symboles);
            free(huff_dc[i]);
        }
    }
    free(huff_ac);
    free(huff_dc);
    for (int k = 0; k < N_comp; k++)
    {
        free(infos_img[k]);
    }
    free(infos_img);
    for (int k = 0; k < N_comp_sos; k++)
    {
        free(sos_table[k]);
    }
    free(sos_table);
    
    
    // for (uint32_t i = 0; i < nb_mcuy; i++) {
    //     for (uint32_t j = 0; j < nb_mcux; j++) {
    //         for (int k = 0; k < N_comp; k++) {
    //             for (uint32_t l = 0; l < infos_img[k]->v_i * 8; l++) {
    //                 free(idct[i][j][k].data[l]);
    //             }
    //             free(idct[i][j][k].data);
    //         }
    //         free(idct[i][j]);
    //     }
    //     free(idct[i]);
    // }
    // free(idct);

    for(uint32_t i =0;i<hauteur;i++){
        free(image[i]);
    }
    free(image);
    return 0;
}
