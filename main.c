#include <stdio.h>
#include <math.h>
#include <stdlib.h>

char *heap;

int memory_check(void *ptr){

    if (ptr == NULL){       //ak je pointer NULL nemame co uvolnit
        return 0;
    }

    else if (*(int *) ((char *) ptr - sizeof(int)) > 0){        //alebo ak aj ukazuje na pamät ktora je uvolnena tak neuvolnujem zase
        return 0;
    }

    else {
        return 1;       //ak ani jedna podmienka neplati vratim 1 a idem uvolnovat
    }


}

int memory_free(void *valid_ptr){

    int cele_pole = sizeof(int), volna_pam1, volna_pam2;        //cele_pole je na prechadzanie a hladanie volnych blokov
                                                                // a je velkosti int lebo na prvych miestach mam zapisanu velkost pola


   // printf("%d\n", *(int *) ((char *) valid_ptr - sizeof(int)));
    *(int *) ((char *) valid_ptr - sizeof(int)) = abs(*(int *) ((char *) valid_ptr - sizeof(int)));
   // printf("%d\n", *(int *) ((char *) valid_ptr - sizeof(int)));


    volna_pam1 = *(int *) ((char *)heap + cele_pole);                                      //velkost prvej pamate
    volna_pam2 = *(int *) ((char *)heap + cele_pole + sizeof(int) + abs(volna_pam1));       //velkost nasledujucej pamate

    while (cele_pole < *(int *)heap){               //cyklus ide pokial neprejde cele pole a velkost toho pola je zapisana na zaciatku

        if (volna_pam1 > 0 && volna_pam2 > 0){      //ak su dve po sebe iduce pamate volne, cize kladne, tak ich idem spajat
                                                    //pamat ktora je alokovana reprezentujem zapornym cislom
            //printf("Spajam\n");

            *(int *) ((char *) heap + cele_pole) = volna_pam1 + volna_pam2 + sizeof(int);       //po spojeni pripocitam
                                                                                                 //k velkosti prvej pamate velkost druhej + velkost hlavicky
            volna_pam1 = *(int *) ((char *) heap + cele_pole);  //tu si ju hodim znova do premennej abz som ju vedel porovnat s dalsou

            for(int i = 0; i < volna_pam1; i++){
                *(char *) ((char *) heap + cele_pole + sizeof(int) + i) = 'a';      //potom zmenim vsetky mieste od lavicky na 'a'
            }                                                                       //respektive jeho hodnotu v ascii tabulke (97)

            if (volna_pam1 + cele_pole + sizeof(int) >= *(int *) ((char *) heap)){
                break;                                                              //predtym ako idem priradit nasledujucu alokovanu alebo volnu pamat
            }                                                                   //skontrolujem ci uz nie je posledna

            volna_pam2 = *(int *) ((char *) heap + cele_pole + sizeof(int) + volna_pam1);   //ak nie tak priradim dalsiu



            continue;


        }

        else{   //ak dve po sebe iduce pamate neboli volne, tak sa vykona else

            cele_pole += sizeof(int) + abs(volna_pam1);         //zapisem ze som presiel velkost pamäte plus hlavicku
            volna_pam1 = volna_pam2;                            //druhu pamät priradim do prvej

            if (volna_pam1 + cele_pole + sizeof(int) >= *(int *) ((char *) heap)){    //rovnako skontrolujem ci to nie je posledna pamät ak ano koncim cyklus
                break;
            }

            volna_pam2 = *(int *) ((char *) heap + cele_pole + sizeof(int) + abs(volna_pam1));      //inak priradim dalsiu pamät dodruhej premennej
            continue;

        }


    }

    return 0;


}

