#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

float C(uint16_t eps){//calcule de la fonction C 
    if(eps == 0){
        return 1/sqrt(2);
    }
    return 1;
}
float **iDCT(uint16_t *matrice){//nécessite une converion si 8 bits
    float **S = malloc(8*sizeof(float *));
    for(int ligne =0;ligne<8;ligne ++){
        S[ligne] = malloc(8*sizeof(float));
    }
    for ( int x = 0;x<8;x++){
        for( int y=0;y<8;y++){
            float Sxy = 0.0;
            for(int lamb=0;lamb<8;lamb++){
                float temp = C(lamb)*C(u)*cos((2*x+1)*lamb*M_PI/16);
                for(int u=0;u<8;u++){
                    Sxy += temp*
                cos((2*y+1)*u*M_PI/16)*matrice[8*lamb+u]/4;
                }

            }
            S[x][y] = Sxy;
        }
    }
    return S;
}