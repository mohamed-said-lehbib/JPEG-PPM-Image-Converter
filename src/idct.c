#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

//calculer les constantes utilisées une fois pour to
//le calcule de ses valeurs a en moyen le même performance que si on l'est définies comme des 
//static const char
#define M_PI 3.14159265358979323846
#define cos6 cos(6*M_PI/16)
#define sin6 sin(6*M_PI/16)
#define sq2 sqrt(2)
#define cos3 cos(3*M_PI/16)
#define sin3 sin(3*M_PI/16)
#define cos1 cos(M_PI/16)
#define sin1 sin(M_PI/16)

double C(uint16_t eps){//calcule de la fonction C,coefficient de normalisation 
    if(eps == 0){
        return 1/sqrt(2);
    }
    return 1;
}
uint8_t **iDCT(int16_t **matrice){//iDCT naive
    uint8_t **S = malloc(8*sizeof(uint8_t *));
    for (int i=0;i<8;i++){
        S[i] = malloc(8*sizeof(uint8_t));
    }
   
    
    for ( int x = 0;x<8;x++){//itérer pour chaque ligne et colonne pour calculer
        for( int y=0;y<8;y++){
            double Sxy = 0.0;
            for(int lamb=0;lamb<8;lamb++){
                double temp = C(lamb)*cos((2*x+1)*lamb*M_PI/16);
                for(int u=0;u<8;u++){
                    Sxy += temp*
                C(u)*cos((2*y+1)*u*M_PI/16)*matrice[lamb][u]/4.0f;
                //c'est côuteux ,à chaque fois on calcule un cosinus
                }

            }
            S[x][y] = (uint8_t) fminf(255.0 , fmaxf(0.0,roundf(Sxy+128.0)));
            //transformé en uint_8 les valeurs float entre 0 et 255
        }
    }
    for (int i=0;i<8;i++){//libérer la mémoire,notamment la sortie de vecteur zigzag
        free(matrice[i]);
    }
    free(matrice);
    return S;
}
void Etape43(float *vect_in){//opérer sur-place
    float vect_out[8];
    vect_out[0] = vect_in[0];
    vect_out[1] = vect_in[4];
    vect_out[2] = vect_in[2];
    vect_out[3] = vect_in[6];
    vect_out[4] = (vect_in[1] - vect_in[7])/2.0f;
    vect_out[5] = vect_in[3]/sq2;
    vect_out[6] = vect_in[5]/sq2;
    vect_out[7] = (vect_in[1] + vect_in[7])/2.0f;
   
    for (int i=0;i<8;i++){
        vect_in[i] = vect_out[i];
    }
}

void Etape32(float *vect_in){
     float vect_out[8];
    
    vect_out[0] = (vect_in[0] + vect_in[1])/2.0f;
    vect_out[1] =   (vect_in[0] - vect_in[1])/2.0f;
    vect_out[2] = (vect_in[2]*cos6- vect_in[3]*sin6)/sq2;
    vect_out[3] = (vect_in[2]*sin6 + vect_in[3]*cos6)/sq2;
    vect_out[4] = (vect_in[4] + vect_in[6])/2.0f;
    vect_out[5] = (vect_in[7] - vect_in[5])/2.0f;
    vect_out[6] = (vect_in[4] - vect_in[6])/2.0f;
    vect_out[7] = (vect_in[5] + vect_in[7])/2.0f;
    
    for (int i=0;i<8;i++){
        vect_in[i] = vect_out[i];
    }
}

void Etape21(float *vect_in){
    float vect_out[8];
    
    vect_out[0] = (vect_in[0] + vect_in[3])/2.0f;
    vect_out[1] = (vect_in[2] + vect_in[1])/2.0f;
    vect_out[2] = (vect_in[1] - vect_in[2])/2.0f;
    vect_out[3] = (vect_in[0] - vect_in[3])/2.0f;;
    vect_out[4] = cos3*vect_in[4] - sin3*vect_in[7];
    vect_out[5] = cos1*vect_in[5] - sin1*vect_in[6];
    vect_out[6] = sin1*vect_in[5] + cos1*vect_in[6];
    vect_out[7] = sin3*vect_in[4] + cos3*vect_in[7];
    
    for (int i=0;i<8;i++){
        vect_in[i] = vect_out[i];
    }
}

void Etape10(float *vect_in){
  
    float vect_out[8];
    vect_out[0] = (vect_in[0] + vect_in[7])/2.0f;
    vect_out[1] = (vect_in[1] + vect_in[6])/2.0f;
    vect_out[2] = (vect_in[2] + vect_in[5])/2.0f;
    vect_out[3] = (vect_in[3] + vect_in[4])/2.0f;
    vect_out[4] = (vect_in[3] - vect_in[4])/2.0f;
    vect_out[5] = (vect_in[2] - vect_in[5])/2.0f;
    vect_out[6] = (vect_in[1] - vect_in[6])/2.0f;
    vect_out[7] = (vect_in[0] - vect_in[7])/2.0f;
    
    for (int i=0;i<8;i++){
        vect_in[i] = vect_out[i];
    }
}
uint8_t **iDCT_rap(int16_t **matrice){//approche factorisé de Lœffer
    
    float inter[8][8];//vecteur pour appliquer l'inverse DCT à chaque ligne
    float res[8][8];//vecteur pour appliquer l'inverse DCT à chaque colonne
 
    uint8_t **out = malloc(8*sizeof(uint8_t *));
    for(int i=0;i<8;i++){
         out[i] = malloc(8*sizeof(uint8_t));
    }
    //IDCT sur tout les lignes

    for (int i=0;i<8;i++){
        //Étape 4 à 3 
       
        for (int j=0;j<8; j++){
            inter[i][j] = (float)matrice[i][j];
        }
        Etape43(inter[i]);
        Etape32(inter[i]);
        Etape21(inter[i]);
        Etape10(inter[i]);
        
    }

     for (int j=0;j<8;j++){
     
        for (int i=0;i<8; i++){
           
            res[j][i] = inter[i][j];
        }
        Etape43(res[j]);
       
        Etape32(res[j]);
        Etape21(res[j]);
        Etape10(res[j]);
        
    }
    
    for(int i=0;i<8;i++){
        for (int j=0;j<8;j++){
            double  resji = res[j][i]*8 +128.0f;
            
            if (resji<0){resji = 0;}
            if (resji>255){resji = 255;}
            out[i][j] = (uint8_t) fminf(255.0f , fmaxf(0.0,roundf(resji)));
            //transformé en uint_8 les valeurs double entre 0 et 255
        }
    }
    for (int j=0;j<8;j++){//libérer la mémoire,notamment la sortie de vecteur zigzag
        free(matrice[j]);
    }
    free(matrice);
    return out;
}