void *memory_alloc(unsigned int size){

    int cele_pole = sizeof(int), alokacia = 0, miesto =  *(int *)(heap + cele_pole);
    int jedna, dva;

    while (cele_pole < *(int *)heap){       //podobny princip ako v uvolnovani
                                            //prechadzam cele pole a zistujem, ci je niekde dostatocne miesto na alokovanie
        jedna = miesto;
        dva = (size + sizeof(int) + 1);     //bolo nutne kedze nam prisiel unsigned int a robil problemy v podmienke
                                            //sizeof(int) + 1 zabranuje vytvaraniu nulovej hlavicky, podstate to sluzi ako moja päticka
    if(jedna >= dva){                       //do ktorej sa ale dokaze tiez alokovat miesto

        alokacia = 1;   //ak je dost miesta alokaciu zmanim na 1, prerusim cyklus a idem alokovat
        break;

    }
        cele_pole += abs(miesto) + sizeof(int);     //ak nie posuniem sa dalej v poli a zistujem dalsiu pamat ci je volna alebo nie
        miesto = *(int *)(heap + cele_pole);        //posun na dalsiu pamät

    }


    if (alokacia){  //ak bolo miesto alokacia je 1

        dva = size;
        *(int *)(heap + cele_pole) = -size;         //v cele_pole je miesto hlavicky volnej a priradim tam - velkost co chcem alokovat
        *(int *)(heap + cele_pole + dva + sizeof(int)) = miesto - size - sizeof(int);   //potom sa posuniem o tu velkost a zapisem tam zvzsok co ostal z povodnej velkost pamate

        return (heap + cele_pole + sizeof(int));    //a vratim poziciu kde zacina ta pamät, preto + sizeof(int), co je moja hlavicka

    }

    else {

        return NULL;        // ak sa nic nenaslo vratim NULL
    }

}

void memory_init(void *ptr, unsigned int size){

    heap = (char *)ptr;                 //globalny pointer som priradil do pola

    for(int i = 0; i < size; i++){
        *(char *) ((char *) ptr + i) = 'a';       //naplnil som si cele pole 1 nech viem, ze to je volne miesto
    }

    *(int *)heap = size;                        //na prve miesto v poli som si zapisal velkost pola ktoru vyuziva v memorz_aloc
    *(int *)(heap + sizeof(int)) = size - 2 * sizeof(int);      //na druhe respektive druhe 4 miesta v poli som si zapisal velkost bloku ktory mam k dispozicii


}

