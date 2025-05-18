#include <stdlib.h>
#include <stdio.h>

int *get_bytes(const char *fich){
    //stocker les valeurs de l'image dans un vecteur de bytes
    FILE *fptr = fopen(fich,"rb");
    int i = 0;

    fseek(fptr,0 , SEEK_END );
    long n = ftell(fptr); //retourner le nombre d'octets
    rewind(fptr);
    
    int i;
    int by;
    int *bytes = malloc(n*sizeof(int)); //
    for(i = 0;i<n;i++){
         /*avancer d'un pas et voir si c'est la limite*/
         by = fgetc(fptr);
         bytes[i] = by;
        
        
    }
    return bytes;
}
int main(int argc, char **argv){
    if( argc != 2){
        return 1;
        //nombre de paramètres non convenable
    }
    bytes = get_bytes(agrv[1]);
    return 0;
}