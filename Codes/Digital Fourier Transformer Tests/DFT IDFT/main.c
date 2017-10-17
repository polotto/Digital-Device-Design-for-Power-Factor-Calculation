#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include "Conv.h"

int main()
{
    int i,txn,thn, ML;
    char op;
    do{

        printf("Insira o tamanho de x(n): ");
        scanf("%d", &txn);

        printf("Insira o tamanho de h(n): ");
        scanf("%d", &thn);

        int xn[txn], hn[thn];

        ML=thn+txn -1; //usei -2 para testar o aliasing

        Read(xn,txn,hn,thn);

        double complex xk[ML], hk[ML], yk[ML], yn[ML];

        for(i=0;i<ML;i++){
            xk[i]= 0 + 0*I;
            hk[i]= 0 + 0*I;
            yk[i]= 0 + 0*I;
            yn[i]= 0 + 0*I;
        }

        DFT_Point(xk,xn,txn,ML);
        DFT_Point(hk,hn,thn,ML);

        for(i=0;i<ML;i++)
            yk[i]= xk[i] * hk[i];

        IDFT_Point(yk,yn,ML);

        Print(xk,hk,yk,ML,yn);

        fflush(stdin);
        printf("\n\nFazer novamente? <s> sim  <n> nao\n: ");
        scanf("%c",&op);
        system("cls");

    }while(op!='n');

    return 0;
}