void test(char *region, int odkade, int dokade, int pamat1, int pamat2, int pamat3, int nahodne){

    if (nahodne == 0){

        if (pamat1 == 50){

            int velk_blokov = 12;
            double pocitadlo = 0;
            double ideal_case = pamat1 / velk_blokov; //idealny pripad vyplnenie 100%

            char region[pamat1];
            memory_init(region, pamat1);

            while (1)
            {
                //alokujem rovnake bloky dokedy to je mozne
                char *test1_alokacia = memory_alloc(velk_blokov);
                if (test1_alokacia == NULL) //koniec alokacie
                    break;
                pocitadlo++;
            }

            float nase = pocitadlo / ideal_case * 100;
            printf("Uspesne alokovanych %.2lf%% blokov\n", nase);
        }

        if (pamat2 == 100){

            int velk_blokov = 12;
            double pocitadlo = 0;
            double idealne = pamat2 / velk_blokov; //idealny pripad vyplnenie 100%

            char region[pamat2];
            memory_init(region, pamat2);

            while (1)
            {
                //alokujem rovnake bloky dokedy to je mozne
                char *test2_alokacia = memory_alloc(velk_blokov);
                if (test2_alokacia == NULL) //koniec alokacie
                    break;
                pocitadlo++;
            }

            float nase = pocitadlo / idealne * 100;
            printf("Uspesne alokovanych %.2lf%% blokov\n", nase);
        }

        if (pamat3 == 200){

            int velk_blokov = 12;
            double pocitadlo = 0;
            double idealne = pamat3 / velk_blokov; //idealny pripad vyplnenie 100%

            char region[pamat3];
            memory_init(region, pamat3);

            while (1)
            {
                //alokujem rovnake bloky dokedy to je mozne
                char *test3_alokacia = memory_alloc(velk_blokov);
                if (test3_alokacia == NULL) //koniec alokacie
                    break;
                pocitadlo++;
            }

            float nase = pocitadlo / idealne * 100;
            printf("Uspesne alokovanych %.2lf%% blokov\n\n", nase);
        }

    }

    if (nahodne == 1){

        if (pamat1){

            double pocitadlo = 0;
            int nah_cislo;
            int poctadlo2 = 0;

            char region[pamat1];
            memory_init(region, pamat1);

            while (1)
            {
                //alokujem rovnake bloky dokedy to je mozne
                nah_cislo =  (rand() % (dokade + 1 - odkade) + odkade);   //interval z ktoreho mam vyberat velkost bloku

                char *test1_alokacia = (char *)memory_alloc(nah_cislo);
                if (test1_alokacia == NULL){
                    break;                              //koniec alokacie
                }
                poctadlo2 = poctadlo2 + nah_cislo;
                pocitadlo++;
            }

            double nase = (poctadlo2) * 100 / pamat1;
            printf("Uspesne alokovanych %.2lf%% blokov\n", nase);

        }

        if (pamat2){

            double pocitadlo = 0;
            int nah_cislo;
            int poctadlo2 = 0;

            char region[pamat2];
            memory_init(region, pamat2);

            while (1)
            {
                //alokujem rovnake bloky dokedy to je mozne
                nah_cislo =  (rand() % (dokade + 1 - odkade) + odkade);   //interval z ktoreho mam vyberat velkost bloku

                char *test2_alokacia = (char *)memory_alloc(nah_cislo);
                if (test2_alokacia == NULL){
                    break;                              //koniec alokacie
                }


                poctadlo2 = poctadlo2 + nah_cislo;
                pocitadlo++;
            }

            double nase = (poctadlo2) * 100 / pamat2;
            printf("Uspesne alokovanych %.2lf%% blokov\n", nase);

        }

        if (pamat3){

            double pocitadlo = 0;
            int nah_cislo;
            int poctadlo2 = 0;

            char region[pamat3];
            memory_init(region, pamat3);

            while (1)
            {
                //alokujem rovnake bloky dokedy to je mozne
                nah_cislo =  (rand() % (dokade + 1 - odkade) + odkade);   //interval z ktoreho mam vyberat velkost bloku

                char *test3_alokacia = (char *)memory_alloc(nah_cislo);
                if (test3_alokacia == NULL){
                    break;                              //koniec alokacie
                }

                poctadlo2 = poctadlo2 + nah_cislo;
                pocitadlo++;
            }

            double nase = (poctadlo2) * 100 / pamat3;
            printf("Uspesne alokovanych %.2lf%% blokov\n\n", nase);

        }







    }

}

int main() {

    char region[150];
    void *pointer1 = NULL, *pointer2 = NULL, *pointer3 = NULL, *pointer4 = NULL, *pointer5 = NULL;
    int hodnota = 0;

/*
    memory_init(region, 50);

    pointer1 = memory_alloc(10);

    pointer2 = memory_alloc(10);

    pointer3 = memory_alloc(17);

    pointer4 = memory_alloc(10);

    pointer5 = memory_alloc(1);

    if (memory_check(pointer2)){
        hodnota = memory_free(pointer2);
    }

    if (memory_check(pointer1)){
        hodnota = memory_free(pointer1);
    }

    if (memory_check(pointer5)){
        hodnota = memory_free(pointer5);
    }

    if (memory_check(pointer4)){
        hodnota = memory_free(pointer4);
    }

    if (memory_check(pointer3)){
        hodnota = memory_free(pointer3);
    }

*/

    test(region, 8, 24, 50, 100, 200, 0);
    test(region, 8, 24, 50, 100, 200, 1);
    test(region, 500, 5000, 1000, 5000, 10000, 1);
    test(region, 8, 5000, 6000, 8000, 10000, 1);

    return 0;
}

