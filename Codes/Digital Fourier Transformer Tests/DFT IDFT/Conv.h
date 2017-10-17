#ifndef CONV_H_INCLUDED
#define CONV_H_INCLUDED

void DFT_Point(double complex func[], int fn[], int tam, int ML);

void IDFT_Point(double complex yk[], double complex yn[],int ML);

void Print(double complex xk[],double complex hk[],double complex yk[], int ML, double complex yn[]);

void Read(int xn[], int txn, int hn[], int thn);

#endif // CONV_H_INCLUDED
