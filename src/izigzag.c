#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int16_t **zigzag_inv(int16_t *vect_in){//effectuer le zigzag inverse sur le vecteur input et retourne un int16
    uint8_t poin = 0; //indice dans le vectteur zigzagué
    
    uint8_t i =0;
    uint8_t j =0;//indices de case

    //allouer de la mémoire pour l'output
    int16_t **table = malloc(8*sizeof(int16_t*));
    for (int i=0; i<8; i++){
        table[i] = malloc(8*sizeof(int16_t));
    }
   
    while( poin < 64){
       
        table[i][j] = vect_in[poin];
        if( (i + j )% 2 ==0){//caractérise l montée
            if( i>0){
                if(j<7){//cas normal
                    j++;
                    i--;
                }
                else{//on est au bordure droit,donc on descent
                    i++;
                }
                
        }
            else{
                j++;//on en haut,donc on s'oriente à droite
            }
        }
        else{//descente
            if(i<7){
                if(j>0){//cas normal
                    i++;
                    j--;
                }
                else{
                    i++;// cas de (1,0),on est au bordure gauche donc on descent
                }
            }
            else{
                j++;//on est en bas donc on s'oriente à droite
            }
        }
        poin++;


    }
    
    return table;
    //quan vect déja libérée dans le main

}