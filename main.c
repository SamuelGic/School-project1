#include <stdio.h>

void *pointer;

int main() {

     struct hlavicka{
        int velkost;
        struct hlavicka *dalsie;
    };

    char pole[50];
    pointer = &pole;




    return 0;
}
