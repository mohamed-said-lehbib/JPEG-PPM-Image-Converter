#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

double C(uint16_t eps){//calcule de la fonction C 
    if(eps == 0){
        return 1/sqrt(2);
    }
    return 1;
}
uint8_t **iDCT(int16_t **matrice){//nécessite une converion si 8 bits
    uint8_t **S = malloc(8*sizeof(uint8_t*));
    for (int i=0; i<8; i++){
        S[i] = malloc(8*sizeof(uint8_t));
    }
uint8_t **iDCT(int16_t **matrice){//nécessite une converion si 8 bits
    uint8_t **S = malloc(8*sizeof(uint8_t *));
    for (int i=0;i<8;i++){
        S[i] = malloc(8*sizeof(uint8_t));
    }
   
    
    for ( int x = 0;x<8;x++){
        for( int y=0;y<8;y++){
            double Sxy = 0.0;
            for(int lamb=0;lamb<8;lamb++){
                double temp = C(lamb)*cos((2*x+1)*lamb*M_PI/16);
                for(int u=0;u<8;u++){
                    Sxy += temp*
                C(u)*cos((2*y+1)*u*M_PI/16)*matrice[lamb][u]/4.0f;
                C(u)*cos((2*y+1)*u*M_PI/16)*matrice[lamb][u]/4.0f;
                }

            }
            S[x][y] = (uint8_t) fminf(255.0f , fmaxf(0.0f,roundf(Sxy+128.0f)));
            S[x][y] = (uint8_t) fminf(255.0 , fmaxf(0.0,roundf(Sxy+128.0)));
        }
    }
    return S;
}