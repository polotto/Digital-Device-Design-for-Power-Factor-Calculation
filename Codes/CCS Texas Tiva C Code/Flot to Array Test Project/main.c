#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double PRECISION = 0.001;
static int MAX_NUMBER_STRING_SIZE = 32;

/**
 * Double to ASCII
 */
char * dtoa(char *s, double n) {
    // handle special cases
    if (isnan(n)) {
        strcpy(s, "nan");
    } else if (isinf(n)) {
        strcpy(s, "inf");
    } else if (n == 0.0) {
        strcpy(s, "0");
    } else {
        int digit, m, m1;
        char *c = s;
        int neg = (n < 0);
        if (neg)
            n = -n;
        // calculate magnitude
        m = log10(n);
        int useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
        if (neg)
            *(c++) = '-';
        // set up for scientific notation
        if (useExp) {
            if (m < 0)
               m -= 1.0;
            n = n / pow(10.0, m);
            m1 = m;
            m = 0;
        }
        if (m < 1.0) {
            m = 0;
        }
        // convert the number
        while (n > PRECISION || m >= 0) {
            double weight = pow(10.0, m);
            if (weight > 0 && !isinf(weight)) {
                digit = floor(n / weight);
                n -= (digit * weight);
                *(c++) = '0' + digit;
            }
            if (m == 0 && n > 0)
                *(c++) = '.';
            m--;
        }
        if (useExp) {
            // convert the exponent
            int i, j;
            *(c++) = 'e';
            if (m1 > 0) {
                *(c++) = '+';
            } else {
                *(c++) = '-';
                m1 = -m1;
            }
            m = 0;
            while (m1 > 0) {
                *(c++) = '0' + m1 % 10;
                m1 /= 10;
                m++;
            }
            c -= m;
            for (i = 0, j = m-1; i<j; i++, j--) {
                // swap without temporary
                c[i] ^= c[j];
                c[j] ^= c[i];
                c[i] ^= c[j];
            }
            c += m;
        }
        *(c) = '\0';
    }
    return s;
}

float offCalc(float *V, unsigned int size){
    float min = V[0];
    float max = V[0];
    int i=0;

    for (i=1;i<size;i++){
        if (V[i] > max){
            max = V[i];
        }
        else if (V[i] < min){
            min = V[i];
        }
    }
    //printf("min max: %f %f\n", min, max);
    return max-min;
}

int main()
{
    char pcBuffer[30], *aux;
    float Value=-1.3;
    float V[11];
    int i;
    srand( (unsigned) time(NULL) );
    for (i=0;i<11;i++)
       V[i]=(float)rand( )/(float) 100;
    //aux=floatToString(Value);
    //ftoa(Value, pcBuffer);
    //ftoa(-1.23, pcBuffer, 30, 2);
    dtoa(pcBuffer, Value);
    printf("Hello world!\n");
    printf("string %s", pcBuffer);
    for (i=1;i<11;i++)
       printf("%f\n", V[i]);
    printf("offCalc %f", offCalc(V,11));
    return 0;
}
