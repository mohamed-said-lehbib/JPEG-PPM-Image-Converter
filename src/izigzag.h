#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int16_t *zigzag_inv(void *quan_vect){
    uint8_t poin = 0; //indice dans le vectteur zigzagué
    
    uint8_t i =0;
    uint8_t j =0;//indices de case

    
        int16_t *table = malloc(64*sizeof(int16_t));
        int16_t *vect8 = (int16_t *)quan_vect;
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