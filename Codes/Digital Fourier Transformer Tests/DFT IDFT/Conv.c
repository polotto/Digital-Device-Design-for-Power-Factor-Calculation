#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include "Conv.h"

#define PI 3.141592653589793238462643383279502884

//***********************************************************************************

void DFT_Point(double complex func[], int fn[], int tam, int ML)
{
    int i,j;
    double complex comp;
    int n=0,k=0;

    for(j=0;j<ML;j++)
    {
        comp = 0 + 0*I;
        n=0;

        for(i=0;i<tam;i++,n++)
            comp = comp + fn[i]*cexp(((-2*PI*k*n)/ML)*I);

        k++;
        func[j] = comp;
    }

}
//***********************************************************************************

void IDFT_Point(double complex yk[], double complex yn[],int ML)
{
    int i,j;
    double complex comp;
    int n=0,k=0;

    for(j=0;j<ML;j++)
    {
        comp = 0 + 0*I;
        k=0;

        for(i=0;i<ML;i++,k++){
            comp = comp + yk[k]*cexp(((2*PI*k*n)/ML)*I);
        }

        n++;
        yn[j] = comp/(ML);

        printf("YN(%d): %.2f %+.2fi\n",j, creal(yn[j]), cimag(yn[j]));

    }

}
//***********************************************************************************

void Print(double complex xk[],double complex hk[],double complex yk[], int ML, double complex yn[])
{
    int i;
    printf("\n\n\n");

    for(i=0;i<ML;i++)
        printf("Xk(%d): %.2f %+.2fi\n",i, creal(xk[i]), cimag(xk[i]));

    printf("\n\n\n");

    for(i=0;i<ML;i++)
        printf("Hk(%d): %.2f %+.2fi\n",i, creal(hk[i]), cimag(hk[i]));

    printf("\n\n\n");

    for(i=0;i<ML;i++)
        printf("yk(%d): %.2f %+.2fi\n",i, creal(yk[i]), cimag(yk[i]));

    printf("\n\n\n  y(n) = {");

    for(i=0;i<=ML;i++){
        printf("%.0f", creal(yn[i]));

        if(i==ML-1)
            break;

        printf(",");
    }

    printf("}\n\n\n");

}
//***********************************************************************************

void Read(int xn[], int txn, int hn[], int thn)
{
    int i;

    printf("\n\n");

    for(i=0;i < txn; i++ )
    {
        printf("xn(%d): ",i);
        scanf("%d", &xn[i]);
    }

    fflush(stdin);
    printf("\n");

    for(i=0;i < thn; i++ )
    {
        printf("hn(%d): ",i);
        scanf("%d", &hn[i]);
    }
    printf("\n");
}



