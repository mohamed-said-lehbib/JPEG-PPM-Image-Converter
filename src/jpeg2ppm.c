#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "../include/izigzag.h"
#include "../include/idct.h"
#include "../include/decodage_huffman.h"
#include "../include/structs.h"
#include <string.h>
#include "../include/quant_inverse.h"
#include "../include/YCbCr2RGB.h"
#include "../include/ecriture_ppm.h"
#include "../include/sur_ech_tot.h"
#include "../include/get_header.h"

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
        perror("ouverture pas établie");
        return 2;
    }

    // SOI
    fgetc(fptr);
    fgetc(fptr); // avancer de deux octets
    // tous les fichiers jpeg finissent par SOI et EOI
    unsigned char byte = fgetc(fptr); // recpuperer le premier ff

    uint8_t tab_q_traite = 0; /*tableaux de quantifications traités
     structure pour stocker les tableaux de différents tailles*/

    quantification_table **tables = malloc(4 * sizeof(quantification_table *)); // 4 tableaux au maximum
    if (tables == NULL)
    { // s'assurer que malloc marche
        fprintf(stderr, "allocation de mémoire ");
        exit(EXIT_FAILURE);
    }

    // initialiser les données de HUffmann
    huff_tbl **huff_ac = malloc(4 * sizeof(huff_tbl *));

    if (huff_ac == NULL)
    { // s'assurer que malloc marche
        fprintf(stderr, "allocation de mémoire ");
        exit(EXIT_FAILURE);
    }
    huff_tbl **huff_dc = malloc(4 * sizeof(huff_tbl *));
    if (huff_dc == NULL)
    {
        fprintf(stderr, "allocation de mémoire ");
        exit(EXIT_FAILURE);
    }
    uint8_t ac = 0; // nombre de tables ac

    uint8_t dc = 0; // nombre de tables dc
    // structure pour souvegarder les données de section SOF
    infos_comp **infos_img = NULL;
    // souvegarde du nombre des composantes de SOF
    uint8_t N_comp = 0;
    // souvegarde du nombre des copmposantes de SOS
    uint8_t N_comp_sos = 0;
    // initialiser la section SOS
    SOS_val **sos_table = NULL;
    // souvegarder globalement la hauteur
    uint16_t hauteur = 0;
    // souvegarder globalement la largeur
    uint16_t largeur = 0;
    // initialiser les données brutes
    uint8_t *brutes = NULL;
    // on ne sait pas leur taille,donc on met une taille variable cap
    size_t cap = 0; // taille variable
    // souvegarder en fin le nombre des données brutes
    uint16_t N_brute = 0;
    while ((byte == 0xff))
    { // while pas de données brutes
        unsigned char flag = fgetc(fptr);
        if (flag == 0xe0) // récuperer le type de l'image,...
        {
            get_app0(fptr);
        }
        else if (flag == 0xfe) // section d'un commentaire,à ignorer
        {
            get_comment(fptr);
        }
        else if (flag == 0xdb) // section DQT,là il y a les tables de quantifictions et leurs indices
        {
            get_tables_q(fptr, &tables, &tab_q_traite); // modifier la valeur qu'on veut par un pointeur
        }
        else if (flag == 0xc4) // section DHT,là où il y a les indices,longuers et symboles correspondants au tablaux de Huffman
        {
            get_huff(fptr, &huff_dc, &huff_ac, &dc, &ac);
        }
        else if (flag == 0xc0) // section SOF0,on extrait ici les dimensions de l'image,et les tableaux
        // les tableaux de quantification correspondante à chaque composante et les facteurs d'échantillonage
        {
            get_sof(fptr, &infos_img, &hauteur, &largeur, &N_comp); // tout stocké dans info_img
        }
        else if (flag == 0xda) // section SOS,mà on extrait les correspondances entre les tableayx de Huffman
        // et les composante,et puis on extrait les données brutes
        {
            get_sos(fptr, &sos_table, &cap, &brutes, &N_brute, &N_comp_sos); // stockage dans sos_table des informations
            // des composantes
            // les donnees brutes sont dans brutes
            break;
        }
        else if (flag == 0xd9)
        {          // EOI
            break; // fin de ecture
        }
        byte = fgetc(fptr); // avancer vers le ff
    }

    // extraction des données brutes
    //--------------------------------------------------------------------decodage----------------------------------------------------------------------------------------------------------------------

    uint8_t nb_y = (uint8_t)infos_img[0]->h_i * infos_img[0]->v_i;
    uint8_t nb_cb = (uint8_t)(N_comp != 1) ? infos_img[1]->h_i * infos_img[1]->v_i : 0;
    uint8_t nb_cr = (uint8_t)(N_comp != 1) ? infos_img[2]->h_i * infos_img[2]->v_i : 0;

    uint8_t hy = infos_img[0]->h_i; // recuperation des dimensions des composantes
    uint8_t vy = infos_img[0]->v_i;

    uint16_t nb_mcux = (((largeur + 7) / 8) + hy - 1) / hy;
    uint16_t nb_mcuy = (((hauteur + 7) / 8) + vy - 1) / vy;
    Huff_arb **arbres_dc = malloc(dc * sizeof(huff_tbl));
    Huff_arb **arbres_ac = malloc(ac * sizeof(huff_tbl));
    // ramplir les arbres de huffman ( DC)
    for (int i = 0; i < dc; i++)
    {
        Huff_arb *arbre_dc = creer_noeud();

        uint8_t *table_dc = huff_dc[i]->lengths;

        uint8_t *symbols_dc = huff_dc[i]->symboles;
        uint16_t code = 0;
        int k = 0;
        for (int i = 0; i < 16; i++)
        {
            int len = i + 1;
            for (int j = 0; j < table_dc[i]; j++)
            {
                inserer_code(arbre_dc, code, symbols_dc[k], len);
                code++;
                k++;
            }
            code <<= 1;
        }
        arbres_dc[i] = arbre_dc;
    }
    // ramplir les arbres de huffman ( AC)
    for (int i = 0; i < ac; i++)
    {
        Huff_arb *arbre_ac = creer_noeud();

        uint8_t *table_ac = huff_ac[i]->lengths;

        uint8_t *symbols_ac = huff_ac[i]->symboles;
        uint16_t code = 0;
        int k = 0;
        for (int i = 0; i < 16; i++)
        {
            int len = i + 1;
            for (int j = 0; j < table_ac[i]; j++)
            {
                inserer_code(arbre_ac, code, symbols_ac[k], len);
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

    uint16_t nb[3] = {nb_y, nb_cb, nb_cr}; // nombre de composant par mcu
    BitStream bs;
    create_bitstream(&bs, brutes, N_brute); // transformation de la donnee brute en flux de bits

    //   -------------------------------------*********************-------------------------------------------------//
    //   --------------------------------------Decodage de Huffman----------------------------------------------------//
    //   -------------------------------------*********************-------------------------------------------------//

    MCU *mcus = decode_huff_image(arbres_dc, arbres_ac, &bs, nb_mcux, nb_mcuy, N_comp, huff_corr_dc, huff_corr_ac, nb);

    //   ------------------------------------**********************-------------------------------------------------//
    //   ------------------------------------Quantification inverse----- -------------------------------------------//
    //   ------------------------------------**********************-------------------------------------------------//

  
    for (int i = 0; i < nb_mcux * nb_mcuy; i++)
    {
        for (int j = 0; j < N_comp; j++)
        {
            for (int k = 0; k < nb[j]; k++)
            {
                quant_inverse(&mcus[i].comps[j].blocs[k], tables[infos_img[j]->i_q]);
            }
        }
    }

    //   ------------------------------------**********************-------------------------------------------------//
    //   -----------------------------Allocation de la memoire pour les donnes apres l'idct-------------------------//
    //   ------------------------------------**********************-------------------------------------------------//

    umatrice ***idct = malloc(nb_mcuy * sizeof(umatrice **));
    for (uint32_t i = 0; i < nb_mcuy; i++)
    {
        idct[i] = malloc(nb_mcux * sizeof(umatrice *));
        for (uint32_t j = 0; j < nb_mcux; j++)
        {
            idct[i][j] = malloc(N_comp * sizeof(umatrice));
            for (int k = 0; k < N_comp; k++)
            {
                idct[i][j][k].data = malloc(infos_img[k]->v_i * 8 * sizeof(uint8_t *));
                for (uint32_t l = 0; l < infos_img[k]->v_i * 8; l++)
                {
                    idct[i][j][k].data[l] = malloc(infos_img[k]->h_i * 8 * sizeof(uint8_t));
                }
            }
        }
    }

    //   ------------------------------------**********************-------------------------------------------------//
    //   ----------------------------------------IDCT && IZIGZAG----------------------------------------------------//
    //   ------------------------------------**********************-------------------------------------------------//

    /* On parcour tout les MCU puis les composantes et pour chaque composante on applique le zigzag */
    
    for (uint32_t i = 0; i < nb_mcux * nb_mcuy; i++)
    {
        for (int j = 0; j < N_comp; j++)
        {
            uint16_t decalex = 0;
            uint16_t decaley = 0;
            for (int k = 0; k < nb[j]; k++)
            {
                umatrice idc;
                idc.data = iDCT_rap(zigzag_inv(mcus[i].comps[j].blocs[k].data));
                if (k != 0 && k % infos_img[j]->h_i == 0)
                {
                    decalex = 0;
                    decaley += 8;
                }
                for (int l = 0; l < 8; l++)
                {
                    for (int m = 0; m < 8; m++)
                    {
                        idct[i / nb_mcux][i % nb_mcux][j].data[l + decaley][m + decalex] = idc.data[l][m];
                    }
                    free(idc.data[l]);
                }
                free(idc.data);

                decalex += 8;
            }
        }
    }

    //   ------------------------------------**********************-------------------------------------------------//
    //   -----------------------------Sur echantillonnage et ecriture en ppm----------------------------------------//
    //   ------------------------------------**********************-------------------------------------------------//

    if (N_comp == 1) // Si on a une seul composante on ecrit en pgm (niveau de gris)
    {
        char out_nom[256];//nom de l'output
        const char *fich_pt = strrchr(argv[1],'/');//finir au dernier /
        fich_pt++;
        snprintf(out_nom,256,"images/");//copier au début images/

        char nom_im[256];//juste le nom
        strncpy(nom_im,fich_pt,sizeof(nom_im));//copier le string dans nom_im
        
        nom_im[255] = '\0';
        char *extension = strrchr(nom_im,'.');//supprimer la section aprés .j...
        *extension = '\0';

        //images/ +nom_im + .pgm
        strncat(out_nom,nom_im,255 - strlen(out_nom));
        strncat(out_nom,".pgm",255 - strlen(out_nom));

        transf_pgm(idct, out_nom, largeur, hauteur);
    }

    /*Sinon on a trois composantes donc on applique un surechantillonnage si besoin.
      En suite, pour chaque MCU on applique la fonction YCbCr2RGB on stock le resultat
      dans la matrice image pour ecrire à la fin dans le fichier ppm*/

    else
    {
        for (uint32_t i = 0; i < nb_mcuy; i++)
        {
            for (uint32_t j = 0; j < nb_mcux; j++)
            {
                idct[i][j] = sur_ech(idct[i][j], infos_img);
            }
        }
        uint32_t **image;
        image = malloc(hauteur * sizeof(uint32_t *));
        for (uint32_t i = 0; i < hauteur; i++)
        {
            image[i] = malloc(largeur * sizeof(uint32_t));
        }
        for (uint32_t i = 0; i < nb_mcuy; i++)
        {
            for (uint32_t j = 0; j < nb_mcux; j++)
            {
                uint32_t **new = YCbCr2RGB(idct[i][j], hy, vy);
                for (uint32_t k = 0; k < vy * 8; k++)
                {
                    for (uint32_t l = 0; l < hy * 8; l++)
                    {
                        if (i * vy * 8 + k < hauteur && j * hy * 8 + l < largeur)
                        {
                            image[i * vy * 8 + k][j * hy * 8 + l] = new[k][l];
                        }
                    }
                    free(new[k]);
                }
                free(new);
            }
        }
        char out_nom[256];
        const char *fich_pt = strrchr(argv[1],'/');//finir au dernier /
        fich_pt++;
        snprintf(out_nom,256,"images/");//avoir image dans out

        char nom_im[256];
        strncpy(nom_im,fich_pt,sizeof(nom_im));//copier le string fich_pt dans nom_im
        
        nom_im[255] = '\0';
        char *extension = strrchr(nom_im,'.');//supprimer la section aprés .j...
        *extension = '\0';

        // images/ + nom_im + .ppm
        strncat(out_nom,nom_im,255 - strlen(out_nom));
        strncat(out_nom,".ppm",255 - strlen(out_nom));

        transf_ppm(image,out_nom , largeur, hauteur);
        for (uint32_t i = 0; i < hauteur; i++)
        {
            free(image[i]);
        }
        free(image);
    }

    //   ------------------------------------**********************-------------------------------------------------//
    //   -----------------------------------Liberation de la mémoire------------------------------------------------//
    //   ------------------------------------**********************-------------------------------------------------//

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
    for (int i = 0; i < ac; i++)

    {
        free(huff_ac[i]->lengths);
        free(huff_ac[i]->symboles);
        free(huff_ac[i]);
    }
    for (int i = 0; i < dc; i++)
    {
        free(huff_dc[i]->lengths);
        free(huff_dc[i]->symboles);
        free(huff_dc[i]);
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

    free(brutes);
  
    free(huff_corr_ac);
    free(huff_corr_dc);

    for (uint32_t i = 0; i < dc; i++)
    {
        free_arbre(arbres_dc[i]);
        free_arbre(arbres_ac[i]);
    }
    free(arbres_dc);
    free(arbres_ac);
    for (uint32_t i = 0; i < nb_mcux * nb_mcuy; i++)
    {
        for (int j = 0; j < N_comp; j++)
        {
            free(mcus[i].comps[j].blocs);
        }
        free(mcus[i].comps);
    }
    free(mcus);
    for (uint32_t i = 0; i < nb_mcuy; i++)
    {
        for (uint32_t j = 0; j < nb_mcux; j++)
        {
            for (int k = 0; k < N_comp; k++)
            {
                for (uint32_t l = 0; l < vy * 8; l++)
                {
                    free(idct[i][j][k].data[l]);
                }
                free(idct[i][j][k].data);
            }
            free(idct[i][j]);
        }
        free(idct[i]);
    }
    free(idct);
    
    return 0;
}
