#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

void *zigzag_inv(void *quant_vect,uint8_t prec){
    uint8_t poin = 0; //indice dans le vectteur zigzagué
    
    uint8_t i =0;
    uint8_t j =0;//indices de case

    if (prec ==0){//liste des élèments en 8 bits
        uint8_t *table = malloc(64*sizeof(uint8_t));
        uint8_t *vect8 = (uint8_t *)quant_vect;
        while( poin < 64){
            
            table[i*8 + j] = vect8[poin];
            if( (i + j )% 2 ==0){//monter
                if( i>0){
                    if(j<7){
                        j++;
                        i--;
                    }
                    else{
                        i++;
                    }
                   
            }
                else{
                    j++;//ici il n'ya pas le cas où les deux s'arrivent
                }
            }
            else{//descente
                if(i<7){
                    if(j>0){
                        i++;
                        j--;
                    }
                    else{
                        i++;// cas de (1,0)
                    }
                }
                else{
                    j++;//en tout cas on est en bas
                }
            }
            poin++;


        }
        return table;
    }
    else{//liste des élèments en 16 bits
        uint16_t *table= malloc(64 * sizeof(uint16_t));
        uint16_t *vect16 = (uint16_t *)quant_vect;
        while( poin < 64){
            
            table[i*8 +j] = vect16[poin];
            if( (i + j )% 2 ==0){//monter
                if( i>0){
                    if(j<7){
                        j++;
                        i--;
                    }
                    else{
                        i++;
                    }
                   
            }
                else{
                    j++;//ici il n'ya pas le cas où les deux s'arrivent
                }
            }
            else{//descente
                if(i<7){
                    if(j>0){
                        i++;
                        j--;
                    }
                    else{
                        i++;// cas de (1,0)
                    }
                }
                else{
                    j++;//en tout cas on est en bas
                }
            }
            poin++;


        }
        return table;
    }
   
}